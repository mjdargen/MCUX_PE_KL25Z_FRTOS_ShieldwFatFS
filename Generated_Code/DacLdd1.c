/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : DacLdd1.c
**     Project     : MCUX_PE_KL25Z_FRTOS_ShieldwFatFS
**     Processor   : MKL25Z128VLK4
**     Component   : DAC_LDD
**     Version     : Component 01.084, Driver 01.09, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-11-28, 13:40, # CodeGen: 38
**     Abstract    :
**         This component implements an internal D/A converter of the MCU.
**         It contains settings for converting various format of a values
**         to supported format of the D/A converter.
**
**     Settings    :
**          Component name                                 : DacLdd1
**          D/A converter                                  : DAC0
**          Interrupt service/event                        : Enabled
**            D/A interrupt                                : INT_DAC0
**            D/A interrupt priority                       : medium priority
**          Output pin                                     : yes
**            D/A channel (pin)                            : DAC0_OUT/ADC0_SE23/CMP0_IN4/PTE30/TPM0_CH3/TPM_CLKIN1
**            D/A channel (pin) signal                     : 
**          Init value                                     : 0
**          D/A resolution                                 : 12 bits
**          Data mode                                      : unsigned 16 bits, right justified
**          Low power mode                                 : Disabled
**          Voltage reference source                       : internal
**          Hardware buffer                                : Disabled
**          DMA                                            : Enabled
**            DMA requests                                 : 
**              Buffer start                               : Disabled
**              Buffer end                                 : Disabled
**            DMA version                                  : Recommended
**              DMA Channel                                : DMAChannel
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Auto initialization                          : yes
**            Event mask                                   : 
**              OnBufferEnd                                : Disabled
**              OnBufferStart                              : Disabled
**              OnComplete                                 : Enabled
**              OnError                                    : Disabled
**          CPU clock/configuration selection              : 
**            Clock configuration 0                        : This component enabled
**            Clock configuration 1                        : This component disabled
**            Clock configuration 2                        : This component disabled
**            Clock configuration 3                        : This component disabled
**            Clock configuration 4                        : This component disabled
**            Clock configuration 5                        : This component disabled
**            Clock configuration 6                        : This component disabled
**            Clock configuration 7                        : This component disabled
**     Contents    :
**         Init     - LDD_TDeviceData* DacLdd1_Init(LDD_TUserData *UserDataPtr);
**         SetValue - LDD_TError DacLdd1_SetValue(LDD_TDeviceData *DeviceDataPtr, LDD_DAC_TData Data);
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file DacLdd1.c
** @version 01.09
** @brief
**         This component implements an internal D/A converter of the MCU.
**         It contains settings for converting various format of a values
**         to supported format of the D/A converter.
**
*/         
/*!
**  @addtogroup DacLdd1_module DacLdd1 module documentation
**  @{
*/         

/* MODULE DacLdd1. */

#include "FreeRTOS.h" /* FreeRTOS interface */
#include "Events.h"
#include "DacLdd1.h"

#ifdef __cplusplus
extern "C" {
#endif 


typedef struct {
  LDD_TDeviceData *DmaTransferDeviceDataPtr; /* DMATransfer device data structure */
  LDD_TUserData *UserDataPtr;          /* RTOS device data structure */
} DacLdd1_TDeviceData;                 /* Device data structure type */

/* {FreeRTOS RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static DacLdd1_TDeviceData DeviceDataPtr__DEFAULT_RTOS_ALLOC;
/*
** ===================================================================
**     Method      :  DacLdd1_Init (component DAC_LDD)
*/
/*!
**     @brief
**         Initializes the device according to design-time
**         configuration properties. Allocates memory for the device
**         data structure. 
**         If the [Enable in init. code] is set to "yes" then the
**         device is also enabled (see the description of the Enable
**         method).
**         This method can be called only once. Before the second call
**         of Init the Deinit method must be called first. If DMA
**         service is enabled this method also initializes inherited
**         DMA Transfer component.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an events or callback parameter.
**     @return
**                         - Device data structure pointer.
*/
/* ===================================================================*/
LDD_TDeviceData* DacLdd1_Init(LDD_TUserData *UserDataPtr)
{
  DacLdd1_TDeviceData *DeviceDataPtr;  /* LDD device structure */
  LDD_TDeviceData *DmaTransferDeviceDataPtr; /* Auxiliary local pointer to inherited DMATransfer component LDD device structure. */
  LDD_TError Error;                    /* Auxiliary local error variable. */

  /* Allocate HAL device structure */
  /* {FreeRTOS RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DeviceDataPtr = &DeviceDataPtr__DEFAULT_RTOS_ALLOC;
  DmaTransferDeviceDataPtr = DacLdd1_DMA0_Init(DeviceDataPtr); /* Initialize DmaChannel inherited component */    
    if (DmaTransferDeviceDataPtr == NULL) { /* DMATransfer inherited component initialized successfully? */
      return NULL;                     /* If no, return with NULL. */
    }
  Error = DacLdd1_DMA0_EnableRequest(DmaTransferDeviceDataPtr);
  if (Error != ERR_OK) {               /* DMA channel allocated and enabled successfully? */
   return NULL;                        /* If no, return with NULL. */
  } else {
   DeviceDataPtr->DmaTransferDeviceDataPtr = DmaTransferDeviceDataPtr; /* If yes, save pointer to DMATransfer component structure */
  }    
  DeviceDataPtr->UserDataPtr = UserDataPtr; /* Store the RTOS device structure */
  /* SIM_SCGC6: DAC0=1 */
  SIM_SCGC6 |= SIM_SCGC6_DAC0_MASK;
  DAC_PDD_EnableDevice(DAC0_BASE_PTR,PDD_DISABLE); /* Disable device */
  /* Initialization of pin routing */
  /* PORTE_PCR30: ISF=0,MUX=0 */
  PORTE_PCR30 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
  /* DAC0_DAT0H: ??=0,??=0,??=0,??=0,DATA1=0 */
  DAC0_DAT0H = DAC_DATH_DATA1(0x00);
  /* DAC0_DAT0L: DATA0=0 */
  DAC0_DAT0L = DAC_DATL_DATA0(0x00);
  /* DAC0_C2: ??=0,??=0,??=0,DACBFRP=0,??=1,??=1,??=1,DACBFUP=1 */
  DAC0_C2 = (DAC_C2_DACBFUP_MASK | 0x0EU);
  /* DAC0_C1: DMAEN=1,??=0,??=0,??=0,??=0,DACBFMD=0,??=0,DACBFEN=0 */
  DAC0_C1 = DAC_C1_DMAEN_MASK;
  /* DAC0_SR: ??=0,??=0,??=0,??=0,??=0,??=0,DACBFRPTF=0,DACBFRPBF=0 */
  DAC0_SR = 0x00U;
  /* DAC0_C0: DACEN=1,DACRFS=0,DACTRGSEL=0,DACSWTRG=0,LPEN=0,??=0,DACBTIEN=0,DACBBIEN=0 */
  DAC0_C0 = DAC_C0_DACEN_MASK;
  /* Registration of the device structure */
  PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_DacLdd1_ID,DeviceDataPtr);
  return ((LDD_TDeviceData*)DeviceDataPtr); /* Return pointer to the data data structure */
}

/*
** ===================================================================
**     Method      :  DacLdd1_SetValue (component DAC_LDD)
*/
/*!
**     @brief
**         Sets DAC output voltage according to specified value.
**         Input data format is specified by [Data mode] property
**         settings. If selected formatting is not native for DAC
**         device which is used then any necessary transformations (e.g.
**         shifting) are done._/Note: This method is available only if
**         Data buffer is disabled./_
**     @param
**         DeviceDataPtr   - Pointer to device data
**                           structure.
**     @param
**         Data            - User data.
**     @return
**                         - Error code, possible codes:
**                           - ERR_OK - OK.
**                           - ERR_SPEED - This device does not work in
**                           the active clock configuration.
**                           - ERR_DISABLED - Component or device is
**                           disabled.
*/
/* ===================================================================*/
LDD_TError DacLdd1_SetValue(LDD_TDeviceData *DeviceDataPtr, LDD_DAC_TData Data)
{
  (void)DeviceDataPtr;                 /* Parameter not used, suppress not used argument warning */
  DAC_PDD_SetData(DAC0_BASE_PTR,(uint16_t)Data,0U);
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  DacLdd1_OnComplete (component DAC_LDD)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void DacLdd1_DMA0_OnComplete(LDD_TUserData *UserDataPtr)
{
  DacLdd1_OnComplete(((DacLdd1_TDeviceData*)UserDataPtr)->UserDataPtr); /* Call user event */
}



/* END DacLdd1. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
