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
#include "ttpservice.h"
#include "virhw.h"

extern uint32_t phase_indicator;

/*
 * can be optimized, etc 4-bytes alignment
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

static inline void _process_slot_parameters()
{
    uint32_t mode_num = CALC_MODE_NUM(CS_GetCurMode());

    uint32_t ccl = MEDL_GetRoundCycleLength(mode_num); /* cluster cycle length */
    uint32_t ctl = MEDL_GetTDMACycleLength(mode_num); /* TDMA cycle length */

    MAC_SetClusterCycleLength(ccl);
    MAC_SetTDMACycleLength(ctl);

    uint32_t rs = CS_GetCurRoundSlot();
    uint32_t tdma = rs / ctl;
    uint32_t slot = rs % ctl;

    MAC_SetTDMARound(tdma);
    MAC_SetSlot(slot);

    //check then load the configuration of the current slot.
    MAC_LoadSlotProperties(CS_GetCurMode(), tdma, slot);
}
void FSM_toListen(void)
{
    TIM_CMD(CLEAR);
    MAC_StartReceive();
    MAC_SetSlot(0);
    MAC_SetTDMARound(0);
    TIM_CMD(START);
}

static uint32_t _disturb(void)
{
    //disturb function shall be specified more detailed
    if ((MAC_GetReceivedFlag(CH0) == MAC_EOK && MAC_GetReceivedFlag(CH1) == MAC_EOK))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void FSM_doListen(void)
{
    ScheduleParameter_t* pSP;
    NodeProperty_t* pNP;
    RoundSlotProperty_t* pRS;

    ttp_frame_desc_t* pDesc[2];
    c_state_t cstate[2];

    int listen_res = 0;

    pSP = MAC_GetScheduleParameter();
    pNP = MAC_GetNodeProperties();

    //break when timeout or received a valid frame.
    listen_res = TIM_WaitAlarm(pSP->ListenTimeout, _disturb);

    //received frames
    if (listen_res) {

        pDesc[0] = MAC_GetFrameDesc(CH0);
        pDesc[1] = MAC_GetFrameDesc(CH1);

        _byte_copy((uint8_t*)&cstate[0], pDesc[0]->pFrame->x.cstate, sizeof(c_state_t));
        _byte_copy((uint8_t*)&cstate[1], pDesc[1]->pFrame->x.cstate, sizeof(c_state_t));

        uint8_t header = pDesc[0]->pFrame->hdr[0];

        if (header != pDesc[1]->pFrame->hdr[0] || !(header & 1))
            goto _end;
        if (!_compare_cstate((uint8_t*)&cstate[0], (uint8_t*)&cstate[1]))
            goto _end;

        if ((cstate[0].ClusterPosition & CS_CP_MODE) == MODE_CS_ID) {
            if (PV_GetBigBangFlag() == BIG_BANG_ENABLE) {
                PV_DisableBigBang();
                goto _end;
            }
            if ((cstate[0].ClusterPosition & CS_CP_DMC) != DMC_NO_REQ)
                goto _end;
            if (pSP->ColdStartIntegrationAllow != COLD_START_INTEGRATION_ALLOWED)
                goto _end;
        }

        MAC_SetCState(&cstate[0]);

        //set current slot parameter according the sender's c-state
        _process_slot_parameters();

        pRS = MAC_GetRoundSlotProperties();
        if (header >> 1 != 0 && pRS->ModeChangePermission == MODE_CHANGE_DENY)
            goto _end;

        uint32_t int_cnt = (cstate[0].ClusterPosition & CS_CP_MODE) == MODE_CS_ID ? pSP->MinimumIntegrationCount : 1;
        PV_SetCounter(INTEGRATION_COUNTER, int_cnt);
        PV_SetCounter(AGREED_SLOTS_COUNTER, 2);
        PV_SetCounter(FAILED_SLOTS_COUNTER, 0);
        SVC_ClrClockSyncFIFO();
        CNI_ResetHLFS(); /**< more reasonable Op interface shall be negotiated */

        // cluster time correcting.
        uint16_t ratio = TIM_GetFrequencyDiv();

        //perform "correction" + "precision" of "sender", meaning cps_value
        uint32_t cps_value = pRS->DelayCorrectionTerms + pSP->Precision;
        uint32_t cps_mi = cps_value / (pSP->MacrotickParameter / ratio);
        uint32_t cur_mi = TIM_GetCurMicrotick();
        TIM_CMD(STOP);
        TIM_CMD(CLEAR);

        uint32_t exe_mi = cur_mi - (pDesc[0]->rcv_timestamp + pDesc[1]->rcv_timestamp) / 2;   
        uint32_t actual_ma = CS_GetCurGTF() + (exe_mi + cps_mi) / ratio;
        uint32_t actual_mi = (exe_mi + cps_mi) % ratio;

        TIM_SetCurMacrotick(actual_ma);
        TIM_SetCurMicrotick(actual_mi);
        MAC_SetPhaseCycleStartPoint(CS_GetCurGTF()-pRS->AtTime,0);

        //attention that the AT and the PRP time have expired at this time.
        MAC_SetTime(CS_GetCurGTF(),pRS->TransmissionDuration,pRS->PSPDuration,pRS->SlotDuration);

        phase_indicator = 0;        /**< point to the psp phase o the next slot */
        TIM_CMD(START);
        MAC_StartPhaseCirculation(); /**< start synchronization mode */

        CNI_SetSRBit(ISR_CV); //CSATE available
        FSM_sendEvent(FSM_EVENT_CSTATE_FRAME_RECEIVED);

    } else {
        //Listening timeout expired, check if the cold start conditions are fullfilled.
        FSM_sendEvent(FSM_EVENT_LISTEN_TIMEOUT_EXPIRED);

        if (pSP->ColdStartAllow == COLD_START_NOT_ALLOWED)
            goto _end;
        if (pSP->MaximumColdStartEntry == 0)
            goto _end;
        if (PV_GetCounter(COLD_START_COUNTER) > pSP->MaximumColdStartEntry)
            goto _end;

        FSM_sendEvent(FSM_EVENT_COLD_START_ALLOWED);

        if (!CNI_CheckHLFS())
            goto _end;

        FSM_sendEvent(FSM_EVENT_HOST_LIFE_UPDATED);
    }

_end:
    MAC_StopReceive();
}