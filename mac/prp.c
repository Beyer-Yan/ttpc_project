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
 * When no frames are received, the function will set CHANNEL_DUMMY flag, meaning that
 * there is not sender sending frame in the current slot. If an invalid activity was 
 * observed only one channel, and silence on the other channel, transient noise is 
 * assumed. 
 *
 * If transmission activity was observed on both channels, the node's first successor is 
 * considered to have transmitted and failed, and the channel activity flag shall be set
 * to CHANNEL_ACTIVE.
 */

#define _FRAME_VALID_   1
#define _FRAME_INVALID_ 0

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

/**
 * The function check the crc32 of the frame received according the parameters.
 * @param  pdata   the byte pointer to the ttp frame
 * @param  size    the size of the valid data of the frame received
 * @param  type    the frame type, FRAME_TYPE_IMPLICIT or FRAME_TYPE_EXPLICIT
 * @return         the crc32 calculated
 */
static uint32_t _frame_crc32_calc(uint8_t* pdata, int size, uint32_t type)
{
    uint32_t ScheduleID = MAC_GetClusterScheduleID();

    CRC_ResetData();

    CRC_PushData((uint8_t*)&ScheduleID,4);
    CRC_PushData(pdata,size);

    /**
     * check the frame type, if implicit frame is assembled, the implicit crc check
     * shall be performed.
     */
    if (type == FRAME_TYPE_IMPLICIT) {
        c_state_t c_state;
        CS_GetCState(&c_state);
        CRC_PushData((uint8_t*)&c_state,sizeof(c_state));
    }
    return CRC_GetResult();
}

/**
 * This function checks the crc of the given frame descriptor.
 * @param  pDesc the frame descriptor
 * @param  type  the frame type, FRAME_TYPE_IMPLICIT or FRAME_TYPE_EXPLICIT
 * @return       0 if the crc check is not passed
 *               1 if the crc check is passed
 */
static uint32_t _frame_crc32_check(TTP_ChannelFrameDesc* pDesc, uint32_t type)
{
    uint32_t frame_crc32;
    uint32_t checked_crc32;
    c_state_t c_state;

    uint8_t* crc_pos;

    TTP_ASSERT(pDesc!=NULL);

    if(pDesc->status==MAC_ERX_ECRC)
        return 0;

    if (type == FRAME_TYPE_EXPLICIT) {
        /**
         * The cstate in the x frame has the same position as the cstate in the i_cs frame. The following
         * expression is also correct:
         *     _byte_copy(&c_state,pDesc->pFrame->i_cs->cstate,sizeof(c_state));
         *
         * If the frame has an explicit c-state, the c-states between the sender and the receiver have to
         * be explicitly compared. If these two c-states differ, then the same case distinction as with a 
         * c-state error of an error of an implicit c-state has to be made.
         */
        _byte_copy((uint8_t*)&c_state, pDesc->pFrame->x.cstate, sizeof(c_state));

        if (!CS_IsSame(&c_state)){
            return 0;
        }
        else
            return 1;
    }

    crc_pos = (uint8_t*)(pDesc->pFrame) + pDesc->length - sizeof(frame_crc32);
    //pRS = MAC_GetRoundSlotProperties();

    _byte_copy((uint8_t*)&frame_crc32, crc_pos, sizeof(frame_crc32));

    checked_crc32 = _frame_crc32_calc((uint8_t*)pDesc->pFrame, pDesc->length - sizeof(frame_crc32), type);
    if (checked_crc32 != frame_crc32)
        return 0;

    return 1;
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

static uint32_t _judge_valid(uint32_t* pframe_status, uint32_t channel)
{
    /** for FRAME_NULL checking */
    if (!MSG_CheckReceived(channel)) {
        *pframe_status = FRAME_NULL;
        //INFO("received nothing on ch:%d",channel);
        return _FRAME_INVALID_;
    }
    TTP_FrameDesc *pFrameDesc = MSG_GetFrameDesc();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    TTP_ChannelFrameDesc *pDesc = channel==CH0?pFrameDesc->pCH0:pFrameDesc->pCH1;

    uint32_t type = pRS->FrameType == FRAME_TYPE_IMPLICIT ? FRAME_N : (!pRS->AppDataLength ? FRAME_I : FRAME_X);

    //for time window ...
    uint32_t mid_axis = SVC_GetAlignedEstimateArivalTimeInterval() + MAC_GetATMicroticks();

    if(pDesc->status==MAC_ERX_LTH || pDesc->status==MAC_ERX_COL){
        *pframe_status = FRAME_INVALID;
        INFO("received a frame, but frame corrupted on ch:%d",channel);
        return _FRAME_INVALID_;
    }

    if (!_judge_time_window(mid_axis, pDesc->rcv_timestamp)){
        INFO("received a frame, but timing err. ch:%u, mid:%u, tsmp:%u",channel,mid_axis,pDesc->rcv_timestamp);
        return _FRAME_INVALID_;
    }

    int actual_frame_size = 0;
    switch (type) {
    /**< 1 byte header + 16 bytes c-state + 4 bytes crc32 */
    case FRAME_I:
        actual_frame_size = 1 + 16 + 4;
        break;
    /** 1 byte header + 0 byte c-state + 4 bytes crc32 + application data size */
    case FRAME_N:
        actual_frame_size = 1 + 0 + 4 + pRS->AppDataLength;
        break;
    /** 1 byte header + 16 bytes c-state + 4 bytes crc1 + application data size + 4 bytes crc2 */
    case FRAME_X:
        actual_frame_size = 1 + 16 + 4 + pRS->AppDataLength + 4;
        break;
    }

    if (actual_frame_size != pDesc->length) {
        *pframe_status = FRAME_INVALID;
        INFO("received a frame, but size error on ch:%d",channel);
        return _FRAME_INVALID_;
    }

    uint8_t header = pDesc->pFrame->hdr[0];

    if ((header & 1) != (pRS->FrameType)) {
        *pframe_status = FRAME_INCORRECT;
        INFO("received a frame, but type error on ch:%d",channel);
        return _FRAME_INVALID_;
    }
    //INFO("received a valid frame on ch:%d",channel);
    return _FRAME_VALID_;
}

static inline void _process_mcr(uint32_t mcr)
{
    //invalid mode error is not considerated here
    uint32_t dmc = DMC_NO_REQ;

    TTP_ASSERT(IS_TTP_MCR(mcr));

    switch (mcr) {
    case MCR_MODE_1:
        dmc = DMC_MODE_1;
        break;
    case MCR_MODE_2:
        dmc = DMC_MODE_2;
        break;
    case MCR_MODE_3:
        dmc = DMC_MODE_3;
        break;
    case MCR_MODE_CLR:
        dmc = DMC_NO_REQ;
        break;
    default:
        dmc = DMC_NO_REQ;
        break;
    }
    CS_SetDMC(dmc);
}
//The function shall be called when the frame of the corresponding channel is valid
static uint32_t _get_valid_frame_status(TTP_ChannelFrameDesc* pDesc, uint32_t type)
{
    int res = _frame_crc32_check(pDesc, type);
    uint32_t frame_status;

    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    if (res) {
        uint8_t header;
        uint8_t frame_mcr; 

        frame_status = FRAME_CORRECT;
        header = pDesc->pFrame->hdr[0];
        frame_mcr = (header & ~1) << 2;

        if (frame_mcr != MCR_MODE_CLR) {
            //mode changing is carried in the frame.
            if (pRS->ModeChangePermission == MODE_CHANGE_DENY) {
                frame_status = FRAME_MODE_VIOLATION;
            }
        }
    } else {
        frame_status = FRAME_INCORRECT;
    }

    return frame_status;
}

static inline uint32_t _is_data_frame()
{
    // the legality of the slot configuration shall be checked upper application
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    return ((pRS->FrameType == FRAME_TYPE_IMPLICIT) || (pRS->AppDataLength) ? 1 : 0);
}

void prp_for_passive(void)
{
    uint32_t res_ch[2];
    uint32_t frame_status_ch[2]={0xffffffff,0xffffffff};

    uint32_t slot_status;

    INFO("RRR PASSIVE   -- TIME:%u",CLOCK_GetCurMacrotick());

    TTP_FrameDesc* pDesc = NULL;
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    res_ch[0] = _judge_valid(&frame_status_ch[0], CH0);
    res_ch[1] = _judge_valid(&frame_status_ch[1], CH1);

    CS_SetMemberBit(pRS->FlagPosition);
    pDesc = MSG_GetFrameDesc();

    if ((res_ch[0] == _FRAME_INVALID_) && (res_ch[1] == _FRAME_INVALID_)) {
        CS_ClearMemberBit(pRS->FlagPosition);
    } else{
        if (res_ch[0] == _FRAME_VALID_)
            frame_status_ch[0] = _get_valid_frame_status(pDesc->pCH0, pRS->FrameType);
        if (res_ch[1] == _FRAME_VALID_)
            frame_status_ch[1] = _get_valid_frame_status(pDesc->pCH1, pRS->FrameType);
    } 

    slot_status = MIN(frame_status_ch[0], frame_status_ch[1]);

    if ((frame_status_ch[0] == FRAME_NULL && frame_status_ch[1] == FRAME_INVALID) ||
        (frame_status_ch[1] == FRAME_NULL && frame_status_ch[0] == FRAME_INVALID)) {
        //@see "Time Triggered Protocol Spec, Page 72"
        slot_status = FRAME_NULL;
    }

    if (slot_status == FRAME_CORRECT) {

        /** for mode change processor */
        TTP_ChannelFrameDesc* pDesc_chosen;

        pDesc_chosen = frame_status_ch[0] == FRAME_CORRECT ? pDesc->pCH0 : pDesc->pCH1;
        uint8_t header = pDesc_chosen->pFrame->hdr[0];
        uint32_t frame_mcr = (header & ~1) << 2;

        _process_mcr(frame_mcr);

        /** for maintaining the integration counter */
        uint32_t integration_cnt = PV_GetCounter(INTEGRATION_COUNTER);
        uint32_t integration_min = pSP->MinimumIntegrationCount;

        if (integration_cnt < integration_min)
            PV_IncCounter(INTEGRATION_COUNTER);

        /** for syncronization operation */
        if (pRS->SynchronizationFrame == SYN_FRAME) {
            uint32_t tsmp_frame;

            if ((frame_status_ch[0] == FRAME_CORRECT) && (frame_status_ch[1] == FRAME_CORRECT)) {
                tsmp_frame = (pDesc->pCH0->rcv_timestamp)/2 + (pDesc->pCH1->rcv_timestamp)/2;
            } else {
                tsmp_frame = frame_status_ch[0] == FRAME_CORRECT ? pDesc->pCH0->rcv_timestamp : 
                                                                   pDesc->pCH1->rcv_timestamp;
            }
            SVC_SyncCalcOffset(tsmp_frame);
        }
        #warning "Should the data carried by frame be pulled into CNI in PASSIVE state ?? "
        if(_is_data_frame())
            MSG_PullAppData(pDesc_chosen);
        
    }else{
        CS_ClearMemberBit(pRS->FlagPosition);
    }

    MAC_SetSlotStatus(slot_status);

    if(_is_data_frame()) { MSG_SetStatus(pRS->CNIAddressOffset, slot_status); }
    
    if(pRS->ClockSynchronization == CLOCK_SYN_NEEDED){
        if(!SVC_ExecSyncSchema(0)){
            CNI_SetSRBit(SR_SE);
            //INFO("SYNC ERROR");
            FSM_TransitIntoState(FSM_FREEZE);
        }
    } 
}

/**
 * The function performs the ack stages and fills the check_a and check_b according to
 * the corresponding ack state.
 * 
 * @param check_a I_a or II_a to be filled
 * @param check_b I_b or II_B to be filled
 * @param pDesc   the pointer of frame descriptor
 * @param type    the frame type, implicit or explicit
 */
static void _ack_stage(uint32_t* check_a, uint32_t* check_b, TTP_ChannelFrameDesc* pDesc, uint32_t type)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    NodeProperty_t* pNP = MAC_GetNodeProperties();
    uint32_t ack_state = (uint32_t)PV_GetAckState();

    //store the memberbit patten
    uint32_t pos_flag_np = CS_GetMemberBit(pNP->FlagPosition);
    uint32_t pos_flag_rs = CS_GetMemberBit(pRS->FlagPosition);

    if (type == FRAME_TYPE_EXPLICIT) {
        *check_a = _frame_crc32_check(pDesc, FRAME_TYPE_EXPLICIT)==1 ? ACK_CHECK_T : ACK_CHECK_F;
        *check_b = *check_a;
    } else {
        /** check ia or check iia */
        CS_SetMemberBit(pNP->FlagPosition);
        ack_state == WAIT_FIRST_SUCCESSOR ? CS_SetMemberBit(pRS->FlagPosition) : CS_ClearMemberBit(pRS->FlagPosition);
        *check_a = _frame_crc32_check(pDesc, FRAME_TYPE_IMPLICIT)==1 ? ACK_CHECK_T : ACK_CHECK_F;

        /** check ib or check iib */
        /** check_a and check_b can be performed paralleled */
        CS_ClearMemberBit(pNP->FlagPosition);
        CS_SetMemberBit(pRS->FlagPosition);
        *check_b = _frame_crc32_check(pDesc, FRAME_TYPE_IMPLICIT)==1 ? ACK_CHECK_T : ACK_CHECK_F;
    }

    //restore the membership pattern
    pos_flag_np ? CS_SetMemberBit(pNP->FlagPosition) : CS_ClearMemberBit(pNP->FlagPosition);
    pos_flag_rs ? CS_SetMemberBit(pRS->FlagPosition) : CS_ClearMemberBit(pRS->FlagPosition);
}

/**
 * The function performs as a wrapper of SVC_Acknowledgment.
 * @param  pDesc   the pointer the frame descriptor, can not be NULL
 * @param  type    the frame type, FRAME_TYPE_IMPLICIT or FRAME_TYPE_EXPLICIT.
 * @param  ch      the channel, CH0 or CH1
 * @param  pFunc   the pointer of the ack function.
 * @return         the ack result
 */
static uint32_t _ack_result(TTP_ChannelFrameDesc* pDesc, uint32_t type, uint32_t ch, AckFunc* pFunc, uint32_t *pframe_status)
{
    uint32_t check_a;
    uint32_t check_b;
    uint32_t res;

    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    TTP_ASSERT(pDesc != NULL && pFunc != NULL);

    _ack_stage(&check_a, &check_b, pDesc, type); /**< fill check_a and check_b */
    res = SVC_Acknowledgment(check_a, check_b, ch, pFunc); /**< perform real ack algorithm */

    switch (res) {
    case 0:
        //frame corrupted, wait for the next frame
        *pframe_status = FRAME_INCORRECT;
        break;
    case 1:
        //a failed, but the frame received is correct
        *pframe_status = FRAME_CORRECT;
        break;
    case 2:
        //tentative, need to be confirmed in 2nd successor
        *pframe_status = FRAME_TENTATIVE;
        break;
    case 3:
        //a acked, the frame received is correct
        *pframe_status = FRAME_CORRECT;
        break;
    default:
        *pframe_status = FRAME_INCORRECT;
        break;
    }

    if ((*pframe_status == FRAME_CORRECT) || (*pframe_status == FRAME_TENTATIVE)) {
        uint8_t header;
        uint32_t frame_mcr;

        header = pDesc->pFrame->hdr[0];
        frame_mcr = ((header & ~1) << 2) & 0xf;

        if (frame_mcr != MCR_MODE_CLR) {
            //mode changing is carried in the frame.
            if (pRS->ModeChangePermission == MODE_CHANGE_DENY) {
                //INFO("ack frame mcr error");
                *pframe_status = FRAME_MODE_VIOLATION;
            }
        }
    }

    return res;
}

void prp_for_active(void)
{
    uint32_t slot_acq = MAC_GetSlotAcquisition();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    //ScheduleParameter_t* pSP = MAC_GetScheduleParameter();
    //NodeProperty_t* pNP = MAC_GetNodeProperties();
    INFO("RRR ACTIVE    -- TIME:%u",CLOCK_GetCurMacrotick());
    
    if (slot_acq == SENDING_FRAME) {
        PV_SetAckState(WAIT_FIRST_SUCCESSOR);
        SVC_AckInit();

        if(_is_data_frame()){
            MSG_SetStatus(pRS->CNIAddressOffset,FRAME_CORRECT);
        }
        goto __check_sync;
    }

    uint32_t res_ch[2];
    uint32_t frame_status_ch[2]={0xffffffff,0xffffffff};

    uint32_t chosen_ch;

    uint32_t slot_status;

    TTP_FrameDesc* pDesc = NULL;

    res_ch[0] = _judge_valid(&frame_status_ch[0], CH0);
    res_ch[1] = _judge_valid(&frame_status_ch[1], CH1);
    chosen_ch = 0;

    AckFunc func[2] = {NULL,NULL};
    /**
     * The reception of a correct frame from a sending node requires that the receiver sets
     * the membership flag of the sender to TRUE before checking the frame CRC.
     */
    CS_SetMemberBit(pRS->FlagPosition);
    pDesc = MSG_GetFrameDesc();

    if ((res_ch[0] == _FRAME_INVALID_) && (res_ch[1] == _FRAME_INVALID_)) {
        CS_ClearMemberBit(pRS->FlagPosition);
    } else{
        if (res_ch[0] == _FRAME_VALID_)
            frame_status_ch[0] = _get_valid_frame_status(pDesc->pCH0, pRS->FrameType);
        if (res_ch[1] == _FRAME_VALID_)
            frame_status_ch[1] = _get_valid_frame_status(pDesc->pCH1, pRS->FrameType);
    }  

    if(ACK_FINISHED != PV_GetAckState()){
        //perform ack algorithm
        uint32_t type = pRS->FrameType;
        uint32_t ack_res[2];
        
        if ((res_ch[0] == _FRAME_VALID_) || (res_ch[1] == _FRAME_VALID_)) {
            if(res_ch[0] == _FRAME_VALID_){
                ack_res[0] = _ack_result(pDesc->pCH0, type, CH0, &func[0],&frame_status_ch[0]);
            }
            if(res_ch[1] == _FRAME_VALID_){
                ack_res[1] = _ack_result(pDesc->pCH1, type, CH1, &func[1], &frame_status_ch[1]);
            }

            if (frame_status_ch[0] != frame_status_ch[1]) {
                chosen_ch = frame_status_ch[0] <= frame_status_ch[1] ? 0 : 1;
                SVC_AckMerge(chosen_ch);
            }
            func[chosen_ch] != NULL ? func[chosen_ch]() : (void)0;
            
            if(ack_res[chosen_ch] == 1){
                //ack failed
                //INFO("ack failed");
                uint32_t max_member_fail =  MAC_GetMaximumMembershipFailureCount();
                if(max_member_fail == PV_GetCounter(MEMBERSHIP_FAILED_COUNTER)){
                    //membership loss, the controller shall transmit into FREEZE state.
                    CNI_SetSRBit(SR_ME);
                    //INFO("membership loss");
                    FSM_TransitIntoState(FSM_FREEZE);
                    return;
                }else{
                    INFO("membership failed");              
                    CNI_SetISRBit(ISR_ML);
                    FSM_TransitIntoState(FSM_PASSIVE);
                }
            }            
        }  
    }

    slot_status = MIN(frame_status_ch[0], frame_status_ch[1]);
    if ((frame_status_ch[0] == FRAME_NULL && frame_status_ch[1] == FRAME_INVALID) ||
        (frame_status_ch[1] == FRAME_NULL && frame_status_ch[0] == FRAME_INVALID)) {
        //@see "Time Triggered Protocol Spec, Page 72"
        slot_status = FRAME_NULL;
    }

    if (slot_status == FRAME_CORRECT) {

        TTP_ChannelFrameDesc *pDesc_chosen = frame_status_ch[0] == FRAME_CORRECT ? pDesc->pCH0 : pDesc->pCH1;
        uint8_t header = pDesc_chosen->pFrame->hdr[0];
        uint32_t frame_mcr = (header & ~1) << 2;

        _process_mcr(frame_mcr);

        /** for syncronization operation */
        if (pRS->SynchronizationFrame == SYN_FRAME) {
            uint32_t tsmp_frame;

            if ((frame_status_ch[0] == FRAME_CORRECT) && (frame_status_ch[1] == FRAME_CORRECT)) {
                tsmp_frame = (pDesc->pCH0->rcv_timestamp)/2 + (pDesc->pCH1->rcv_timestamp)/2;
            } else {
                tsmp_frame = pDesc_chosen->rcv_timestamp;
            }
            SVC_SyncCalcOffset(tsmp_frame);
        }
        /** the data carried by frame will not be pulled into CNI in passive state */
        //MSG_SetStatus(pRS->CNIAddressOffset, FRAME_CORRECT);
        if (_is_data_frame()) {
            MSG_PullAppData(pDesc_chosen);
        }
    }else{
        #warning "Shall the controller clear the memberbit when a TENTATIVE frame ?"
        CS_ClearMemberBit(pRS->FlagPosition);
    }
    MAC_SetSlotStatus(slot_status);
    
    c_state_t cstate,cx;
    CS_GetCState(&cstate);
    _byte_copy((uint8_t*)&cx,pDesc->pCH0->pFrame->x.cstate,sizeof(c_state_t));
    INFO("local:%x,%x,%x,%x,%x,%x",cstate.GlobalTime,cstate.ClusterPosition,cstate.Membership[0],cstate.Membership[1],cstate.Membership[2],cstate.Membership[3]);
    INFO("frame:%x,%x,%x,%x,%x,%x",cx.GlobalTime,cx.ClusterPosition,cx.Membership[0],cx.Membership[1],cx.Membership[2],cx.Membership[3]);
    
    /** for the assumption that channel 0 and channel 1 are the same */
    if(_is_data_frame()) { MSG_SetStatus(pRS->CNIAddressOffset, slot_status); }

    __check_sync:
    
    if(pRS->ClockSynchronization == CLOCK_SYN_NEEDED){
        if(!SVC_ExecSyncSchema(0)){
            CNI_SetSRBit(SR_SE);
            //INFO("SYNC ERROR");
            FSM_TransitIntoState(FSM_FREEZE);
        }
    }
}

void prp_for_coldstart(void)
{
    uint32_t x = CLOCK_GetCurMacrotick();
    uint32_t slot_acq = MAC_GetSlotAcquisition();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    INFO("RRR COLDSTART -- TIME:%u",CLOCK_GetCurMacrotick());
    
    if (slot_acq == SENDING_FRAME) {
        goto __check_sync;
    }
    
    uint32_t res_ch[2];
    uint32_t frame_status_ch[2]={0xffffffff,0xffffffff};
    uint32_t slot_status;
    TTP_FrameDesc* pDesc = NULL;

    res_ch[0] = _judge_valid(&frame_status_ch[0],CH0);
    res_ch[1] = _judge_valid(&frame_status_ch[1],CH1);
    
    CS_SetMemberBit(pRS->FlagPosition);
    pDesc = MSG_GetFrameDesc();

    if ((res_ch[0] == _FRAME_INVALID_) && (res_ch[1] == _FRAME_INVALID_)) {
        CS_ClearMemberBit(pRS->FlagPosition);
    } else{
        if (res_ch[0] == _FRAME_VALID_)
            frame_status_ch[0] = _get_valid_frame_status(pDesc->pCH0, pRS->FrameType);
        if (res_ch[1] == _FRAME_VALID_)
            frame_status_ch[1] = _get_valid_frame_status(pDesc->pCH1, pRS->FrameType);
    }    

    slot_status = MIN(frame_status_ch[0], frame_status_ch[1]);
    if ((frame_status_ch[0] == FRAME_NULL && frame_status_ch[1] == FRAME_INVALID) ||
        (frame_status_ch[1] == FRAME_NULL && frame_status_ch[0] == FRAME_INVALID)) {
        //@see "Time Triggered Protocol Spec, Page 72"
        slot_status = FRAME_NULL;
    }

    //No ack need to be performed
    if (slot_status == FRAME_CORRECT) {
      
        /** for mode change processor */
        TTP_ChannelFrameDesc *pDesc_chosen = frame_status_ch[0] == FRAME_CORRECT ? pDesc->pCH0 : pDesc->pCH1;
        uint8_t header = pDesc_chosen->pFrame->hdr[0];
        uint32_t frame_mcr = (header & ~1) << 2;

        _process_mcr(frame_mcr);

        /** for syncronization operation */
        if (pRS->SynchronizationFrame == SYN_FRAME) {
            uint32_t tsmp_frame;

            if ((frame_status_ch[0] == FRAME_CORRECT) && (frame_status_ch[1] == FRAME_CORRECT)) {
                tsmp_frame = (pDesc->pCH0->rcv_timestamp)/2 + (pDesc->pCH1->rcv_timestamp)/2;
            } else {
                tsmp_frame = pDesc_chosen->rcv_timestamp;
            }
            SVC_SyncCalcOffset(tsmp_frame);
        }
        /**No data will be carried in cold start mode */
    } else {
        CS_ClearMemberBit(pRS->FlagPosition);
    }

    MAC_SetSlotStatus(slot_status);
    
    __check_sync:

    if(pRS->ClockSynchronization == CLOCK_SYN_NEEDED){
        if(!SVC_ExecSyncSchema(0)){
            CNI_SetSRBit(SR_SE);
            //INFO("SYNC ERROR");
            FSM_TransitIntoState(FSM_FREEZE);
        }
    }
}
