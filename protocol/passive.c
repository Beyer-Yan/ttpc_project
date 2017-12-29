/**
  ******************************************************************************
  * @file    	passive.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.03
  * @brief   	some definitions for passive state
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
#include "protocol.h"

extern void psp_for_passive(void);
extern void tp(void);
extern void prp_for_passive(void);

extern volatile uint32_t phase_indicator;

/** definitions of three phases circulation for passive state */
struct SubSeqRoutine SSR_passive = 
{
	.pIndicator  = &phase_indicator,
	.func        = 
		{
			psp_for_passive,
			tp,
			prp_for_passive,
		},
};


void FSM_toPassive(void)
{
    //nothing to be done
}

void FSM_doPassive(void)
{
	//nothing to be done
}
