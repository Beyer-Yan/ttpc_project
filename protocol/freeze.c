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
 #include "ttpc_mac.h"
 #include "ttpdebug.h"
 #include "protocol_data.h"
 #include "protocol.h"
 #include "virhw.h"

void FSM_toFreeze(void)
{
	//close the timer
	MAC_StopPhaseCirculation();

	//clear the CO flag
	TCN_ClrCO();
	
}

void FSM_doFreeze(void)
{
	SVC_Sleep();
}