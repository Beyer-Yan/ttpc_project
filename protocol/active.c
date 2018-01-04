/**
  ******************************************************************************
  * @file    	active.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.03
  * @brief   	some definitions for active state
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
 
#include "protocol.h"

#include "host.h"

extern void psp_for_active(void);
extern void tp(void);
extern void prp_for_active(void);

extern volatile uint32_t phase_indicator;

/** definitions of three phases circulation for active state  */
struct SubSeqRoutine SSR_active = 
{
	.pIndicator  = &phase_indicator,
	.func        = 
		{
			psp_for_active,
			tp,
			prp_for_active,
		},
};

void FSM_toActive(void)
{
    //for testing
    //HOST_ModeChange(DMC_MODE_1);
}

void FSM_doActive(void)
{
	//nothing to be done
}
