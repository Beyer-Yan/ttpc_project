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
typedef enum FuncState{ENABLE  = 0, DISABLE = !ENABLE };

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
};

/**
 * Macro definitions of basic properties of the timer
 */

/**
 * The channels of the timers supporting the fixed type granularities of
 * the local timer, such as 1-ns granularity, 10-ns granularity,100-ns
 * granularity and the 1-us granularity. Each channel specifies one of
 * them, which cannot be modified when the specified granularity is set
 * firstly. The setting of fixing the granularity of the local timer is
 * just a way to an agreement of the microtick, by supporting a consistent
 * absolute granularity of the local clock forcedly. 
 * Actually, achieving the agreement of the granularity of the local timers
 * is difficult, especially facing the different implementations of TTPC
 * controller, although within the same standard -- AS6003. 
 * The one of other ways to solve the granularity mismatch of the local timer
 * is to compensate the residual errors of granularity alignment of different 
 * local timers.
 */

/**
 * the approach of the CHANNEL to solve the granularity mismatch
 */
#define TIM_CH_1				(uint16_t)0x0001   
#define TIM_CH_2				(uint16_t)0x0002
#define TIM_CH_3				(uint16_t)0x0004

/**
 * The absolutely granularity of the local timer macro definition
 */
#define MIC_1_NS 				(uint16_t)0x0001
#define MIC_10_NS 				(uint16_t)0x0002
#define MIC_25_NS				(uint16_t)0x0004
#define MIC_50_NS				(uint16_t)0x0008
#define MIC_100_NS				(uint16_t)0x000C
#define MIC_1_US 				(uint16_t)0x0010
#define MIC_10_US				(uint16_t)0x0011
#define MIC_100_US 				(uint16_t)0x0012

/**
 * Compensate mode definitions
 */
#define CHANNEL_MODE			(uint16_t)0x0001
#define COMPENSATE_MODE			(uint16_t)0x0002


/**
 * User timer options
 */
#define TIM_USER_MODE_PE 		(uint16_t)0x0001
#define TIM_USER_MODE_AP		(uint16_t)0x0002

/**
 * Capture channels
 */
#define TIM_CAPTURE_RX0         (uint16_t)0x0001
#define TIM_CAPTURE_RX1         (uint16_t)0x0002
#define TIM_CAPTURE_PSP         (uint16_t)0x0004
#define TIM_CAPTURE_USR         (uint16_t)0x0008

/**
 * functions about the timer of TTPC
 */

/* platform-dependent operations interface *********************************/
void TIM_DepInit(void);

/* Base operations of TTPC timer *******************************************/
void     TIM_CMD(TimOpState op);
uint16_t TIM_GetRatio(void);
void     TIM_RatioAdj(int32_t *corVal, int Steps);

/* Microtick settings ******************************************************/ 
void TIM_SetCompensateMode(uint16_t CompensateMode);
void TIM_ChannelCMD(uint16_t TIM_Channel, FuncState NewState);
void TIM_SetChannelMicrotickGranule(uint16_t TIM_Channel, uint16_t MicGranule);
void TIM_SetLocalMicrotickGranule(uint16_t GranuleValue);
void TIM_SetLocalMicrotickValue(uint32_t MicrotickValue);

/* Macrotick setting ******************************************************/
void TIM_SetMacrotickGranule(uint16_t MacGranule);
void TIM_SetMacrotickValue(uint16_t MacrotickValue);

/* Macro/Micro-ticks getting operations ***********************************/
uint32_t TIM_GetCurMicroticks(void);
uint16_t TIM_GetCurMacroticks(void);

/* Capture values *********************************************************/
uint32_t TIM_GetCapture(uint16_t CapChannel);

/** in unit of microtick */
uint32_t TIM_GetCaptureRX0(void);
uint32_t TIM_GetCaptureRX1(void);
uint32_t TIM_GetCapturePSP(void);
uint32_t TIM_GetCaptureUser(void);

/**
 * Function gets the macrotick value when psp starts.
 * Different from function "TIM_GetCapturePSP", it returns the corresponding macrotick
 * value of the value returned by function "TIM_GetCapturePSP" at the same real time. 
 * @return  the macrotick if PSP time.
 */
uint16_t TIM_GetCaptureMacrotickPSP(void);

void TIM_ClearCaptureAll(void);
void TIM_ClearCapture(uint16_t CapChannel);

/* Trigger settings *******************************************************/

/** in unit of macrotick */
void TIM_SetTriggerAT(uint16_t AT);
void TIM_SetTriggerPRP(uint16_t PRP);
void TIM_SetTriggerUser0(uint16_t User);
void TIM_SetTriggerUser1(uint16_t User);

void TIM_SetForPoll(uint16_t Value, uint16_t AlarmNumber);
uint32_t TIM_PollAlarm(uint16_t AlarmNumber);

#define  TIM_SetPollDefault(v) 		TIM_SetForPoll(v,0)
#define  TIM_PollAlarmDefault() 	TIM_PollAlarm(0)

/** sync operation for waiting for the elapsing of number of "Alarm" macroticks. */
void TIM_WaitAlarm(uint16_t  Alarm);

/* user settings **********************************************************/
void TIM_EnableUserIT(void);
void TIM_DisenableUserIT(void);
void TIM_SetAlarmOnce(uint16_t UserCounter);
void TIM_SetAlarmPeriod(uint16_t UserCounter);

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