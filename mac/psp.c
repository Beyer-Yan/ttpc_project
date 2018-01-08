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
#include "ttpmac.h"
#include "ttpdebug.h"
#include "ttpservice.h"
#include "clock.h"
#include "medl.h"
#include "msg.h"
#include "xfer.h"

#include "host.h"

uint32_t _G_ModeChanged                              __SECTION("PV_SECTION") = 0;
static volatile uint16_t _G_SlotStartMacrotickTime   __SECTION("PV_SECTION") = 0; 
static volatile uint32_t _G_SlotStartMicrotickTime   __SECTION("PV_SECTION") = 0;

static volatile uint16_t _G_ClusterCycleStartTime    __SECTION("PV_SECTION") = 0;
static volatile uint16_t _G_TDMARoundStartTimeOffset __SECTION("PV_SECTION") = 0;

/**
 * This function shall be called at the start time of a slot.
 */

static inline void _update_time(void)
{
    _G_SlotStartMacrotickTime = CLOCK_GetCurMacrotick();
    _G_SlotStartMicrotickTime = CLOCK_GetCurMicrotick();
}

static inline void _update_mode(void)
{
    uint32_t dmc = CS_GetCurDMC();
	uint16_t mode = CS_GetCurMode();

    TTP_ASSERT(IS_TTP_DMC(dmc));

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
/** 
 * update and check the slot indicator.
 * @return 1, indicate a cluster-start point
 *         0, not cluster start
 */
static inline int _update_slot_indicator(void)
{
    uint32_t ret_val = 0;
    uint32_t res = MAC_UpdateSlot();
    if (res == FIRST_SLOT_OF_CURRENT_CLUSTER) {
        _G_ClusterCycleStartTime    = _G_SlotStartMacrotickTime;
        _G_TDMARoundStartTimeOffset = 0;
        ret_val = 1;
    }

    if(res==FIRST_SLOT_OF_SUCCESSOR_TDMAROUND)
    {
        _G_TDMARoundStartTimeOffset = _G_SlotStartMacrotickTime - _G_ClusterCycleStartTime;
    }
    
    return ret_val;
}

static inline RoundSlotProperty_t* _update_slot_configuration(void)
{
    /**
	 * ensure that this function shall be called after updating the slot and the 
	 * the c-state mode.
	 */
    uint32_t mode = CS_GetCurMode();
    //uint32_t tdma = MAC_GetTDMARound();
    uint32_t round_slot = MAC_GetRoundSlot();
    RoundSlotProperty_t *pRS;

    //check then load the configuration of the current slot.
    
	if(_G_ModeChanged) {
		uint32_t mode_num = CALC_MODE_NUM(mode);
		uint32_t ccl = MEDL_GetRoundCycleLength(mode_num); /* cluster cycle length */
		uint32_t ctl = MEDL_GetTDMACycleLength(mode_num);  /* TDMA cycle length */

		MAC_SetClusterCycleLength(ccl);
		MAC_SetTDMACycleLength(ctl);
	}
    pRS = MAC_LoadSlotProperties(mode, round_slot);
    TTP_ASSERT(pRS!=NULL);
    return pRS;
}

static inline RoundSlotProperty_t* _slot_property_update(void)
{
    RoundSlotProperty_t *pRS;
    _update_time();
    if(_update_slot_indicator())
        _update_mode();

    pRS = _update_slot_configuration();
    CS_SetGTF(_G_ClusterCycleStartTime + _G_TDMARoundStartTimeOffset + pRS->AtTime);
    return pRS;
}

/*
static inline uint32_t _check_clique(void)
{
    uint32_t res = 0;
    uint32_t clique_res = 0;

    
    clique_res = SVC_CliqueDetect();

    switch (clique_res) {
    case CLIQUE_MINORITY:
        CNI_SetSRBit(SR_CE);
        break;
    case CLIQUE_NO_ACTIVITY:

        CNI_SetSRBit(SR_CB);
        break;
    case CLIQUE_MAJORITY:
        res = 1;
    default:
        break;
    }

    return res;
}
*/

static inline void _prepare_for_receive(void)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    uint32_t mai = MAC_GetMacrotickParameter();

    if (pRS->SlotFlags & SlotFlags_SynchronizationFrame) {
        SVC_SetEstimateArivalTimeInterval(pRS->DelayCorrectionTerms0+mai,pRS->DelayCorrectionTerms1+mai);
    }

    uint32_t actual_at = pRS->AtTime + _G_ClusterCycleStartTime + _G_TDMARoundStartTimeOffset;

    MAC_SetSlotTime(actual_at, pRS->TransmissionDuration,pRS->PSPDuration, pRS->SlotDuration,0);
    MAC_SetSlotAcquisition(RECVING_FRAME);
}

static inline void _prepare_for_transmit(void)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
	NodeProperty_t*      pNP = MAC_GetNodeProperties();

    uint32_t actual_at = pRS->AtTime + _G_ClusterCycleStartTime + _G_TDMARoundStartTimeOffset;

    /**
	 * A sending node shall perceive itself as fully operational in its PSP, and shall
	 * therefore set its membership flag in the membership vector and set its agreed 
	 * slot counter to 1.
	 * @see AS6003, Page 34, Line -7.
	 */
    CS_SetMemberBit(pRS->FlagPosition);
    PV_SetCounter(AGREED_SLOTS_COUNTER, 1);

    MAC_SetSlotTime(actual_at, pRS->TransmissionDuration,pRS->PSPDuration, pRS->SlotDuration, pNP->SendDelay);   
    MAC_SetSlotAcquisition(SENDING_FRAME);

    uint32_t mcr = CNI_GetCurMCR();
    if(!IS_TTP_MCR(mcr)){
        CNI_SetSRBit(SR_MV);
        CNI_SetISRBit(ISR_HE);
        CNI_ClrMCR();
    }
    
    MSG_PushFrame();
    
    if(CNI_IsModeChangeRequested()){
        uint32_t dmc = MCR_TO_DMC(mcr);
        CS_SetDMC(dmc);
        CNI_ClrMCR();        
    }
    DRV_PrepareToTransmit();
    /** init the ack state, only if the node plans to send in this slot */
    PV_SetAckState(ACK_INIT);
}

static inline uint32_t _is_data_frame()
{
    // the legality of the slot configuration shall be checked upper application
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    return ( !(pRS->SlotFlags & SlotFlags_FrameTypeExplicit) || (pRS->AppDataLength) ? 1 : 0);
}

void psp_for_passive(void)
{
    RoundSlotProperty_t* pRS = _slot_property_update();
    
    //INFO("SLOT----------------------------------------%d",MAC_GetRoundSlot());
    
    //INFO("SSS PASSIVE   -- TIME:%u",_G_SlotStartMacrotickTime);
    //INFO("mode:%d",CALC_MODE_NUM(CS_GetCurMode()));
    //INFO("agreed:%d",PV_GetCounter(AGREED_SLOTS_COUNTER));
    //INFO("failed:%d",PV_GetCounter(FAILED_SLOTS_COUNTER));
    /** check MEDL configuration */
    #warning "periodic checking for MEDL has not been implemented"

    if (MAC_IsOwnNodeSlot()) {
        uint32_t clique_res = SVC_CliqueDetect();
        if(clique_res == CLIQUE_MINORITY){
            CNI_SetSRBit(SR_CE);
            //INFO("CLIQUE_MINORITY");
            FSM_TransitIntoState(FSM_FREEZE);
            return;
        }else if(clique_res == CLIQUE_NO_ACTIVITY){
            CNI_SetSRBit(SR_CB);
            //INFO("CLIQUE_NO_ACTIVITY");
            FSM_TransitIntoState(FSM_FREEZE);
            return;
        }else{
            //majority clique
            PV_ClrCounter(AGREED_SLOTS_COUNTER);
            PV_ClrCounter(FAILED_SLOTS_COUNTER);
        }
    }

    if (!MAC_IsSendSlot())
        goto _end;
    
    if(!SVC_SlotAcquirement())
        goto _end;

    if(CNI_IsModeChangeRequested()){
        if(!(pRS->SlotFlags & SlotFlags_ModeChangePermission)){
            CNI_SetSRBit(SR_MV);
            CNI_ClrMCR();
            goto _end;
        }
    }
     /**< now, membership acquired, the controller shall transite into ACTIVE state*/
    FSM_TransitIntoState(FSM_ACTIVE);

    if (_is_data_frame()) {
        
        #warning "just for system testing"
        HOST_PrepareData();
        
        if (!MSG_CheckMsgRF(pRS->CNIAddressOffset)) {
            CNI_SetSRBit(SR_NR);
            goto _end;             
        }
    }
    _prepare_for_transmit();
    return;

    _end:
     _prepare_for_receive();
}

void psp_for_active(void)
{
    RoundSlotProperty_t* pRS = _slot_property_update();

    //INFO("SLOT---------------------------------------- %d",MAC_GetRoundSlot());
    //INFO("mode:%d",CALC_MODE_NUM(CS_GetCurMode()));
    //INFO("agreed:%d",PV_GetCounter(AGREED_SLOTS_COUNTER));
    //INFO("failed:%d",PV_GetCounter(FAILED_SLOTS_COUNTER));
    //INFO("cluster start:%u",_G_ClusterCycleStartTime);
    //INFO("tdma offset  :%u",_G_TDMARoundStartTimeOffset);
    //INFO("slot start   :%u",_G_SlotStartMacrotickTime);
    //INFO("at time      :%u",pRS->AtTime);
    
    //INFO("SSS ACTIVE    -- TIME:%u",_G_SlotStartMacrotickTime);

    if (MAC_IsOwnNodeSlot()) {
        //clique detection
        uint32_t clique_res = SVC_CliqueDetect();
        if(clique_res == CLIQUE_MINORITY){
            CNI_SetSRBit(SR_CE);
            //INFO("CLIQUE_MINORITY");
            FSM_TransitIntoState(FSM_FREEZE);
            return;
        }else if(clique_res == CLIQUE_NO_ACTIVITY){
            CNI_SetSRBit(SR_CB);
            //INFO("CLIQUE_NO_ACTIVITY");
            FSM_TransitIntoState(FSM_FREEZE);
            return;
        }else{
            //majority clique
            PV_ClrCounter(AGREED_SLOTS_COUNTER);
            PV_ClrCounter(FAILED_SLOTS_COUNTER);
        }
    }

    if (MAC_IsSendSlot()) {
        if (SVC_CheckHostLifeSign()) {
            //host life updated during active state
            //freeshot state
            PV_DisableFreeShot();
        } else {
            FSM_TransitIntoState(FSM_PASSIVE);
            CS_ClearMemberBit(pRS->FlagPosition);
            goto _end;
        }

        //check the mode request field
        if (CNI_IsModeChangeRequested()) {
            if (!(pRS->SlotFlags & SlotFlags_ModeChangePermission)) {
                CNI_SetSRBit(SR_MV);
                CNI_ClrMCR();
                CS_ClearMemberBit(pRS->FlagPosition);
                FSM_TransitIntoState(FSM_PASSIVE);
                goto _end;
            } 
        }

        //check whether the data is ready, if there is a data frame is to be send
        if (_is_data_frame()) {
            
            #warning "just for system testing"
            HOST_PrepareData();
            
            if (!MSG_CheckMsgRF(pRS->CNIAddressOffset)) {
                CNI_SetSRBit(SR_NR);
                #warning "Should the controller transite into PASSIVE state ??"
                CS_ClearMemberBit(pRS->FlagPosition);
                goto _end;             
            }
        }

        _prepare_for_transmit();
        return;
    }
    _end:
    _prepare_for_receive();
}

void psp_for_coldstart(void)
{
    RoundSlotProperty_t* pRS = _slot_property_update();
    
    //INFO("SLOT----------------------------------------%d",MAC_GetRoundSlot());
    //INFO("mode:%d",CALC_MODE_NUM(CS_GetCurMode()));
    //INFO("agreed:%d",PV_GetCounter(AGREED_SLOTS_COUNTER));
    //INFO("failed:%d",PV_GetCounter(FAILED_SLOTS_COUNTER));
    
    //INFO("SSS COLDSTART -- TIME:%u",_G_SlotStartMacrotickTime);
       
    if (MAC_IsOwnNodeSlot()) {
        uint32_t clique_res = SVC_CliqueDetect();
        if(clique_res == CLIQUE_MINORITY){
            //INFO("CLIQUE_MINORITY");
            FSM_TransitIntoState(FSM_LISTEN);
            return;
        }else if(clique_res == CLIQUE_NO_ACTIVITY){
            //INFO("CLIQUE_NO_ACTIVITY");
            FSM_TransitIntoState(FSM_SUB_CS);
            return;
        }else{
            //majority clique
            PV_ClrCounter(AGREED_SLOTS_COUNTER);
            PV_ClrCounter(FAILED_SLOTS_COUNTER);
            //cstate valid now, the controller will notify the host in the next AT time
            //by interruption.
            CNI_SetISRBit(ISR_CV);
            
            #warning "just for test, when the cstate is valid, the host shall raise a mode change request if necessary"
            uint32_t dmc = CS_GetCurDMC();
            if(dmc==DMC_NO_REQ)
                HOST_ModeChange(DMC_MODE_1);
        }
    }

    //the node is in passive or active now, because of the delay of the FSM,
    //the state will be changed in the next transition

    if (MAC_IsSendSlot()) {
        if (!SVC_CheckHostLifeSign()) {
            //passive state now
            CS_ClearMemberBit(pRS->FlagPosition);
            FSM_TransitIntoState(FSM_PASSIVE);
            _prepare_for_receive();
            return;
        }

        //check the mode request field
        if (CNI_IsModeChangeRequested()) {
            if (!(pRS->SlotFlags & SlotFlags_ModeChangePermission)) {
                CNI_SetSRBit(SR_MV);
                CNI_ClrMCR();
                CS_ClearMemberBit(pRS->FlagPosition);
                FSM_TransitIntoState(FSM_PASSIVE);
                _prepare_for_receive();
                return;
            } 
        }
        FSM_TransitIntoState(FSM_ACTIVE);
        _prepare_for_transmit();//transmit a I-frame, normally.
        return;
    }
    _prepare_for_receive();
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
    _G_ClusterCycleStartTime    = CycleStartTime;
    _G_TDMARoundStartTimeOffset = TDMAStartOffset;
}
