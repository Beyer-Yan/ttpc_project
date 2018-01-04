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
#include "ttpmac.h"
#include "ttpdebug.h"
#include "ttpservice.h"
#include "clock.h"
#include "msg.h"
#include "xfer.h"

#include "stm32f4xx.h"


#define EXE_MI_FOR_LISTEN  (3314 + 100)

extern uint32_t phase_indicator;

/*
 * can be optimized, ex. 4-bytes alignment
 */
static inline void _byte_copy(uint8_t* dst, uint8_t* src, int size)
{
    while (size--) {
        *dst++ = *src++;
    }
}

static inline int _compare_cstate(uint8_t* cstate0, uint8_t* cstate1)
{
    int res = 1;
    int i = 0;

    while (i++ < sizeof(c_state_t)) {
        if (*cstate0++ != *cstate1++) {
            res = 0;
            break;
        }
    }
    return res;
}

static inline int _process_slot_parameters()
{
    uint32_t mode = CS_GetCurMode();
    uint32_t mode_num = CALC_MODE_NUM(mode);
    uint32_t rs = CS_GetCurRoundSlot();

    if(MAC_LoadSlotProperties(mode, rs)==NULL){
        return 0;
    }

    uint32_t ccl = MEDL_GetRoundCycleLength(mode_num); /* cluster cycle length */
    uint32_t ctl = MEDL_GetTDMACycleLength(mode_num);  /* TDMA cycle length */

    MAC_SetClusterCycleLength(ccl);
    MAC_SetTDMACycleLength(ctl);

    uint32_t tdma = rs / ctl;
    uint32_t slot = rs % ctl;

    MAC_SetTDMARound(tdma);
    MAC_SetSlot(slot);

    //check then load the configuration of the current slot.
    return 1;
}

void FSM_toListen(void)
{
    //TIM_CMD(CLEAR);
    MAC_SetSlot(0);
    MAC_SetTDMARound(0);
    CLOCK_Start();
    MAC_StartReceive();
}

static uint32_t _listen_disturb(void)
{
    //disturb function shall be specified more detailed
    return DRV_IsChannelActive();
}

void FSM_doListen(void)
{
    ScheduleParameter_t* pSP;
    NodeProperty_t* pNP;
    RoundSlotProperty_t* pRS;

    TTP_FrameDesc* pDesc;
    c_state_t cstate;
    uint8_t header;

    pSP = MAC_GetScheduleParameter();
    pNP = MAC_GetNodeProperties();

    //break when timeout or received a valid frame.
    uint32_t freq_div = CLOCK_GetFrequencyDiv(); 
    uint32_t frequency = CLOCK_GetLocalFrequency();
    uint32_t ATW = pSP->ArrivalTimingWindow*frequency/1000 + 1;
    
    uint32_t t1,t2,t3,t4;
    
    DBG_Flush();
    if (CLOCK_WaitAlarm(pSP->ListenTimeout, _listen_disturb)) {
        uint32_t t1 = TIM14->CNT;
        CLOCK_WaitMicroticks(ATW,NULL);
        
        uint32_t ch0_res = MSG_CheckReceived(CH0);
        uint32_t ch1_res = MSG_CheckReceived(CH1);

        if(ch0_res==0 && ch1_res==0 )
        {
            //perform the ATW algorithm 
            PV_DisableBigBang();
            goto _end;
        }
  
        pDesc = MSG_GetFrameDesc();
        if(pDesc==NULL)
            goto _end;
        
        //frame received on both CH0 and CH1
        if(ch0_res==1 && ch1_res==1){
            uint8_t header0 = pDesc->pCH0->pFrame->hdr[0];
            uint8_t header1 = pDesc->pCH1->pFrame->hdr[0];

            if(header0!=header1)
                goto _end;
            if (!(header0 & 1))
                goto _end;
            if (!_compare_cstate(pDesc->pCH0->pFrame->x.cstate, pDesc->pCH1->pFrame->x.cstate) )
                goto _end;
        }

        if(ch0_res){
            header = pDesc->pCH0->pFrame->hdr[0];
            _byte_copy((uint8_t*)&cstate, pDesc->pCH0->pFrame->x.cstate, sizeof(c_state_t));
        } 
        else{
            header = pDesc->pCH1->pFrame->hdr[0];
            _byte_copy((uint8_t*)&cstate, pDesc->pCH1->pFrame->x.cstate, sizeof(c_state_t));
        }

        //in case of a cold start frame
        if ((cstate.ClusterPosition & CS_CP_MODE) == MODE_CS_ID) {           
            if (PV_GetBigBangFlag() == BIG_BANG_ENABLE) {
                PV_DisableBigBang();
                goto _end;
            }
            if ((cstate.ClusterPosition & CS_CP_DMC) != DMC_NO_REQ)
                goto _end;
            if (pSP->ColdStartIntegrationAllow != COLD_START_INTEGRATION_ALLOWED)
                goto _end;
        }
        
        // now the frame received is taken into consideration
        CS_SetCState(&cstate);
        uint32_t t2 = TIM14->CNT;
        //set current slot parameter according the sender's c-state
        if(!_process_slot_parameters())
            goto _end;
        uint32_t t3 = TIM14->CNT;
        
        pRS = MAC_GetRoundSlotProperties();

        // mode error, desert the received frame
        if (header >> 1 != 0 && pRS->ModeChangePermission == MODE_CHANGE_DENY)
            goto _end;

        uint32_t int_cnt = (cstate.ClusterPosition & CS_CP_MODE) == MODE_CS_ID ? pSP->MinimumIntegrationCount : 1;
        PV_SetCounter(INTEGRATION_COUNTER, int_cnt);
        PV_SetCounter(AGREED_SLOTS_COUNTER, 2);
        PV_SetCounter(FAILED_SLOTS_COUNTER, 0);
        SVC_ClrClockSyncFIFO();
        CNI_ResetHLFS(); /**< more reasonable Op interface shall be negotiated */
        
        // cluster time correcting.
        //perform "correction" + "precision" of "sender", meaning cps_value
        uint32_t cps_value = pRS->DelayCorrectionTerms + pSP->Precision;
        uint32_t cps_mi = cps_value*frequency/1000+1;
        uint32_t y = TIM14->CNT;
        //stop then clear the clock
        CLOCK_Clear();
        
        uint32_t AT_time = CS_GetCurGTF()&0xffff;
 
        uint32_t actual_ma = AT_time + (EXE_MI_FOR_LISTEN + cps_mi) / freq_div;
        uint32_t actual_mi = (EXE_MI_FOR_LISTEN + cps_mi) % freq_div;

        CLOCK_SetCurMacrotick(actual_ma);
        CLOCK_SetCurMicrotick(actual_mi);
        
        //#error "some bugs to be fixed"
        MAC_SetPhaseCycleStartPoint(CS_GetCurGTF()-pRS->AtTime,0);
        
        //attention that the AT and the PRP time have expired at this time.
        //MAC_SetSlotTime(AT_time,pRS->TransmissionDuration,pRS->PSPDuration,pRS->SlotDuration, 0);
        uint16_t slot_end = AT_time + (pRS->SlotDuration & 0xffff) - pRS->PSPDuration;
        CLOCK_SetTriggerSlotEnd(slot_end);

        phase_indicator = 0;        /**< point to the psp phase o the next slot */
        uint32_t t4 = TIM14->CNT;
        CLOCK_Start();
        
        INFO("%u,%u,%u",actual_ma,actual_mi,t4-t1);
        
        MAC_StartPhaseCirculation(); /**< start synchronization mode */

        CNI_SetISRBit(ISR_CV); //CSATE available
        FSM_TransitIntoState(FSM_PASSIVE);

    } else {
        //Listening timeout expired, check if the cold start conditions are fullfilled.
        
        if (pSP->ColdStartAllow == COLD_START_NOT_ALLOWED)
            goto _end;
        if (pSP->MaximumColdStartEntry == 0)
            goto _end;
        if (PV_GetCounter(COLD_START_COUNTER) >= pSP->MaximumColdStartEntry){
            goto _end;
        }
        if (!SVC_CheckHostLifeSign())
            goto _end;

        FSM_TransitIntoState(FSM_COLD_START);
    }

_end:
    MAC_StopReceive();
}
