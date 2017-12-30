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
 #include "host.h"

void FSM_toFreeze(void)
{
	//stop then clear the timer
	CLOCK_Stop();

	//clear the CO flag
	TCN_ClrCO();
    //clear the CA flag
    TCN_ClrCA();
    //clear the BIST flag
    TCN_ClrBIST();
	
}

void FSM_doFreeze(void)
{
    TTP_CR0 |= CR_CO; //just for test;
    while(!(TTP_CR0&CR_CO))
    {
        SVC_Sleep();
    }
    //now the controller is on
    if( (TTP_CR0&CR_BIST) && (TTP_CR0&CR_CO) )
    {
        FSM_TransitIntoState(FSM_TEST);
        return;
    }
    else if( (TTP_CR0&CR_CA) && (TTP_CR0&CR_CO) )
    {
        FSM_TransitIntoState(FSM_AWAIT);
        return;
    }
    else
    {
        FSM_TransitIntoState(FSM_INIT);
    }
}
