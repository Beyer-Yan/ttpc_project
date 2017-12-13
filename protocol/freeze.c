/**
  ******************************************************************************
  * @file    	freeze.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	20170330
  * @brief   	some definitions for freeze state
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
 #include "ttpmac.h"
 #include "ttpdebug.h"
 #include "protocol_data.h"
 #include "protocol.h"
 #include "clock.h"
 #include "ttpservice.h"

void FSM_toFreeze(void)
{
	//stop then clear the timer
	CLOCK_Stop();

	//clear the CO flag
	TCN_ClrCO();
	
}

void FSM_doFreeze(void)
{
	SVC_Sleep();
}