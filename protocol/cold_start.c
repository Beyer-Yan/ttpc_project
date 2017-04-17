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

    uint32_t mode_num = CALC_MODE_NUM(MODE_CS_ID);
    RoundSlotProperty_t *pRS = MAC_LoadSlotProperties(mode_num,0,slot);
    
    //CS initing and settings
    CS_SetGTF(tsf);
    CS_SetRoundSlot(slot);
    CS_SetMode(MODE_CS_ID);
    CS_SetDMC(DMC_NO_REQ);
    CS_ClearMemberAll();
    CS_SetMemberBit(pRS->FlagPosition);

    //MAC settings
    MAC_SetClusterCycleLength(MEDL_GetRoundCycleLength(mode_num));
    MAC_SetTDMACycleLength(MEDL_GetTDMACycleLength(mode_num));
    MAC_SetSlot(slot);
    MAC_SetTDMARound(0);

    //SVC synchronization setting
    SVC_ClrClockSyncFIFO();

    //timer settings and slot timing properties setting
    TIM_SetLocalMicrotickValue(0);
    TIM_SetMacrotickValue(tsf+pNP->SendDelay);
    uint16_t real_prp  = (tsf+pNP->SendDelay+pRS->TransmissionDuration)&0xffff;
    uint16_t slot_end  = (tsf + pRS->SlotDuration - pRS->AtTime)&0xffff;
    TIM_SetTriggerPRP(real_prp);
    TIM_SetTriggerUser0(slot_end);

    //counters setting
    PV_IncCounter(COLD_START_COUNTER);
    PV_SetCounter(FAILED_SLOTS_COUNTER,0);
    PV_SetCounter(INTEGRATION_COUNTER,pSP->MinimumIntegrationCount);
    PV_DisableFreeShot();

    //start the real clock and transmition
    MAC_PrepareCSFrame();
    MAC_StartPhaseCirculation();
    MAC_StartTransmit();
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

