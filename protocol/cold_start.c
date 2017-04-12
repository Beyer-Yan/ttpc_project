/**
  ******************************************************************************
  * @file    	cold_start.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.03
  * @brief   	some definitions for cold start and Sub Clod Start state
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */

#include "protocol.h"
#include "ttpc_mac.h"
#include "medl.h"
#include "virhw.h"
#include "ttpservice.h"
#include "protocol_data.h"

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
    uint32_t tsf = CNI_GetTSF();
    NodeProperty_t *pNP = MAC_GetNodeProperties();
    ScheduleParameter_t *pSP = MAC_GetScheduleParameter();

    uint32_t slot = pNP->LogicalNameSlotPosition;

    RoundSlotProperty_t *pRS = MAC_LoadSlotProperties(MODE_CS_ID,0,slot);
    
    //CS_SetGTF(tsf);

    //CS setting
    CS_SetRoundSlot(slot);
    CS_SetMode(MODE_CS_ID);
    CS_SetDMC(DMC_NO_REQ);
    CS_ClearMemberAll();
    CS_SetMemberBit(pRS->FlagPosition);

    //MAC setting
    MAC_SetSlot(slot);
    MAC_SetTDMARound(0);

    //SVC synchronization setting
    SVC_ClrClockSyncFIFO();

    //counters setting
    

    //slot timing properties setting
    uint16_t real_prp  = tsf + TP&0xffff;
    uint16_t slot_end  = 0 + SD&0xffff;

    TIM_SetTriggerAT(real_at);
    TIM_SetTriggerPRP(real_prp);
    TIM_SetTriggerUser0(slot_end);

}

void FSM_doColdStart(void)
{
    //do nothing
}

void FSM_toSubColdStart(void)
{

}

void FSM_doSubColdStart(void)
{

}

