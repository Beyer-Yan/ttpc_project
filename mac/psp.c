/**
  ******************************************************************************
  * @file    	psp.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.12
  * @brief   	the pre-send-phase process
  ******************************************************************************
  * @attention
  *
  * The Pre-Send-Phase(PSP) is an exclusive phase for the TDMA slot. When the
  * synchronization architecture is set up, TDMA phase will start to work. When
  * the controller is in ACTIVE or PASIVE state, the phase circulation will live.
  * In addition, in COLD START state, one slot phase transaction must be proceed,
  * which is not instantiated here.
  * 
  * @desc
  *
  * 
  ******************************************************************************
  */
#include "protocol.h"
#include "protocol_data.h"
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "ttpservice.h"
#include "virhw.h"
#include "medl.h"

uint32_t _G_ModeChanged = 0;
static volatile uint32_t _G_SlotStartMacrotickTime = 0; 
static volatile uint32_t _G_SlotStartMicrotickTime = 0;

static volatile uint32_t _G_ClusterCycleStartTime  = 0;
static volatile uint32_t _G_TDMARoundStartTime     = 0;

/**
 * This function shall be called at the start time of a slot.
 */

static inline void _update_time()
{
    _G_SlotStartMacrotickTime = TIM_GetCurMacrotick();
    _G_SlotStartMicrotickTime = TIM_GetCurMicrotick();
}

static inline void _update_mode(void)
{
    uint32_t dmc = CS_GetCurDMC();
	uint16_t mode;

    if (dmc != DMC_NO_REQ) {
        switch (dmc) {
        case DMC_MODE_1:
            mode = MODE_1;
            break;
        case DMC_MODE_2:
            mode = MODE_2;
            break;
        case DMC_MODE_3:
            mode = MODE_3;
            break;
        default:
            break;
        }

		_G_ModeChanged = 1;

        CS_SetMode(mode);
        CS_SetDMC(DMC_NO_REQ);
        PV_DisableFreeShot();
    } else {
		_G_ModeChanged = 0;
	}
}

static inline void _load_slot_configuration(void)
{
    /**
	 * ensure that this function shall be called after updating the slot and the 
	 * the c-state mode.
	 */
    uint32_t mode = CS_GetCurMode();
    uint32_t tdma = MAC_GetTDMARound();
    uint32_t slot = MAC_GetNodeSlot();

    //check then load the configuration of the current slot.
    MAC_LoadSlotProperties(mode, tdma, slot);
	if(_G_ModeChanged) {
		uint32_t mode_num = CALC_MODE_NUM(mode);
		uint32_t ccl = MEDL_GetRoundCycleLength(mode_num); /* cluster cycle length */
		uint32_t ctl = MEDL_GetTDMACycleLength(mode_num);  /* TDMA cycle length */

		MAC_SetClusterCycleLength(ccl);
		MAC_SetTDMACycleLength(ctl);
	}
}

static inline void _slot_properties_update()
{
    uint32_t res;
    RoundSlotProperty_t *pRS;
    
    _update_time();
    res = MAC_UpdateSlot();
    if (res == FIRST_SLOT_OF_CURRENT_CLUSTER) {
        _update_mode();
        _G_ClusterCycleStartTime = _G_SlotStartMacrotickTime;
        _G_TDMARoundStartTime    = _G_SlotStartMacrotickTime;
    }

    if(res==FIRST_SLOT_OF_SUCCESSOR_TDMAROUND)
    {
        _G_TDMARoundStartTime = _G_SlotStartMacrotickTime;
    }
    _load_slot_configuration();

    pRS = MAC_GetRoundSlotProperties();
    CS_SetGTF(_G_ClusterCycleStartTime + _G_TDMARoundStartTime + pRS->AtTime);
}

static inline uint32_t _check_clique(void)
{
    uint32_t res = 0;
    uint32_t clique_res = 0;

    /**  clique detecting */
    clique_res = SVC_CliqueDetect();

    switch (clique_res) {
    case CLIQUE_MINORITY:
        FSM_sendEvent(FSM_EVENT_CLIQUE_MINORITY);
        CNI_SetSRBit(SR_CE);
        break;
    case CLIQUE_NO_ACTIVITY:
        FSM_sendEvent(FSM_EVENT_COMMUNICATION_BLACKOUT);
        CNI_SetSRBit(SR_CB);
        break;
    case CLIQUE_MAJORITY:
        FSM_sendEvent(FSM_EVENT_CLIQUE_MAJORITY);
        res = 1;
    default:
        break;
    }

    PV_ClrCounter(AGREED_SLOTS_COUNTER);
    PV_ClrCounter(FAILED_SLOTS_COUNTER);

    return res;
}

static inline void _prepare_for_receive(void)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    uint32_t mai = MAC_GetMacrotickParameter();

    if (pRS->SynchronizationFrame == SYN_FRAME) {
        SVC_SetEstimateArivalTimeInterval(pRS->DelayCorrectionTerms +  mai);
    }

    uint32_t actual_at = pRS->AtTime + _G_ClusterCycleStartTime + _G_TDMARoundStartTime;

    MAC_SetTime(actual_at, pRS->TransmissionDuration,pRS->PSPDuration, pRS->SlotDuration);
    MAC_SetSlotAcquisition(RECEIVING_FRAME);
}

static inline void _prepare_for_transmit(void)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
	NodeProperty_t*      pNP = MAC_GetNodeProperties();

    uint32_t actual_at = pRS->AtTime + _G_ClusterCycleStartTime + _G_TDMARoundStartTime + pNP->SendDelay;

    /**
	 * A sending node shall perceive itself as fully operational in its PSP, and shall
	 * therefore set its membership flag in the membership vector and set its agreed 
	 * slot counter to 1.
	 * @see AS6003, Page 34, Line -7.
	 */
    CS_SetMemberBit(pRS->FlagPosition);
    PV_SetCounter(AGREED_SLOTS_COUNTER, 1);

    MAC_SetTime(actual_at, pRS->TransmissionDuration,pRS->PSPDuration, pRS->SlotDuration);
    MAC_PushFrame();
    MAC_SetSlotAcquisition(SENDING_FRAME);

    /** init the ack state, only if the node plans to send in this slot */
    PV_SetAckState(ACK_INIT);
}

static inline uint32_t _is_data_frame()
{
    // the legality of the slot configuration shall be checked upper application
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    return ((pRS->FrameType == FRAME_TYPE_IMPLICIT) || (pRS->AppDataLength) ? 1 : 0);
}

void psp_for_passive(void)
{
    uint32_t res = 0;
    RoundSlotProperty_t* pRS;

    /** check MEDL configuration */
    #warning "periodic checking for MEDL has not been implemented"

    if (MAC_IsOwnNodeSlot()) {
        if (!_check_clique())
            return;
    }

    pRS = MAC_GetRoundSlotProperties();
    if (MAC_IsSendSlot()) {
        if (SVC_SlotAcquirement()) {
            if (CNI_IsModeChangeRequested()) {
                if (pRS->ModeChangePermission == MODE_CHANGE_DENY) {
                    CNI_SetSRBit(SR_MV);
                    FSM_sendEvent(FSM_EVENT_MODE_VIOLATION_ERR);
                    res = 0;
                    goto _end;
                } else {
                    CS_SetMemberBit(pRS->FlagPosition); /**< now, membership acquired */
                }
            }
            res = 1; /**< actually, the controller transmitted into activa state  */
            FSM_sendEvent(FSM_EVENT_NODE_SLOT_ACQUIRED);
        }
    }

_end:
    res ? _prepare_for_transmit() : _prepare_for_receive();
}

void psp_for_active(void)
{
    uint32_t res = 0;
    RoundSlotProperty_t* pRS;

    _slot_properties_update();

    if (MAC_IsOwnNodeSlot()) {
        if (!_check_clique())
            return;
    }

    if (MAC_IsSendSlot()) {

        if (SVC_CheckHostLifeSign()) {
            //host life updated during active state
            FSM_sendEvent(FSM_EVENT_HOST_LIFE_UPDATED);
            //freeshot state
            PV_DisableFreeShot();
            res = 1;
        } else {
            FSM_sendEvent(FSM_EVENT_HOST_LIFE_NOT_UPDATED);
        }
    }

    pRS = MAC_GetRoundSlotProperties();
    //check the mode request field
    if (res != 0) {
        if (CNI_IsModeChangeRequested()) {
            if (pRS->ModeChangePermission == MODE_CHANGE_DENY) {
                CNI_SetSRBit(SR_MV);
                FSM_sendEvent(FSM_EVENT_MODE_VIOLATION_ERR);
                res = 0;
            }
        }
    }
    //check whether the data is ready, if there is a data frame is to be send
    if (res != 0) {
        if (_is_data_frame()) {
            if (!MSG_CheckMsgRF(pRS->CNIAddressOffset)) {
                CNI_SetSRBit(SR_NR);
                res = 0;
            }
        }
    }

    res ? _prepare_for_transmit() : _prepare_for_receive();
}

void psp_for_coldstart(void)
{
}

uint32_t  MAC_GetSlotStartMacroticks(void)
{
    return _G_SlotStartMacrotickTime;
}
uint32_t  MAC_GetSlotStartMicroticks(void)
{
    return _G_SlotStartMicrotickTime;
}

void MAC_SetPhaseCycleStartPoint(uint32_t CycleStartTime, uint32_t TDMAStartOffset)
{
    _G_ClusterCycleStartTime = CycleStartTime;
    _G_TDMARoundStartTime    = 0;
}