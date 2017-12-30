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
#include "ttpmac.h"
#include "protocol.h"
#include "medl.h"
#include "clock.h"
#include "ttpservice.h"
#include "protocol_data.h"
#include "xfer.h"
#include "ttpdebug.h"
#include "msg.h"

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

/** 
 * The execution from the statup point to the actual transmission point shall be specified in
 * the implementation.
 */
void FSM_toColdStart(void)
{
    //inform the host that the cold start starts
    //code executing time shall be measured from the cold-start staring time
    //to the actual action time. Normally the PSP duration

    static uint16_t COLD_START_EXE_TIME_MACROTICK = 200;

    //tsp indicates the AT time of this node
    uint16_t tsf = CNI_GetTSF()&0xffff;

    CLOCK_Clear(); //stop then clear the clock
    //timer settings and slot timing properties setting
    CLOCK_SetCurMicrotick(0);
    CLOCK_SetCurMacrotick(tsf - COLD_START_EXE_TIME_MACROTICK);
    CLOCK_Start();

    NodeProperty_t *pNP = MAC_GetNodeProperties();
    ScheduleParameter_t *pSP = MAC_GetScheduleParameter();

    uint32_t slot = pNP->LogicalNameSlotPosition;

    uint32_t mode_num = CALC_MODE_NUM(MODE_CS_ID);
    RoundSlotProperty_t *pRS = MAC_LoadSlotProperties(mode_num,slot);
    
    //CS initing and settings
    CS_SetGTF(tsf);
    CS_SetRoundSlot(slot);
    CS_SetMode(MODE_CS_ID);
    CS_SetDMC(DMC_NO_REQ);
    CS_ClearMemberAll();
    CS_SetMemberBit(pRS->FlagPosition);
    
    MSG_PrepareCSFrame();
    DRV_PrepareToTransmit();
    MAC_StopReceive();

    //MAC settings
    MAC_SetClusterCycleLength(MEDL_GetRoundCycleLength(mode_num));
    MAC_SetTDMACycleLength(MEDL_GetTDMACycleLength(mode_num));
    MAC_SetSlot(slot);
    MAC_SetTDMARound(0);
    MAC_SetPhaseCycleStartPoint(tsf-pRS->AtTime,0);
    MAC_SetSlotTime(tsf+pNP->SendDelay,pRS->TransmissionDuration,pRS->PSPDuration,pRS->SlotDuration);
    
    //SVC synchronization setting
    SVC_ClrClockSyncFIFO();

    //counters setting
    PV_IncCounter(COLD_START_COUNTER);
    PV_SetCounter(FAILED_SLOTS_COUNTER,0);
    PV_SetCounter(INTEGRATION_COUNTER,pSP->MinimumIntegrationCount);
    PV_DisableFreeShot();

    //start the real clock and transmition
    phase_indicator = 1; //start from TP, for psp=0,tp=1,prp=2
    
    MAC_StartPhaseCirculation();
    MAC_SetSlotAcquisition(SENDING_FRAME);
}

void FSM_doColdStart(void)
{
    //do nothing
}

void FSM_toSubColdStart(void)
{
    CLOCK_Clear();
    CLOCK_Start();

    MAC_StopPhaseCirculation();
    MAC_StartReceive();

    //reset phase_indicator
    phase_indicator = 0;
}

static uint32_t _coldstart_disturb(void)
{
    //disturb function shall be specified more detailed
    return DRV_IsChannelActive();
}

// static uint32_t _coldstart_disturb(void)
// {
//     //disturb function shall be specified more detailed
//     if ((MAC_GetReceivedFlag(CH0) == MAC_EOK && MAC_GetReceivedFlag(CH1) == MAC_EOK))
//     {
//         return 1;
//     }
//     else
//     {
//         return 0;
//     }
// }

void FSM_doSubColdStart(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    //max cold start entries exceed
    if (PV_GetCounter(COLD_START_COUNTER) >= pSP->MaximumColdStartEntry) {
        goto _end;
    }

    //traffic detected during startup timeout
    if (CLOCK_WaitAlarm(pSP->StartupTimeout,_coldstart_disturb)) {
        goto _end;
    } 
    //check validity of host
    if (SVC_CheckHostLifeSign()) {
        //host life updated
        FSM_TransitIntoState(FSM_COLD_START);
        return;
    }
    //host life not updated
    _end:
    FSM_TransitIntoState(FSM_LISTEN);
}
