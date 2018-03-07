/**
  ******************************************************************************
  * @file    	prp.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.01.18
  * @brief   	the Pre-Send-Phase process
  ******************************************************************************
  * @attention
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
#include "led.h"
#include "ttpservice.h"
#include "crc.h"
#include "msg.h"
#include "medl.h"
#include "clock.h"

/**
 * This function sets the channel activity.
 * When no frames are received, the function will set communication CB flag, meaning 
 * that there is not sender sending frame in the current slot. If an invalid activity 
 * was observed only one channel, and silence on the other channel, transient noise is 
 * assumed. 
 *
 * If transmission activity was observed on both channels, the node's first successor is 
 * considered to have transmitted and failed, and the channel activity flag shall be set
 * to CHANNEL_ACTIVE.
 */

#define _INTERNAL_VALID    0xff
#define _INTERNAL_CORRECT  0xfe 

static const char* _slot_status_name[6] = 
{
    "FRAME_CORRECT",
    "FRAME_TENTATIVE",
    "FRAME_MODE_VIOLATION",
    "FRAME_INCORRECT",
    "FRAME_NULL",
    "FRAME_INVALID"
};

static int16_t _G_diff = 0;

/**
 * byte copy for inner use.
 * @param dst  the destination address
 * @param src  the source address
 * @param size the size the the data, which shall be large then or equal to 0. 
 */
static inline void _byte_copy(uint8_t* dst, uint8_t* src, int size)
{
    while (size-->0) {
        *dst++ = *src++;
    }
}

static void data_print_for_test(void)
{
    uint16_t data1,data2,data3,data4,data5,data6;
    
    uint8_t* addr1,*addr2,*addr3,*addr4,*addr5,*addr6;
    
    addr1 = MSG_GetMsgAddr(0);
    addr2 = MSG_GetMsgAddr(4);
    addr3 = MSG_GetMsgAddr(8);
    addr4 = MSG_GetMsgAddr(12);
    addr5 = MSG_GetMsgAddr(16);
    addr6 = MSG_GetMsgAddr(20);
    
    _byte_copy((uint8_t*)&data1,addr1,2);
    _byte_copy((uint8_t*)&data2,addr2,2);
    _byte_copy((uint8_t*)&data3,addr3,2);
    _byte_copy((uint8_t*)&data4,addr4,2);
    _byte_copy((uint8_t*)&data5,addr5,2);
    _byte_copy((uint8_t*)&data6,addr6,2);
    
    INFO("%u,%u,%u,%u,%u,%d",data2,data3,data4,data5,data6,_G_diff);  
}

static void _debug_print(uint32_t slot_status)
{
    INFO("slot_status:%s",_slot_status_name[slot_status]);
    INFO("mode:%d",CALC_MODE_NUM(CS_GetCurMode()));
    INFO("agreed:%d",PV_GetCounter(AGREED_SLOTS_COUNTER));
    INFO("failed:%d",PV_GetCounter(FAILED_SLOTS_COUNTER));  
}

/**
 * Judge whether the value locates in the time window defined by mid_axis
 * @param  mid_axis the middle axis of the time window
 * @param  value    the value to be judged
 * @return          1 for yes, 0 for no.
 */
static uint32_t _judge_time_window(uint32_t mid_axis, uint32_t value)
{
    //uint32_t mai = MAC_GetMacrotickParameter();
    uint32_t ratio = MAC_GetRatio();
    
    uint32_t win_left = mid_axis -  ratio;
    uint32_t win_right = mid_axis + ratio;

    uint32_t res = 0;

    #warning "just for testing"
    _G_diff = value - mid_axis;
    
    if (win_left > win_right) {
        if ((value > win_left) || (value < win_right)) {
            res = 1;
        }
    } else {
        if ((value > win_left) && (value < win_right)) {
            res = 1;
        }
    }

    return res;
}
/**
 * This function judges the validity of the frame received, frames with right size
 * ,right timing, right CRC result and right mode-carrying will be judged as
 * _FRAME_VALID_.
 */
static uint32_t _judge_valid(uint32_t channel)
{
    /** for FRAME_NULL checking */
    if (!MSG_CheckReceived(channel)) {
        //INFO("received nothing on ch:%d",channel);
        return FRAME_NULL;
    }
    TTP_FrameDesc *pFrameDesc = MSG_GetFrameDesc();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    TTP_ChannelFrameDesc *pDesc = channel==CH0?pFrameDesc->pCH0:pFrameDesc->pCH1;

    uint32_t type = pRS->SlotFlags & SlotFlags_FrameTypeExplicit ? (!pRS->AppDataLength ? FRAME_I : FRAME_X) : FRAME_N;

    //for time window ...
    uint32_t mid_axis = SVC_GetAlignedEstimateArivalTimeInterval(channel) + MAC_GetATMicroticks();

    if(pDesc->status==TTP_ERX_LTH || pDesc->status==TTP_ERX_COL){
        //INFO("received a frame, but frame corrupted on ch:%d",channel);
        return FRAME_INVALID;
    }

    if (!_judge_time_window(mid_axis, pDesc->rcv_timestamp)){
        //INFO("received a frame, but timing err. ch:%u, mid:%u, tsmp:%u",channel,mid_axis,pDesc->rcv_timestamp);
        return FRAME_INVALID;
    }

    int actual_frame_size = 0;
    switch (type) {
    /**< 1 byte header + 12 bytes c-state + 4 bytes crc32 */
    case FRAME_I:
        actual_frame_size = 1 + 12 + 4;
        break;
    /** 1 byte header + 0 byte c-state + 4 bytes crc32 + application data size */
    case FRAME_N:
        actual_frame_size = 1 + 0 + 4 + pRS->AppDataLength;
        break;
    /** 1 byte header + 12 bytes c-state + 4 bytes crc1 + application data size + 4 bytes crc2 */
    case FRAME_X:
        actual_frame_size = 1 + 12 + 4 + pRS->AppDataLength + 4;
        break;
    }

    if (actual_frame_size != pDesc->length) {
        //INFO("received a frame, but size error on ch:%d",channel);
        return FRAME_INVALID;
    }

    //INFO("received a valid frame on ch:%d",channel);
    return _INTERNAL_VALID;
}
/**
 * This function is used for frame status checking with no acknowledgement, such as
 * in PASSIVE state process or COLSTART state process or ACTIVE state process but 
 * acknowledgement has been finished.
 * This process is also named C-state scenario 1
 * @return the calculated frame status
 */
static uint32_t _frame_status_check_with_no_ack(TTP_ChannelFrameDesc* pDesc)
{
    uint8_t header = pDesc->pFrame->hdr[0];
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    c_state_t c_state;

    if(pDesc->status==TTP_ERX_CRC)
        return FRAME_INCORRECT;

    if ((header & 1) != !!(pRS->SlotFlags & SlotFlags_FrameTypeExplicit)) {
        return FRAME_INCORRECT;
    }
    #warning "schedule ID is ignored for explicit cstate frame"
    if(pRS->SlotFlags & SlotFlags_FrameTypeExplicit){
        _byte_copy((uint8_t*)&c_state, pDesc->pFrame->x.cstate, sizeof(c_state));
        if(!CS_IsSame(&c_state))
            return FRAME_INCORRECT;
    }else{
        uint8_t* crc32_pos  = (uint8_t*)(pDesc->pFrame) + pDesc->length - 4;
        uint32_t frame_crc32;
        uint32_t checked_crc32;
        uint32_t ScheduleID = MAC_GetClusterScheduleID();

        _byte_copy((uint8_t*)&frame_crc32, crc32_pos, 4);
        CRC_ResetData();
        CRC_PushData((uint8_t*)&ScheduleID,4);
        CRC_PushData((uint8_t*)pDesc->pFrame, pDesc->length - 4);
        
        CS_GetCState(&c_state);
        CRC_PushData((uint8_t*)&c_state,sizeof(c_state));

        checked_crc32 = CRC_GetResult();
        if(checked_crc32!=frame_crc32)
            return FRAME_INCORRECT;
    }
    //now, the frame passed all the checks except the mcr
    uint8_t frame_mcr; 
    frame_mcr = (header & ~1) << 2;

    if (frame_mcr != MCR_NO_REQ) {
        //mode changing is carried in the frame.
        if(!IS_TTP_MCR(frame_mcr) || !(pRS->SlotFlags & SlotFlags_ModeChangePermission)){
            return FRAME_MODE_VIOLATION;
        }
    }

    return FRAME_CORRECT;
}

static uint32_t _frame_status_precheck_within_ack(TTP_ChannelFrameDesc* pDesc)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    uint8_t header = pDesc->pFrame->hdr[0];
     
    if(pDesc->status==TTP_ERX_CRC){
        return FRAME_INCORRECT;
    }
    if ((header & 1) != !!(pRS->SlotFlags & SlotFlags_FrameTypeExplicit)) {
        return  FRAME_INCORRECT;
    }
    return _INTERNAL_CORRECT;
}

/**
 * This function calculates the valid frame status when acknowledgement is needed, which is
 * also name C-state scenario 2.
 * @return ack result. 
 */
static uint32_t _frame_status_check_within_ack(TTP_ChannelFrameDesc* pDesc, uint32_t channel, AckFunc* pFunc, uint32_t* frame_status)
{
    uint32_t ack_res;
    uint32_t type;
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    uint8_t header = pDesc->pFrame->hdr[0];

    type = (pRS->SlotFlags & SlotFlags_FrameTypeExplicit) ? SlotFlags_FrameTypeExplicit : !SlotFlags_FrameTypeExplicit;

    ack_res = SVC_Acknowledgment(pDesc,channel,type,pFunc);

    if(ack_res == ACK_WAITING){
        *frame_status = FRAME_INCORRECT;
    }else if(ack_res==ACK_NEGATIVE){
        *frame_status = FRAME_CORRECT;
    }else if(ack_res == ACK_TENTATIVE){
        *frame_status = FRAME_TENTATIVE;
    }else if(ack_res == ACK_POSITIVE){
        *frame_status = FRAME_CORRECT;
    }

    if(*frame_status==FRAME_CORRECT || *frame_status==FRAME_TENTATIVE){
        uint32_t frame_mcr = (header & ~1) << 2;
        if (frame_mcr != MCR_NO_REQ) {
            //mode changing is carried in the frame.
            if(!IS_TTP_MCR(frame_mcr) || !(pRS->SlotFlags & SlotFlags_ModeChangePermission)){
                *frame_status = FRAME_MODE_VIOLATION;
            }
        }
    }
    return ack_res;
}

static void _process_mcr(TTP_ChannelFrameDesc *pDesc_channel)
{
    //invalid mode error is not considerated here
    uint8_t header     = 0;
    uint32_t frame_mcr = 0;
    uint32_t dmc       = 0;

    if(pDesc_channel==NULL)
        return;

    header = pDesc_channel->pFrame->hdr[0];
    frame_mcr = (header & 0x0e) << 2;

    if(frame_mcr){
        //INFO("frame mcr requested");
        dmc = DMC_NO_REQ;
        if(IS_TTP_MCR(frame_mcr)){
            dmc = MCR_TO_DMC(frame_mcr);
            CS_SetDMC(dmc); 
        }
    }
}

static inline uint32_t _is_data_frame()
{
    // the legality of the slot configuration shall be checked upper application
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    return ( !(pRS->SlotFlags & SlotFlags_FrameTypeExplicit) || (pRS->AppDataLength) ? 1 : 0);
}

static void _process_sync_frame(uint32_t frame_status[2], TTP_FrameDesc* pDesc)
{
    uint32_t tsmp_frame_ch0 = 0;
    uint32_t tsmp_frame_ch1 = 0;
    uint32_t validity_ch0   = 0;
    uint32_t validity_ch1   = 0;
    
    if(frame_status[0] == FRAME_CORRECT){
        validity_ch0   = 1;
        tsmp_frame_ch0 = pDesc->pCH0->rcv_timestamp;
    }
    if(frame_status[1] == FRAME_CORRECT){
        validity_ch1   = 1;
        tsmp_frame_ch1 = pDesc->pCH1->rcv_timestamp;
    }
    SVC_SyncCalcOffset(tsmp_frame_ch0,tsmp_frame_ch1,validity_ch0,validity_ch1, 0);
}

static uint32_t _select_slot_status(uint32_t frame_status[2])
{
    uint32_t slot_status;
    slot_status = MIN(frame_status[0], frame_status[1]);

    if ((frame_status[0] == FRAME_NULL && frame_status[1] == FRAME_INVALID) ||
        (frame_status[1] == FRAME_NULL && frame_status[0] == FRAME_INVALID)) {
        //@see "Time Triggered Protocol Spec, Page 72"
        slot_status = FRAME_NULL;
    }
    return slot_status;
}

void prp_for_passive(void)
{
    uint32_t frame_status_ch[2]={0xffffffff,0xffffffff};
    uint32_t slot_status;

    //INFO("RRR PASSIVE   -- TIME:%u",CLOCK_GetCurMacrotick());

    TTP_FrameDesc* pDesc = NULL;
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    frame_status_ch[0] = _judge_valid(CH0);
    frame_status_ch[1] = _judge_valid(CH1);

    CS_SetMemberBit(pRS->FlagPosition);
   
    if ((frame_status_ch[0] == _INTERNAL_VALID) || (frame_status_ch[1] == _INTERNAL_VALID)) {

        pDesc = MSG_GetFrameDesc();

        if (frame_status_ch[0] == _INTERNAL_VALID){
            frame_status_ch[0] = _frame_status_check_with_no_ack(pDesc->pCH0);
        }
        if (frame_status_ch[1] == _INTERNAL_VALID)
            frame_status_ch[1] = _frame_status_check_with_no_ack(pDesc->pCH1);
    } 

    slot_status = _select_slot_status(frame_status_ch);

    if (slot_status == FRAME_CORRECT) {

        TTP_ChannelFrameDesc *pDesc_chosen = frame_status_ch[0] == FRAME_CORRECT ? pDesc->pCH0 : pDesc->pCH1;
        /** for mode change processor */
        _process_mcr(pDesc_chosen);

        /** for maintaining the integration counter */
        uint32_t integration_cnt = PV_GetCounter(INTEGRATION_COUNTER);
        uint32_t integration_min = pSP->MinimumIntegrationCount;

        if (integration_cnt < integration_min)
            PV_IncCounter(INTEGRATION_COUNTER);

        /** for syncronization operation */
        if (pRS->SlotFlags & SlotFlags_SynchronizationFrame) {
            _process_sync_frame(frame_status_ch,pDesc);
        }
        #warning "Should the data carried by frame be pulled into CNI in PASSIVE state ?? "
        if(_is_data_frame())
            MSG_PullAppData(pDesc_chosen);
        
    }else{
        CS_ClearMemberBit(pRS->FlagPosition);
    }

    MAC_SetSlotStatus(slot_status);

    //_debug_print(slot_status);
    
    if(_is_data_frame()) { MSG_SetStatus(pRS->CNIAddressOffset, slot_status); }
    
    if(pRS->SlotFlags & SlotFlags_ClockSynchronization){
        if(!SVC_ExecSyncSchema(0)){
            CNI_SetSRBit(SR_SE);
            //INFO("SYNC ERROR");
            FSM_TransitIntoState(FSM_FREEZE);
        }
    }    
}

void prp_for_active(void)
{
    uint32_t slot_acq = MAC_GetSlotAcquisition();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    
    uint32_t frame_status_ch[2]={0xffffffff,0xffffffff};
    uint32_t slot_status;
    
    uint32_t ack_finished;
    
    TTP_FrameDesc* pDesc = NULL;
    
    //ScheduleParameter_t* pSP = MAC_GetScheduleParameter();
    //NodeProperty_t* pNP = MAC_GetNodeProperties();
    //INFO("RRR ACTIVE    -- TIME:%u",CLOCK_GetCurMacrotick());
    
    if (slot_acq == SENDING_FRAME) {
        PV_SetAckState(WAIT_FIRST_SUCCESSOR);
        SVC_AckInit();
        
        #warning "Shall the sending node calculate the time difference ?"
        if (pRS->SlotFlags & SlotFlags_SynchronizationFrame) {
            //no frame timestamp should be caculated
            SVC_SyncCalcOffset(0,0,0,0,1);
            _G_diff = 0;
        }
        
        if(_is_data_frame()){
            MSG_SetStatus(pRS->CNIAddressOffset,FRAME_CORRECT);
        }
        goto __check_sync;
    }
    
    frame_status_ch[0] = _judge_valid(CH0);
    frame_status_ch[1] = _judge_valid(CH1);
    /**
     * The reception of a correct frame from a sending node requires that the receiver sets
     * the membership flag of the sender to TRUE before checking the frame CRC.
     */
    CS_SetMemberBit(pRS->FlagPosition);
    ack_finished = ACK_FINISHED == PV_GetAckState();

    if ((frame_status_ch[0] == _INTERNAL_VALID) || (frame_status_ch[1] == _INTERNAL_VALID)) {

        pDesc = MSG_GetFrameDesc();

        if (frame_status_ch[0] == _INTERNAL_VALID)
            frame_status_ch[0] = ack_finished ? _frame_status_check_with_no_ack(pDesc->pCH0) :
                                                _frame_status_precheck_within_ack(pDesc->pCH0);
        if (frame_status_ch[1] == _INTERNAL_VALID)
            frame_status_ch[1] = ack_finished ? _frame_status_check_with_no_ack(pDesc->pCH1) : 
                                                _frame_status_precheck_within_ack(pDesc->pCH1);
    }

    if( !ack_finished && (frame_status_ch[0] == _INTERNAL_CORRECT || 
                          frame_status_ch[1] == _INTERNAL_CORRECT) ){
        uint32_t ack_ch[2] = {0xff,0xff};
        uint32_t chosen_ch;
        AckFunc func[2] = {NULL,NULL};

        if(frame_status_ch[0] == _INTERNAL_CORRECT)
            ack_ch[0] = _frame_status_check_within_ack(pDesc->pCH0,0,&func[0],&frame_status_ch[0]);
        if(frame_status_ch[1] == _INTERNAL_CORRECT)
            ack_ch[1] = _frame_status_check_within_ack(pDesc->pCH1,1,&func[1],&frame_status_ch[1]);
        
        chosen_ch = frame_status_ch[0] <= frame_status_ch[1] ? 0 : 1;
        SVC_AckMerge(chosen_ch);
        func[chosen_ch] != NULL ? func[chosen_ch]() : (void)0;

        if(ack_ch[chosen_ch] == ACK_NEGATIVE){
            //INFO("ack failed");
            uint32_t max_member_fail =  MAC_GetMaximumMembershipFailureCount();
            if(max_member_fail == PV_GetCounter(MEMBERSHIP_FAILED_COUNTER)){
                //membership loss, the controller shall transmit into FREEZE state.
                CNI_SetSRBit(SR_ME);
                //INFO("membership loss");
                FSM_TransitIntoState(FSM_FREEZE);
                return;
            }else{
                //INFO("membership failed");              
                CNI_SetISRBit(ISR_ML);
                FSM_TransitIntoState(FSM_PASSIVE);
            }
        } 
    } 

    slot_status = _select_slot_status(frame_status_ch);
    
    if (slot_status == FRAME_CORRECT) {

        TTP_ChannelFrameDesc *pDesc_chosen = frame_status_ch[0] == FRAME_CORRECT ? pDesc->pCH0 : pDesc->pCH1;
       /** for mode change processor */
        _process_mcr(pDesc_chosen);

        /** for syncronization operation */
        if (pRS->SlotFlags & SlotFlags_SynchronizationFrame) {
            _process_sync_frame(frame_status_ch,pDesc);
        }
        /** the data carried by frame will not be pulled into CNI in passive state */
        //MSG_SetStatus(pRS->CNIAddressOffset, FRAME_CORRECT);
        if (_is_data_frame()) {
            MSG_PullAppData(pDesc_chosen);
        }
    }else{
        CS_ClearMemberBit(pRS->FlagPosition);
    }

    MAC_SetSlotStatus(slot_status);
    //_debug_print(slot_status);

    /** for the assumption that channel 0 and channel 1 are the same */
    if(_is_data_frame()) { MSG_SetStatus(pRS->CNIAddressOffset, slot_status); }

    __check_sync:
    if(pRS->SlotFlags & SlotFlags_ClockSynchronization){
        if(!SVC_ExecSyncSchema(0)){
            CNI_SetSRBit(SR_SE);
            //INFO("SYNC ERROR");
            FSM_TransitIntoState(FSM_FREEZE);
        }
    }
    data_print_for_test();
}

void prp_for_coldstart(void)
{
    uint32_t slot_acq = MAC_GetSlotAcquisition();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    
    TTP_FrameDesc* pDesc = NULL;
    uint32_t frame_status_ch[2]={0xffffffff,0xffffffff};
    uint32_t slot_status;

    //INFO("RRR COLDSTART -- TIME:%u",CLOCK_GetCurMacrotick());
    
    if (slot_acq == SENDING_FRAME) {
        #warning "Shall the sending node calculate the time difference ?"
        if (pRS->SlotFlags & SlotFlags_SynchronizationFrame) {
            SVC_SyncCalcOffset(0,0,0,0,1);
        }
        goto __check_sync;
    }
    
    frame_status_ch[0] = _judge_valid(CH0);
    frame_status_ch[1] = _judge_valid(CH1);
    
    CS_SetMemberBit(pRS->FlagPosition);

    if ((frame_status_ch[0] == _INTERNAL_VALID) && (frame_status_ch[1] == _INTERNAL_VALID)) {

        pDesc = MSG_GetFrameDesc();

        if (frame_status_ch[0] == _INTERNAL_VALID){
            frame_status_ch[0] = _frame_status_check_with_no_ack(pDesc->pCH0);
        }
        if (frame_status_ch[1] == _INTERNAL_VALID)
            frame_status_ch[1] = _frame_status_check_with_no_ack(pDesc->pCH1);
    }    

    slot_status = _select_slot_status(frame_status_ch);

    //No ack need to be performed
    if (slot_status == FRAME_CORRECT) {
      
        TTP_ChannelFrameDesc *pDesc_chosen = frame_status_ch[0] == FRAME_CORRECT ? pDesc->pCH0 : pDesc->pCH1;
        /** for mode change processor */
        _process_mcr(pDesc_chosen);

        /** for syncronization operation */
        if (pRS->SlotFlags & SlotFlags_SynchronizationFrame) {
            _process_sync_frame(frame_status_ch,pDesc);
        }
        /**No data will be carried in cold start mode */
    } else {
        CS_ClearMemberBit(pRS->FlagPosition);
    }

    MAC_SetSlotStatus(slot_status);
    //_debug_print(slot_status);
    
    __check_sync:
    if(pRS->SlotFlags & SlotFlags_ClockSynchronization){
        if(!SVC_ExecSyncSchema(0)){
            CNI_SetSRBit(SR_SE);
            //INFO("SYNC ERROR");
            FSM_TransitIntoState(FSM_FREEZE);
        }
    }
}
