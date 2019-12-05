/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : DMA1.c
**     Project     : MCUX_PE_KL25Z_FRTOS_ShieldwFatFS
**     Processor   : MKL25Z128VLK4
**     Component   : DMAController
**     Version     : Component 01.039, Driver 01.02, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-11-28, 13:40, # CodeGen: 38
**     Abstract    :
**          This embedded component implements initialization
**          and runtime handling of an on-chip DMA controller.
**     Settings    :
**          Component name                                 : DMA1
**          OnError                                        : no
**          OnComplete                                     : yes
**          RWTransactionSizeGtThenOne                     : yes
**          RWTransactionSize                              : 2
**          RWTransactionUnitSameInAllComponent            : yes
**          HasCycleStealMode                              : no
**          HasHalfCompleteInterrupt                       : no
**          Device                                         : DMA
**          Channel arbitration type                       : Fixed
**            Autoset channel priority                     : 
**          Statically allocated channels                  : 1
**            Channel settings                             : Chan: 0
**              Channel                                    : DMA_Channel0
**              Allocate channel                           : yes
**              Channel shared                             : yes
**              Used by component(s)                       : DacLdd1_DMA0
**              Arbitration type                           : Fixed
**                Priority                                 : 0
**              Interrupt service                          : Enabled
**                Transfer complete interrupt              : 
**                  Interrupt vector                       : INT_DMA0
**                  Interrupt priority                     : 2
**              Component name0                            : DacLdd1_DMA0
**          Dynamically allocatable channels               : 0
**     Contents    :
**         Init          - LDD_TDeviceData* DMA1_Init(void);
**         InitChannel   - LDD_TDeviceData* DMA1_InitChannel(LDD_TDeviceData *DeviceDataPtr,...
**         EnableRequest - LDD_TError DMA1_EnableRequest(DMA1_TChanDeviceData *ChanDeviceDataPtr);
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
** @file DMA1.c
** @version 01.02
** @brief
**          This embedded component implements initialization
**          and runtime handling of an on-chip DMA controller.
*/         
/*!
**  @addtogroup DMA1_module DMA1 module documentation
**  @{
*/         

/* MODULE DMA1. */

#include "DMA1.h"
#include "SIM_PDD.h"

#include "FreeRTOS.h" /* FreeRTOS interface */
/* Total number of channel supported by DMA device */
#define DMA1_CHANNEL_COUNT                                  0x01U
/* Number of statically allocated DMA channels */
#define DMA1_STATIC_CHANNEL_COUNT                           0x01U
/* Number of dynamically allocated DMA channels */
#define DMA1_DYNAMIC_CHANNEL_COUNT                          0x00U

/* Logical channel constants */
#define DMA1_CHN_0_PHY_NUM                                  (0U)
#define DMA1_CHN_0_TCD_PTR                                  ((DMA1_TTCD*)(void*)&DMA_SAR0)
#define DMA1_CHN_0_DMAMUX_CFG_REG_PTR                       ((uint8_t*)(void*)&DMAMUX0_CHCFG0)

/* Channel MUX macros */
#define DMA1__MuxReset(_DMAMUX_CFG_PTR)                     (*_DMAMUX_CFG_PTR = 0U)
#define DMA1__MuxDisableRequest(_DMAMUX_CFG_PTR)            (*_DMAMUX_CFG_PTR &= (uint8_t)~(uint8_t)DMAMUX_CHCFG_ENBL_MASK)
#define DMA1__MuxEnableRequest(_DMAMUX_CFG_PTR)             (*_DMAMUX_CFG_PTR |= DMAMUX_CHCFG_ENBL_MASK)
#define DMA1__MuxSetRequest(_DMAMUX_CFG_PTR, _REQ)          (*_DMAMUX_CFG_PTR = _REQ)

#define DMA1__CHN_ERROR_FLAGS  (DMA_DSR_BCR_BED_MASK | DMA_DSR_BCR_BES_MASK | DMA_DSR_BCR_CE_MASK)

/* Channel TCD macros */
#define DMA1__StartTransfer(_TCD_PTR)                       (_TCD_PTR->DMA_DCR_Reg |= DMA_DCR_START_MASK)
#define DMA1__CancelTransfer(_TCD_PTR)                      (_TCD_PTR->DMA_DSR_BCR_Reg |= DMA_DSR_BCR_DONE_MASK)
#define DMA1__ClearDoneFlag(_TCD_PTR)                       (_TCD_PTR->DMA_DSR_BCR_Reg |= DMA_DSR_BCR_DONE_MASK)
#define DMA1__EnableRequest(_TCD_PTR)                       (_TCD_PTR->DMA_DCR_Reg |= DMA_DCR_ERQ_MASK)
#define DMA1__DisableRequest(_TCD_PTR)                      (_TCD_PTR->DMA_DCR_Reg &= ~(uint32_t)DMA_DCR_ERQ_MASK)
#define DMA1__GetByteCount(_TCD_PTR)                        (_TCD_PTR->DMA_DSR_BCR_Reg & DMA_DSR_BCR_BCR_MASK)
#define DMA1__SetByteCount(_TCD_PTR, _ByteCount)            (_TCD_PTR->DMA_DSR_BCR_Reg = (_ByteCount & DMA_DSR_BCR_BCR_MASK))
#define DMA1__GetStatus(_TCD_PTR)                           (_TCD_PTR->DMA_DSR_BCR_Reg & ~(uint32_t)DMA_DSR_BCR_BCR_MASK)
#define DMA1__GetDsrBcrReg(_TCD_PTR)                        (_TCD_PTR->DMA_DSR_BCR_Reg)
#define DMA1__SetSrcDataTransferSize(_TCD_PTR, _Size)       (_TCD_PTR->DMA_DCR_Reg = (_TCD_PTR->DMA_DCR_Reg & ~(uint32_t)DMA_DCR_SSIZE_MASK) | (((uint32_t)(_Size)) << DMA_DCR_SSIZE_SHIFT))
#define DMA1__GetSrcDataTransferSize(_TCD_PTR)              ((_TCD_PTR->DMA_DCR_Reg & DMA_DCR_SSIZE_MASK) >> DMA_DCR_SSIZE_SHIFT)
#define DMA1__SetDstDataTransferSize(_TCD_PTR, _Size)       (_TCD_PTR->DMA_DCR_Reg = (_TCD_PTR->DMA_DCR_Reg & ~(uint32_t)DMA_DCR_DSIZE_MASK) | (((uint32_t)(_Size)) << DMA_DCR_DSIZE_SHIFT))
#define DMA1__GetDstDataTransferSize(_TCD_PTR)              ((_TCD_PTR->DMA_DCR_Reg & DMA_DCR_DSIZE_MASK) >> DMA_DCR_DSIZE_SHIFT)
#define DMA1__SetSrcAddress(_TCD_PTR, _Address)             (_TCD_PTR->DMA_SAR_Reg = _Address)
#define DMA1__SetDstAddress(_TCD_PTR, _Address)             (_TCD_PTR->DMA_DAR_Reg = _Address)
#define DMA1__SetCycleStealMode(_TCD_PTR)                   (_TCD_PTR->DMA_DCR_Reg |= DMA_DCR_CS_MASK)
#define DMA1__SetContinuousMode(_TCD_PTR)                   (_TCD_PTR->DMA_DCR_Reg &= ~(uint32_t)DMA_DCR_CS_MASK)

/* Channel constants structure prototype */
typedef struct DMA1_TChnDevConst_struct {
  uint32_t                         PhyChnNum;              /*!< Physical channel number */
  DMA1_TTCD                       *TCDPtr;                 /*!< TCD registers address */
  uint8_t                         *DmaMuxRegPtr;           /*!< DMA mux. register address */
} DMA1_TChnDevConst, *DMA1_TChnDevConstPtr;

/* Device constants structure prototype */
typedef struct DMA1_TDevConst_struct {
  DMA1_TChnDevConst const          ChnDevConst[DMA1_CHANNEL_COUNT];
} DMA1_TDevConst, *DMA1_TDevConstPtr;

/* Channel device data structure prototype */
typedef struct DMA1_TChnDevData_stuct {
  DMA1_TTCD                       *TCDPtr;                 /*!< Channel TCD address */
  DMA1_TChnDevConst const         *ChnDevConstPtr;         /*!< Channel device constants structure address */
  LDD_TUserData                   *UserDataPtr;            /*!< User data pointer */
  DMA1_TEvents                     Events;                 /*!< Events state variable */
} DMA1_TChnDevData, *DMA1_TChnDevDataPtr;

/* Device data structure prototype */
typedef struct {
  DMA1_TChnDevData                 ChnDevData[DMA1_CHANNEL_COUNT]; /*!< Channel device data array */
} DMA1_TDeviceData, *DMA1_TDeviceDataPtr;

/* Channel constants array */
DMA1_TDevConst const DMA1_DevConst = {
  {
    { /* Channel 0 (DMA_Channel0) */
      DMA1_CHN_0_PHY_NUM,                                  /*!< Physical channel number */
      DMA1_CHN_0_TCD_PTR,                                  /*!< TCD registers address */
      DMA1_CHN_0_DMAMUX_CFG_REG_PTR                        /*!< Mux cfg. register address */
    }
  }
};


/* {FreeRTOS RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static DMA1_TDeviceData DevDataPtr__DEFAULT_RTOS_ALLOC;
/* {FreeRTOS RTOS Adapter} Global variable used for passing a parameter into ISR */
static DMA1_TDeviceDataPtr INT_DMA0__BAREBOARD_RTOS_ISRPARAM;
static void HandleInterrupt(DMA1_TChnDevData *ChnDevDataPtr);
/*
** ===================================================================
**     Method      :  DMA1_Init (component DMAController)
*/
/*!
**     @brief
**         Initializes the associated peripheral(s) and the component
**         internal variables. The method is called automatically as a
**         part of the application initialization code.
**     @return
**                         - Pointer to the device data structure. 
*/
/* ===================================================================*/
LDD_TDeviceData* DMA1_Init(void)
{
  DMA1_TDeviceData                 *DevDataPtr;            /* LDD device structure */

  /* {FreeRTOS RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DevDataPtr = &DevDataPtr__DEFAULT_RTOS_ALLOC;
  /* {FreeRTOS RTOS Adapter} Driver memory allocation: Fill the allocated memory by zero value */
  PE_FillMemory(DevDataPtr, 0U, sizeof(DMA1_TDeviceData));
  /* Transfer complete interrupt vector(INT_DMA0) allocation */
  /* {FreeRTOS RTOS Adapter} Set interrupt vector: IVT is static, ISR parameter is passed by the global variable */
  INT_DMA0__BAREBOARD_RTOS_ISRPARAM =  DevDataPtr;
  /* Transfer complete interrupt vector(INT_DMA0) priority setting */
  /* NVIC_IPR0: PRI_0=0x80 */
  NVIC_IPR0 = (uint32_t)((NVIC_IPR0 & (uint32_t)~(uint32_t)(
               NVIC_IP_PRI_0(0x7F)
              )) | (uint32_t)(
               NVIC_IP_PRI_0(0x80)
              ));
  /* NVIC_ISER: SETENA|=1 */
  NVIC_ISER |= NVIC_ISER_SETENA(0x01);
  /* Enable clock gates */
  #ifdef SIM_PDD_CLOCK_GATE_DMA
  SIM_PDD_SetClockGate(SIM_BASE_PTR, SIM_PDD_CLOCK_GATE_DMA, PDD_ENABLE);
  #endif
  #ifdef SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0
  SIM_PDD_SetClockGate(SIM_BASE_PTR, SIM_PDD_CLOCK_GATE_DMA_MULTIPLEXOR0, PDD_ENABLE);
  #endif
  /* Registration of the device structure */
  PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_DMA1_ID,DevDataPtr);
  return DevDataPtr;
}

/*
** ===================================================================
**     Method      :  DMA1_InitChannel (component DMAController)
*/
/*!
**     @brief
**         The method allocates DMA channel for the DMA transfer
**         defined by the transfer descriptor input parameter
**         DescriptorPtr. This method must be called before DMA channel
**         is initialized. Otherwise ERR_NOTAVAIL error code is
**         returned after call of DMA_LDD channel methods.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         ChIntPtr        - Pointer to a DMA transfer
**                           descriptor structure.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an event or callback parameter.
**     @return
**                         - Error code, possible codes:
**                           - ERR_OK - OK.
**                           - ERR_SPEED - This device does not work in
**                           the active clock configuration.
**                           - ERR_DISABLED - Component or device is
**                           disabled.
**                           - ERR_NOTAVAIL - There is not any possible
**                           channel with desired features to be
**                           allocated.
*/
/* ===================================================================*/
LDD_TDeviceData* DMA1_InitChannel(LDD_TDeviceData *DeviceDataPtr, DMA1_TChnInit *ChIntPtr, LDD_TUserData *UserDataPtr)
{
  uint32_t                         LogChnNum      = ChIntPtr->ChnNum;
  DMA1_TChnDevData                *ChnDevDataPtr;
  DMA1_TChnDevConst const         *ChnDevConstPtr;
  DMA1_TTCD                       *TCDPtr;
  uint8_t                         *DmaMuxCfgPtr;

  ChnDevDataPtr  = &((DMA1_TDeviceData *)DeviceDataPtr)->ChnDevData[LogChnNum];
  ChnDevConstPtr = &(DMA1_DevConst.ChnDevConst[LogChnNum]);
  TCDPtr         = ChnDevConstPtr->TCDPtr;
  DmaMuxCfgPtr   = ChnDevConstPtr->DmaMuxRegPtr;
  DMA1__MuxReset(DmaMuxCfgPtr);                            /* Disable channel peripheral request (in the DMAMUX) */
  DMA1__CancelTransfer(TCDPtr);                            /* Cancel transfer (if pending) */
  *TCDPtr = ChIntPtr->TCD;                                 /* Initialize channel TCD (channel registers) */
  ChnDevDataPtr->ChnDevConstPtr = ChnDevConstPtr;          /* Remember channel constants address */
  ChnDevDataPtr->TCDPtr = TCDPtr;                          /* Remember channel TCD address */
  ChnDevDataPtr->UserDataPtr = UserDataPtr;                /* Remember channel User data pointer */
  ChnDevDataPtr->Events = ChIntPtr->Events;                /* Initialize events */
  DMA1__MuxSetRequest(DmaMuxCfgPtr,ChIntPtr->MUX_CHCFG_Reg); /* Set channel peripheral request (in the DMAMUX) */
  return ChnDevDataPtr;
}

/*
** ===================================================================
**     Method      :  DMA1_EnableRequest (component DMAController)
*/
/*!
**     @brief
**         The method enables DMA request from peripheral. Please note
**         that this method doesn't start the transfer. The transfer is
**         started as soon as DMA request from peripheral is asserted.
**     @param
**         ChanDeviceDataPtr - DMA channel
**                           data structure pointer returned by
**                           [InitChannel()] method.
**     @return
**                         - Error code, possible codes: 
**                           - ERR_OK - OK. 
**                           - ERR_DISABLED - Component is disabled.
*/
/* ===================================================================*/
LDD_TError DMA1_EnableRequest(DMA1_TChanDeviceData *ChanDeviceDataPtr)
{
  DMA1_TChnDevData                *ChnDevDataPtr = (DMA1_TChnDevData *)ChanDeviceDataPtr;

  /* Enable request from peripheral */
  DMA1__EnableRequest(ChnDevDataPtr->TCDPtr);
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  HandleInterrupt (component DMAController)
**
**     Description :
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HandleInterrupt(DMA1_TChnDevData *ChnDevDataPtr)
{
  DMA1_TTCD                       *TCDPtr = ChnDevDataPtr->TCDPtr;
  uint32_t                         DsrBcrRegCopy;

  /* Create copy of status and byte count register */
  DsrBcrRegCopy = DMA1__GetDsrBcrReg(TCDPtr);
  if ((DsrBcrRegCopy & DMA_DSR_BCR_DONE_MASK) != 0x00U) { /* Transfer done? */
    /* Clear all status bits in the status register. */
    /* Note: This also clears byte count register */
    DMA1__ClearDoneFlag(TCDPtr);
    /* Transfer done without error */
    if (ChnDevDataPtr->Events.OnCompleteFnPtr != NULL) {
      ChnDevDataPtr->Events.OnCompleteFnPtr(ChnDevDataPtr->UserDataPtr);
    }
  }
}

/*
** ===================================================================
**     Method      :  DMA1_TransferComplete_ISR (component DMAController)
**
**     Description :
**         Transfer complete ISR
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
PE_ISR(DMA1_INT_DMA0_TransferComplete_ISR)
{
  /* {FreeRTOS RTOS Adapter} ISR parameter is passed through the global variable */
  DMA1_TDeviceDataPtr DevDataPtr = INT_DMA0__BAREBOARD_RTOS_ISRPARAM;
  HandleInterrupt(&(DevDataPtr->ChnDevData[0]));
}

/* END DMA1. */

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
