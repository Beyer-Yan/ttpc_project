/**
  ******************************************************************************
  * @file    	virhw.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.09
  * @brief   	This file connects the low-level hardware and the high-level protocol
  ******************************************************************************
  * @attention
  *
  * @desc
  * For the platform portability,  it is necessary to  separate the low-level hardwares and
  * the high-level  protocol  services. The virtual  hardware level is set to transport the 
  * action coming from the upper level to the real hardware operations. Different platforms
  * have different hardware implementations, so the program generosity should be guaranteed
  * within  shielding the real hardware differences.
  * 
  * The clock is a very important notion of TTA, which determines the timing correctness of 
  * the protocol  services. The local  clock and  the the global clock are specified in the 
  * clock module. The DMA module is inessential, but it will reduce the memory access delay 
  * jitter and the utilization  of the CPU. the configuration of the DMA shall be static in 
  * TTP/C implementation,which means the channels of data streams within the priorities are
  * specified statically. The DMA should  be transparent in run  time of TTP/C. The TRIGGER 
  * module provides the concrete trigger-control unit in TDMA timing of MAC services, which 
  * contains the capture of the timestamps of the sending or the receiving messages,and the 
  * triggering point of the PSP,AT  and PRP actions.  The IRQ module is a manager center of 
  * hardware  interruptions.  And  the XFER  module is  the bridge of the  physical  driver 
  * interface and the mac. The ADJTIME module is the local clock frequency adjustment unit, 
  * implemented in software.   
  ******************************************************************************
  */
#ifndef __VIRHW_H__
#define __VIRHW_H__

#include "ttpdef.h"

/**
 * @defgroup TTPC_HW_Def
 */
/**@{*/

/** 
 * Basic function type definitions
 */
typedef enum FuncState{ENABLE  = 0, DISABLE = !ENABLE } FuncState;

/**
 * @defgroup TTPC_CLOCK_Def
 */
/**@{*/

///////////////////////////////////////////////////////////////////////////////
// Clock module definitions                                                  //
///////////////////////////////////////////////////////////////////////////////
/**
 * Operations of the timer
 */
typedef enum TimOpState 
{
	START = 0,
	STOP,
	PAUSE,
	CLEAR
}TimOpState;

/**
 * Macro definitions of basic properties of the timer
 */

/**
 * User timer options
 */
#define TIM_USER_MODE_PE 		(uint16_t)0x0001
#define TIM_USER_MODE_AP		(uint16_t)0x0002

/**
 * functions about the timer of TTPC
 */

/* platform-dependent operations interface *********************************/
void TIM_DepInit(void);

/* Base operations of TTPC timer *******************************************/
void     TIM_CMD(TimOpState op);
void     TIM_EnableTrigger(void);
void     TIM_DisableTrigger(void);

/* Macro/Micro-ticks getting operations ***********************************/
uint32_t TIM_GetFrequencyDiv(void);
uint16_t TIM_GetLocalFrequency(void);

uint32_t TIM_GetCaptureRX0(void);
uint32_t TIM_GetCaptureRX1(void);

uint32_t TIM_GetCurMacrotick(void);
uint32_t TIM_GetCurMicrotick(void);
uint32_t TIM_GetFrequencyDiv(void);


void TIM_ClearCaptureAll(void);

/* Trigger settings *******************************************************/

void TIM_SetStateCorrectionTerm(uint16_t value);
void TIM_SetFrequencyDiv(uint16_t div);
void TIM_SetCurMacrotick(uint32_t MacrotickValue);
void TIM_SetCurMicrotick(uint32_t MicrotickValue);

//in uint of macrotick
void TIM_SetTriggerAT(uint32_t at);
void TIM_SetTriggerPRP(uint32_t prp);
void TIM_SetTriggerSlotEnd(uint32_t end);

void TIM_SetTriggerUser(uint16_t UserMacrotickValue);

/* user settings **********************************************************/
void TIM_EnableUserIT(void);
void TIM_DisenableUserIT(void);
void TIM_SetAlarmOnce(uint16_t UserMacroValue);
void TIM_SetAlarmPeriod(uint16_t UserMacroValue);

/** 
 * sync operation for waiting for the elapsing of number of "Alarm" macroticks.
 * @Macrotick 
 * @fDisturb  disturb function, when fDisturb is true, it terminates the waiting.
 * @return 0: from waiting timeout, 1: from the disturb function 
 **/
uint32_t TIM_WaitAlarm(uint16_t  Macroticks, uint32_t (*fDisturb)(void));

/**@}*/// end of group TTPC_CLOCK_Def

/**
 * @defgroup TTPC_DMA
 */
/**@{*/

/////////////////////////////////////////////////////////////////////////////
// DMA module definitions                                                  //
/////////////////////////////////////////////////////////////////////////////

void DMA_DepInit(void);
#warning "more interfaces to be done"

/**@}*/// end of group TTPC_DMA

/**
 * @defgroup TTPC_ARQ
 */
/**@{*/		

/////////////////////////////////////////////////////////////////////////////
// TTPC basic interrupt routines definitions                               //
/////////////////////////////////////////////////////////////////////////////

//code here

/**@}*/// end of group TTPC_ARQ

/**
 * @defgroup TTPC_WDG
 */
/**@{*/

void WDG_DepInit(void);
void WDG_SetWindowValue(uint8_t WindowValue);
void WDG_SetCounter(uint8_t Counter);
void WDG_CMD(FuncState NewState);
void WDG_EnableIT(void);
void WDG_DisenableIT(void);     

/**@}*/// end of group TTPC_WDG

/**
 * @defgroup EXTERNAL_CRC24_Unit
 */
/**@{*/

#warning "Incompatible CRC bits with AS6003 "

/////////////////////////////////////////////////////////////////////////////
// CRC24 interfaces definitions                                            //
/////////////////////////////////////////////////////////////////////////////

void     CRC_DepInit(void);
void     CRC_ResetData(void);
uint32_t CRC_CalcBlock(uint32_t *pBuf, uint32_t BufLength);
uint32_t CRC_Calc(uint32_t data);
uint32_t CRC_GetCRC(void);

/**@}*/// end of group EXTERNAL_CRC32	
 
/**
 * @defgroup Physical_transfer
 */
/**@{*/

/////////////////////////////////////////////////////////////////////////////
// transmission driver layer interfaces definitions                        //
/////////////////////////////////////////////////////////////////////////////


typedef struct
{
    uint32_t    status;
    uint32_t    length;
    int8_t*     BufferAddr;
}DataStreamTypeDef;

/**
 * status macro definitions
 */
#define DRV_OK          (uint32_t)0x00000000
#define DRV_ES          (uint32_t)0x00000001  /**< error summary */
#define TXD_COL         (uint32_t)0x00000002
#define RXD_COL         (uint32_t)0x00000004
#define CRC_ERR         (uint32_t)0x00000008
#define PHY_ERR         (uint32_t)0x00000010
#define LTH_ERR         (uint32_t)0x00000020
#define DRV_INV         (uint32_t)0xa0000000 /**< invalid tx/rx time */
/**
 * /@todo The other errors to be expanded
 */

/**
 * functions definitions
 */

void DRV_DepInit(void);
void DRV_HardwareReset(void);

void DRV_StartTransmission(void);
void DRV_StopTransmission(void);
void DRV_StartReception(void);
void DRV_StopReception(void);

void DRV_PrepareToTransmitOfCH0(uint8_t* BufferAddr, uint16_t length);
void DRV_PrepareToTransmitOfCH1(uint8_t* BufferAddr, uint16_t length);
uint32_t DRV_CheckTransmitted(void);

/**
 * This function polls for a frame reception
 * @return 1:received farmes, 0:received nothing.
 */
uint32_t DRV_CheckReceived(void); 
DataStreamTypeDef DRV_GetReceived(void);

// void DRV_SetCommRate(uint16_t Rate);

/**@}*/// end of group Physical_transfer

void HW_GetPlatformID(uint8_t *buf, uint8_t length);

/**@}*/// end of group TTPC_HW_Def

#endif