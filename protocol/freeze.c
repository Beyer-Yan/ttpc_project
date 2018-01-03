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
 #include "led.h"

static void _error_log(void)
{
    if(TTP_SR&SR_MC)
        INFO("MEDL error");
    if(TTP_SR&SR_CC)
        INFO("Concurrent control error");
    if(TTP_SR&SR_NR)
        INFO("Frame not ready error");
    if(TTP_SR&SR_MV)
        INFO("Mode violation error");
    if(TTP_SR&SR_SO)
        INFO("Slot occupied error");
    if(TTP_SR&SR_BE)
        INFO("Bus Guardian error");
    if(TTP_SR&SR_CB)
        INFO("Clique no activity -- Communication blackout error");
    if(TTP_SR&SR_ME)
        INFO("Membership error");
    if(TTP_SR&SR_SE)
        INFO("Synchronization error");
    if(TTP_SR&SR_CE)
        INFO("Clique error -- Clique minority error");
    DBG_Flush();
    LED_On(LED_ERR);
}

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
    
    //protocol error happened, just for test;
    if(TTP_SR&0xfffffff0)
    {
        _error_log();
        TTP_CR0 &= ~CR_CO;
    }
    else
        TTP_CR0 |= CR_CO;
}

void FSM_doFreeze(void)
{
    while(!(TTP_CR0&CR_CO))
    {
        //SVC_Sleep();
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
