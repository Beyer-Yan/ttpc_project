/**
 ******************************************************************************
 * @file    	core.c
 * @author  	Beyer
 * @email   	sinfare@foxmail.com
 * @version 	v1.0.0
 * @date    	2017.12
 * @brief   	the core interface implementation of ttpservice.h
 ******************************************************************************
 * @attention
 *   
 * @log
 *   
 *
 ******************************************************************************
 */
#include "ttpservice.h"
#include "cpu.h"
#include "protocol.h"
#include "ttpdef.h"
#include "clock.h"

void SVC_RaiseATSynchronousInterrupt(void)
{
    SVC_RaiseAsynchronousInterrupt();
}
void SVC_RaiseAsynchronousInterrupt(void)
{
    if( (TTP_CR0&CR_IE) !=0 )
    {
        if( ((~TTP_CR0>>16) & TTP_ISR) > 0 ) 
        {
            CPU_SoftwareInterrupt();
        }
    }
}

void SVC_Sleep(void)
{
    CPU_Sleep();
}

void SVC_Wait(uint32_t PhaseNumber)
{
    static uint32_t _phaseToTrigger[3] = {CLOCK_TRIGGER_END,CLOCK_TRIGGER_AT,CLOCK_TRIGGER_PRP};
    CLOCK_WaitTrigger(_phaseToTrigger[PhaseNumber]);
}

/** interrupt line shall be specified, diffrent from the awake line */
static void __ExternalEventHandler(void)
{
    if( (TTP_CR0&CR_CO)==0 )
    {
        //controller off now
        //FSM_TransitIntoStateUrgent(FSM_FREEZE);
    }
}

void SVC_CloseRootInt(void)
{
    CPU_InterruptDisable();
}
void SVC_OpenRootInt(void)
{
    CPU_RegisterInterruptHandler(__ExternalEventHandler);
    CPU_InterruptEnable();
}
