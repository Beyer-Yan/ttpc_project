/**
  ******************************************************************************
  * @file    	cold_start.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.03
  * @brief   	some definitions for cold start state
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
#include "protocol.h"

extern void psp_for_coldstart(void);
extern void tp(void);
extern void prp_for_coldstart(void);

extern volatile uint32_t phase_indicator;

/** definitions of three phases circulation for active state  */
struct SubSeqRoutine SSR_coldstart = 
{
	.pIndicator  = &phase_indicator,
	.func        = 
		{
			psp_for_coldstart,
			tp,
			prp_for_coldstart,
		},
};

void FSM_toColdStart(void)
{

}

void FSM_doColdStart(void)
{
	
}