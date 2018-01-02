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
  * triggering point of the PSP,AT  and PRP actions..  And  the XFER  module is  the bridge
   of the  physical  driver interface and the mac. 
  ******************************************************************************
  */
#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stdint.h>
/**
 * @defgroup TTPC_HW_Def
 */
/**@{*/

/**
 * @defgroup TTPC_CLOCK_Def
 */
/**@{*/

///////////////////////////////////////////////////////////////////////////////
// Clock module definitions                                                  //
///////////////////////////////////////////////////////////////////////////////

/**
 * Macro definitions of basic properties of the timer
 */

/**
 * User timer options
 */
#define CLOCK_USER_MODE_PE 		(uint16_t)0x0001
#define CLOCK_USER_MODE_AP		(uint16_t)0x0002

#define CLOCK_TRIGGER_AT        0
#define CLOCK_TRIGGER_PRP       1
#define CLOCK_TRIGGER_END       2

/**
 * functions about the timer of TTPC
 */

/* platform-dependent operations interface *********************************/
void CLOCK_DepInit(void);

/* Base operations of TTPC timer *******************************************/
void     CLOCK_Start(void);
void     CLOCK_Stop(void);
void     CLOCK_Reset(void);
void     CLOCK_Clear(void);

void     CLOCK_EnableTrigger(void);
void     CLOCK_DisableTrigger(void);

/* Macro/Micro-ticks getting operations ***********************************/
uint32_t CLOCK_GetFrequencyDiv(void);
uint32_t CLOCK_GetLocalFrequency(void);

uint32_t CLOCK_GetCaptureRX0(void);
uint32_t CLOCK_GetCaptureRX1(void);

uint32_t CLOCK_GetCurMacrotick(void);
uint32_t CLOCK_GetCurMicrotick(void);


void CLOCK_ClearCaptureAll(void);

/* Trigger settings *******************************************************/

void CLOCK_SetStateCorrectionTerm(int16_t value);
void CLOCK_SetFrequencyDiv(uint16_t div);
void CLOCK_SetCurMacrotick(uint32_t MacrotickValue);
void CLOCK_SetCurMicrotick(uint32_t MicrotickValue);

//in uint of macrotick
void CLOCK_SetTriggerAT(uint32_t at);
void CLOCK_SetTriggerPRP(uint32_t prp);
void CLOCK_SetTriggerSlotEnd(uint32_t end);

static inline void CLOCK_SetTrigger(uint32_t at,uint32_t prp,uint32_t end)
{
    CLOCK_SetTriggerAT(at);
    CLOCK_SetTriggerPRP(prp);
    CLOCK_SetTriggerSlotEnd(end);
}

void CLOCK_WaitTrigger(uint32_t ClockTrigger);

/* user settings **********************************************************/
void CLOCK_EnableUserIT(void);
void CLOCK_DisenableUserIT(void);
void CLOCK_SetAlarmOnce(uint16_t UserMacroValue);
void CLOCK_SetAlarmPeriod(uint16_t UserMacroValue);
void CLOCK_SetTriggerUser(uint16_t UserMacrotickValue);
/** 
 * sync operation for waiting for the elapsing of number of "Alarm" macroticks.
 * @Macrotick 
 * @fDisturb  disturb function, when fDisturb is true, it terminates the waiting.
 * @return 0: from waiting timeout, 1: from the disturb function 
 **/
uint32_t CLOCK_WaitAlarm(uint32_t  Macroticks, uint32_t (*fDisturb)(void));

uint32_t CLOCK_WaitMicroticks(uint32_t MicrotickValue, uint32_t (fDisturb)(void));

uint32_t test_x_function(uint32_t x);

/**@}*/// end of group TTPC_CLOCK_Def

#endif
