#include <stdint.h>
#include <MKL25Z4.h>
#include <math.h>

#include "FRTOS1.h"
#include "mytasks.h"
#include "sound.h"
#include "misc.h"
#include "gpio_defs.h"
#include "timers.h"
#include "AudioAmp.h"
#include "WAIT1.h"
#include "DacLdd1.h"
//#include "TPM0.h"
#include "DMAMUX_PDD.h"

uint16_t * Reload_DMA_Source[2]={0,0};
uint32_t Reload_DMA_Byte_Count=0;
uint32_t DMA_Playback_Count=0;
uint8_t read_buffer_num=0;

// already initialized
/*void DAC_Init(void) {
  // Init DAC output

	SIM->SCGC6 |= MASK(SIM_SCGC6_DAC0_SHIFT);
	SIM->SCGC5 |= MASK(SIM_SCGC5_PORTE_SHIFT);

	PORTE->PCR[DAC_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[DAC_POS] |= PORT_PCR_MUX(0);	// Select analog

	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;

	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = MASK(DAC_C0_DACEN_SHIFT) | MASK(DAC_C0_DACRFS_SHIFT);
}*/

void SineTable_Init(void) {
	unsigned n;

	for (n=0; n<NUM_STEPS; n++) {
		SineTable[n] = (MAX_DAC_CODE/2)*sinf(n*(2*3.1415927/NUM_STEPS));
	}
}

/* Fill waveform buffers with silence. */
void Init_Waveform(void) {
	uint32_t i;

	for (i=0; i<NUM_WAVEFORM_SAMPLES; i++) {
		Waveform[0][i] = (MAX_DAC_CODE/2);
		Waveform[1][i] = (MAX_DAC_CODE/2);
	}
}

void Init_Voices(void) {
	uint16_t i;

	for (i=0; i<NUM_VOICES; i++) {
		Voice[i].Volume = 0;
		Voice[i].Decay = 0;
		Voice[i].Duration = 0;
		Voice[i].Period = 0;
		Voice[i].Counter = 0;
		Voice[i].CounterIncrement = 0;
		Voice[i].Type = VW_UNINIT;
	}
}

/*void TPM0_Init(void) {
	//turn on clock to TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

	//set clock source for tpm
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
}*/

void TPM0_Start(void) {
// Enable counter
	TPM0->SC |= TPM_SC_CMOD(1);
}

void Configure_TPM0_for_DMA(uint32_t period_us) {

	// disable TPM
	TPM0->SC = 0;
	//load the counter and mod
	TPM0->MOD = TPM_MOD_MOD(period_us*48);
	//set TPM to count up and divide by 1 prescaler and clock mode
	TPM0->SC = (TPM_SC_DMA_MASK | TPM_SC_PS(0));

}

static void InitDMA(void) {
  /* enable DMA MUX0: */
  DMAMUX_PDD_EnableChannel(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE); /* enable DMA MUX0 */
  /* PIT triggering for DMA0: */
  DMAMUX_PDD_EnableTrigger(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_DISABLE); /* disable PIT Trigger */
  /* use TPM0 overflow for DMA0 request: */
  DMAMUX_PDD_SetChannelSource(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, 54); /* KL25Z reference manual, 3.4.8.1, p64: source number 43 TPM0 Overflow DMA source */


  /* DMA channel 0 source configuration: */
  DMA_PDD_SetSourceAddress(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, (uint32_t) Reload_DMA_Source[read_buffer_num]); /* set source address */
  DMA_PDD_SetSourceAddressModulo(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, DMA_PDD_CIRCULAR_BUFFER_DISABLED); /* no circular buffer */
  DMA_PDD_EnableSourceAddressIncrement(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_DISABLE); /* source address will be incremented by transfer size */
  DMA_PDD_SetSourceDataTransferSize(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, DMA_PDD_8_BIT); /* Transfer size from source  */

  /* DMA channel 0 destination configuration: */
  DMA_PDD_SetDestinationAddress(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, (uint32_t) (&(DAC0->DAT[0]))); /* set destination address */
  DMA_PDD_SetDestinationAddressModulo(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, DMA_PDD_CIRCULAR_BUFFER_DISABLED); /* no circular buffer */
  DMA_PDD_EnableDestinationAddressIncrement(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE); /* auto-increment for destination address */
  DMA_PDD_SetDestinationDataTransferSize(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, DMA_PDD_8_BIT); /* Transfer to destination size */

  /* DMA channel 0 transfer configuration: */
  DMA_PDD_EnableTransferCompleteInterrupt(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE); /* request interrupt at the end of the DMA transfer */
  (void)DMA_PDD_GetRequestAutoDisableEnabled(DMA_BASE_PTR, DMA_PDD_CHANNEL_0); /* disable DMA request at the end of the sequence */
}

/* Initialize sound hardware, sine table, and waveform buffer. */
void Sound_Init(void) {
	SineTable_Init();
	Init_Waveform();
	Init_Voices();
	write_buffer_num = 0; // Start writing to waveform buffer 0

	//DacLdd1_Init(NULL);

	//hopefully already initialized
	//DAC_Init();
	InitDMA();
	/*TPM0_Init();
	Configure_TPM0_for_DMA(AUDIO_SAMPLE_PERIOD_US);

	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);*/

	/*SIM->SCGC5 |= (1UL << SIM_SCGC5_PORTE_SHIFT);

	PORTE->PCR[AMP_ENABLE_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[AMP_ENABLE_POS] |= PORT_PCR_MUX(1);	// Select GPIO
	PTE->PDDR |= MASK(AMP_ENABLE_POS); // set to output
	PTE->PSOR = MASK(AMP_ENABLE_POS);  // enable audio amp*/

	// amp should already be initialized, just turn on
	Sound_Enable_Amp();

}

void Sound_Enable_Amp(void) {
	AudioAmp_SetVal(NULL);
}

void Sound_Disable_Amp(void) {
	AudioAmp_ClrVal(NULL);
}

void ShortDelay (uint32_t dly) {
  volatile uint32_t t;

	for (t=dly; t>0; t--)
		;
}

/* Simple audio test function using busy-waiting. */
void Play_Tone(void) {
	int i, d=MAX_DAC_CODE>>7, n;
	int p[12] = {813, 1150, 813, 813, 813, 1150, 813, 813, 813, 813, 1150, 1150};
	int c[12] = {800, 283,  800, 800, 800, 283,  800, 800, 800, 800,  283, 283};

#if 0
	for (p=5; p>=1; p--) {
		for (n=0; n<20/p; n++) {
			Play_Sound_Sample((MAX_DAC_CODE>>1)+d);
			Delay(p);
			Play_Sound_Sample((MAX_DAC_CODE>>1)-d);
			Delay(p);
		}
	}
#else
	for (i=0; i<4; i++) {
		n = c[i];
		while (n--) {
			DacLdd1_SetValue(NULL,(MAX_DAC_CODE>>1)+d);
			ShortDelay(p[i]);
			DacLdd1_SetValue(NULL,(MAX_DAC_CODE>>1)-d);
			ShortDelay(p[i]);
		}
		WAIT1_Waitms(40);
	}
#endif
}

int16_t Sound_Generate_Next_Sample (VOICE_T *voice) {
	uint16_t lfsr;
	uint16_t bit;
	int16_t sample;

	switch (voice->Type) {
		case VW_NOISE:
			lfsr = voice->Counter;
			// source code from http://en.wikipedia.org/wiki/Linear_feedback_shift_register
			/* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
			bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
			lfsr =  (lfsr >> 1) | (bit << 15);
			voice->Counter = lfsr;
			sample = (lfsr >> 4) - (MAX_DAC_CODE/2); // scale to get 12-bit value
			break;
		case VW_SQUARE:
			if (voice->Counter < voice->Period/2) {
				sample = MAX_DAC_CODE/2 - 1;
			} else {
				sample = -MAX_DAC_CODE/2;
			}
			voice->Counter++;
			if (voice->Counter == voice->Period) {
				voice->Counter = 0;
			}
			break;
		case VW_SINE:
			sample = SineTable[((voice->Counter)/256)]; // & (NUM_STEPS-1)];
			voice->Counter += voice->CounterIncrement;
			if (voice->Counter > voice->Period * voice->CounterIncrement){
				voice->Counter = 0;
			}
			break;
		default:
			sample = 0;
			break;
	}
	return sample;
}

void Play_Waveform_with_DMA(void) {
	Configure_DMA_For_Playback(Waveform[0], Waveform[1], NUM_WAVEFORM_SAMPLES, 1);
	Start_DMA_Playback();
}

void Configure_DMA_For_Playback(uint16_t * source1, uint16_t * source2, uint32_t count, uint32_t num_playbacks) {

	// Disable DMA channel in order to allow changes
	DMAMUX_PDD_EnableChannel(DMAMUX0_BASE_PTR, 0, PDD_DISABLE); /* disable DMA MUX0 */

	Reload_DMA_Source[0] = source1;
	Reload_DMA_Source[1] = source2;
	read_buffer_num = 0;

	Reload_DMA_Byte_Count = count*2;
	DMA_Playback_Count = num_playbacks;

	// Generate DMA interrupt when done
	// Increment source, transfer words (16 bits)
	// Enable peripheral request
	/*DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_SINC_MASK |
											DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) |
											DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK;*/



	DMA_PDD_EnablePeripheralRequest(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE); /* enable request from peripheral */
	DMA_PDD_EnableSourceAddressIncrement(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE);
	DMA_PDD_SetSourceDataTransferSize(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, 2);
	DMA_PDD_SetDestinationDataTransferSize(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, 2);
	//DMA_PDD_EnableRequestMask(DMA_BASE_PTR, PDD_ENABLE);
	DMA_PDD_EnableCycleSteal(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE);


	//DMA_PDD_SetSourceAddressModulo
	//DMA_PDD_SetDestinationAddressModulo


	// Enable DMA MUX channel without periodic triggering
	// select TPM0 overflow as trigger
	DMAMUX_PDD_SetChannelSource(DMAMUX0_BASE_PTR, 0, 54); /* KL25Z reference manual, 3.4.8.1, p64: source number 43 TPM0 Overflow DMA source */


}

void Start_DMA_Playback() {

	// initialize source and destination pointers
	DMA_PDD_SetSourceAddress(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, (uint32_t) Reload_DMA_Source[read_buffer_num]); /* set source address */
	DMA_PDD_SetDestinationAddress(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, (uint32_t) (&(DAC0->DAT[0]))); /* set destination address */

	// byte count
	DMA_PDD_SetByteCount(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, Reload_DMA_Byte_Count); /* set number of bytes to transfer */

	// Select TPM0 as trigger for DMA
	DMAMUX_PDD_SetChannelSource(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, 54); /* KL25Z reference manual, 3.4.8.1, p64: source number 43 TPM0 Overflow DMA source */


	// verify done flag is cleared
	//DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;

	// Enable DMA
	DMAMUX_PDD_EnableChannel(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE); /* enable DMA MUX0 */

	// start the timer running
  TPM_PDD_ClearOverflowInterruptFlag(TPM0_BASE_PTR);
  //TPM_PDD_InitializeCounter(TPM0_BASE_PTR); /* reset timer counter */
  //TPM_PDD_SelectPrescalerSource(TPM0_BASE_PTR, TPM_PDD_SYSTEM); /* enable timer so I can reset the value below */

}
