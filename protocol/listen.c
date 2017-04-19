/**
 ******************************************************************************
 * @file    	listen.c
 * @author  	Beyer
 * @email   	sinfare@foxmail.com
 * @version 	v1.0.0
 * @date    	20170331
 * @brief   	brief description about this file
 ******************************************************************************
 * @attention
 *   
 * @log
 *   
 *
 ******************************************************************************
 */
#include "medl.h"
#include "protocol.h"
#include "protocol_data.h"
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "virhw.h"

void FSM_toListen(void)
{
    ScheduleParameter_t *pSP = MAC_GetScheduleParameter();
    TIM_SetPollDefault(pSP->ColdStartTimeout);
}

void FSM_doListen(void)
{
    uint16_t runtime_ma;
    uint32_t runtime_mi;
    c_state_t cstate;

    ScheduleParameter_t* pSP;
    NodeProperty_t* pNP;
    RoundSlotProperty_t* pRS;

    int i = 0;

_poll:
    while (!TIM_PollAlarmDefault() && DRV_CheckReceived());

    //received frames
    if (DRV_CheckReceived()) {


    } else {
        //Listening timeout expired, check if the cold start conditions are fullfilled.
        FSM_sendEvent(FSM_EVENT_LISTEN_TIMEOUT_EXPIRED);
        if (pSP->ColdStartAllow == COLD_START_ALLOWED) {
            if (PV_GetCounter(COLD_START_COUNTER) <= pSP->MaximumColdStartEntry) {
                FSM_sendEvent(FSM_EVENT_COLD_START_ALLOWED);
                if (CNI_CheckHLFS()) {
                    FSM_sendEvent(FSM_EVENT_HOST_LIFE_UPDATED);
                    return;
                }
            }
        }

        TIM_SetPollDefault(pSP->ColdStartTimeout);
        goto _poll;
    }
}