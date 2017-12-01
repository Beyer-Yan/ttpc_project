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
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "virhw.h"

#include "ttpservice.h"

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
    while (size--) {
        *dst++ = *src++;
    }
}

/**
 * The function check the crc32 of the frame received according the parameters.
 * @param  pdata   the byte pointer to the ttp frame
 * @param  size    the size of the valid data of the frame received
 * @param  type    the frame type, FRAME_TYPE_IMPLICIT or FRAME_TYPE_EXPLICIT
 * @return         the crc32 calculated
 * @attention      4-byte alignment is needed while calculating the crc32. So, if a
 *                 pdata with no 4-byte-alignment is passed into the function, it will
 *                 be casted into a 4-byte-alignment pointer by introducing an extra
 *                 temperory 4-byte-alignment variable, which will reduce system
 *                 performance.
 */
static uint32_t _frame_crc32_calc(uint8_t* pdata, int size, uint32_t type)
{

    int remain = 0;
    int quotient = 0;

    //for 32-bit architecture
    quotient = size % 4;
    remain = size / 4;

    //case of the 4-byte-alignment access
    uint32_t tmp;
    int i = 0;

    uint32_t crc32;
    uint32_t crc32_mask[3] = { 0xff, 0xffff, 0xffffff };

    uint32_t ScheduleID = MAC_GetClusterScheduleID();

    CRC_ResetData();
    CRC_Calc(ScheduleID);

    for (i = 0; i < quotient; i++) {
        _byte_copy((uint8_t*)&tmp, pdata + i * 4, 4);
        CRC_Calc(tmp);
    }
    if (remain != 0) {
        _byte_copy((uint8_t*)&tmp, pdata + quotient * 4, remain);
        CRC_Calc(tmp & crc32_mask[remain - 1]);
    }

    /**
     * check the frame type, if implicit frame is assembled, the implicit crc check
     * shall be performed.
     */
    if (type == FRAME_TYPE_IMPLICIT) {
        c_state_t c_state;
        MAC_GetCState(&c_state);
        CRC_CalcBlock((uint32_t*)&c_state,sizeof(c_state));
    }
    return CRC_GetCRC();
}

/**
 * This function checks the crc of the given frame descriptor.
 * @param  pDesc the frame descriptor
 * @param  type  the frame type, FRAME_TYPE_IMPLICIT or FRAME_TYPE_EXPLICIT
 * @return       0 if the crc check is not passed
 *               1 if the crc check is passed
 */
static uint32_t _frame_crc32_check(ttp_frame_desc_t* pDesc, uint32_t type)
{
    uint32_t frame_crc32;
    uint32_t checked_crc32;
    c_state_t c_state;
    RoundSlotProperty_t* pRS;

    uint8_t* crc_pos;

    crc_pos = (uint8_t*)(pDesc->pFrame) + pDesc->length - sizeof(frame_crc32);
    pRS = MAC_GetRoundSlotProperties();

    _byte_copy((uint8_t*)&frame_crc32, crc_pos, sizeof(frame_crc32));

    checked_crc32 = _frame_crc32_calc((uint8_t*)pDesc->pFrame, pDesc->length - sizeof(frame_crc32), type);
    if (checked_crc32 != frame_crc32)
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
        if (!CS_IsSame(&c_state))
            return 0;
    }
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
    uint32_t mai = MAC_GetMacrotickParameter();
    uint32_t ratio = MAC_GetRatio();

    uint32_t win_left = mid_axis - 1 * mai / ratio;
    uint32_t win_right = mid_axis + 1 * mai / ratio;

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

    uint32_t rcv_res = MAC_GetReceivedFlag(channel);

    /** for FRAME_NULL checking */
    if (rcv_res == MAC_ERX_NON) {
        *pframe_status = FRAME_NULL;
        return _FRAME_INVALID_;
    }

    /** for FRMAE_INV checking */
    if (rcv_res == MAC_ERX_INV) {
        *pframe_status = FRAME_INVALID;
        return _FRAME_INVALID_;
    }

    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    ttp_frame_desc_t* pDesc = MAC_GetFrameDesc(channel);

    uint32_t frame = pRS->FrameType == FRAME_TYPE_IMPLICIT ? FRAME_N : (!pRS->AppDataLength ? FRAME_I : FRAME_X);

    //for time window ...
    uint32_t mid_axis = SVC_GetAlignedEstimateArivalTimeInterval() + MAC_GetATMicroticks();

    if (!_judge_time_window(mid_axis, pDesc->rcv_timestamp))
        return _FRAME_INVALID_;

    int actual_frame_size;
    switch (frame) {
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
    default: break;
    }

    if (actual_frame_size != pDesc->length) {
        *pframe_status = FRAME_INVALID;
        return _FRAME_INVALID_;
    }

    //uint32_t frame_crc32;

    /** copy the crc field of the frame received, guaranteeing the 4-byte-alignment access*/
    //_byte_copy(&frame_crc32, (uint8_t*)(pDesc->pFrame)+actual_frame_size-4, 4);

    uint8_t header = pDesc->pFrame->hdr[0];
    //uint32_t frame_mcr = (header & ~1) << 2;

    if ((header & 1) != (pRS->FrameType)) {
        *pframe_status = FRAME_INCORRECT;
        return _FRAME_INVALID_;
    }

    return _FRAME_VALID_;
}

static inline void _process_mcr(uint32_t mcr)
{
    //invalid mode error is not considerated here
    uint32_t dmc = DMC_NO_REQ;
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
        break;
    }
    CS_SetDMC(dmc);
}

static uint32_t _get_frame_status(ttp_frame_desc_t* pDesc_ch, uint32_t type)
{
    int res = _frame_crc32_check(pDesc_ch, type);
    uint32_t frame_status;

    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    if (res) {
        uint8_t header;
        uint8_t frame_mcr; 

        frame_status = FRAME_CORRECT;
        header = pDesc_ch->pFrame->hdr[0];
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
    uint32_t frame_status_ch[2];

    uint32_t slot_status;

    ttp_frame_desc_t* pDesc_ch[2] = { NULL, NULL };
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    res_ch[0] = _judge_valid(&frame_status_ch[0], CH0);
    res_ch[1] = _judge_valid(&frame_status_ch[1], CH1);

    CS_SetMemberBit(pRS->FlagPosition);

    if ((res_ch[0] == _FRAME_INVALID_) && (res_ch[1] == _FRAME_INVALID_)) {
        CS_ClearMemberBit(pRS->FlagPosition);
    } else if (res_ch[0] == _FRAME_VALID_) {
        pDesc_ch[0] = MAC_GetFrameDesc(CH0);
        frame_status_ch[0] = _get_frame_status(pDesc_ch[0], pRS->FrameType);
    } else {
        pDesc_ch[1] = MAC_GetFrameDesc(CH1);
        frame_status_ch[1] = _get_frame_status(pDesc_ch[1], pRS->FrameType);
    }

    slot_status = MIN(frame_status_ch[0], frame_status_ch[1]);

    if ((frame_status_ch[0] == FRAME_NULL && frame_status_ch[1] == FRAME_INVALID) ||
        (frame_status_ch[1] == FRAME_NULL && frame_status_ch[0] == FRAME_INVALID)) {
        //@see "Time Triggered Protocol Spec, Page 72"
        slot_status = FRAME_NULL;
    }

    if (slot_status == FRAME_CORRECT) {

        /** for mode change processor */
        ttp_frame_desc_t* pDesc_chosed;

        pDesc_chosed = frame_status_ch[0] == FRAME_CORRECT ? pDesc_ch[0] : pDesc_ch[1];
        uint8_t header = pDesc_chosed->pFrame->hdr[0];
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
                tsmp_frame = (pDesc_ch[0]->rcv_timestamp + pDesc_ch[1]->rcv_timestamp) / 2;
            } else {
                tsmp_frame = pDesc_ch[frame_status_ch[0] == FRAME_CORRECT ? 0 : 1]->rcv_timestamp;
            }
            SVC_SyncCalcOffset(tsmp_frame);
        }
        /** the data carried by frame will not be pulled into CNI in passive state */
        _is_data_frame() ? MAC_PullAppData(pDesc_chosed) : (void)0;
    } else {
        CS_ClearMemberBit(pRS->FlagPosition);
    }

    int32_t step = (int32_t)pRS->SlotDuration;

    MAC_SetSlotStatus(slot_status);

    _is_data_frame() ? MSG_SetStatus(pRS->CNIAddressOffset, slot_status) : (void)0;
    pRS->ClockSynchronization == CLOCK_SYN_NEEDED ? SVC_ExecSyncSchema(step) : (void)0;
}

/**
 * The function performs the ack stages and fills the check_a and check_b according to
 * the corresponding ack state.
 * 
 * @param check_a [description]
 * @param check_b [description]
 * @param pDesc   [description]
 * @param type    [description]
 */
static void _ack_stage(uint32_t* check_a, uint32_t* check_b, ttp_frame_desc_t* pDesc, uint32_t type)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    NodeProperty_t* pNP = MAC_GetNodeProperties();
    uint32_t ack_state = (uint32_t)PV_GetAckState();

    //store the memberbit patten
    uint32_t pos_flag_np = CS_GetMemberBit(pNP->FlagPosition);
    uint32_t pos_flag_rs = CS_GetMemberBit(pRS->FlagPosition);

    if (type == FRAME_TYPE_EXPLICIT) {
        *check_a = _frame_crc32_check(pDesc, FRAME_TYPE_EXPLICIT);
        *check_b = *check_a;
    } else {
        /** check ia or check iia */
        CS_SetMemberBit(pNP->FlagPosition);
        ack_state == WAIT_FIRST_SUCCESSOR ? CS_SetMemberBit(pRS->FlagPosition) : CS_ClearMemberBit(pRS->FlagPosition);
        *check_a = _frame_crc32_check(pDesc, FRAME_TYPE_IMPLICIT);

        /** check ib or check iib */
        /** check_a and check_b can be performed paralleled */
        CS_ClearMemberBit(pNP->FlagPosition);
        CS_SetMemberBit(pRS->FlagPosition);
        *check_b = _frame_crc32_check(pDesc, FRAME_TYPE_IMPLICIT);
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
 * @return         frame status of channel ch.
 */
static uint32_t _ack_result(ttp_frame_desc_t* pDesc, uint32_t type, uint32_t ch, AckFunc* pFunc)
{
    uint32_t check_a;
    uint32_t check_b;
    uint32_t res;
    uint32_t frame_status;

    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    TTP_ASSERT(pDesc != NULL && pFunc != NULL);

    _ack_stage(&check_a, &check_b, pDesc, type); /**< fill check_a and check_b */
    res = SVC_Acknowledgment(check_a, check_b, ch, pFunc); /**< perform real ack algorithm */

    switch (res) {
    case 0:
        frame_status = FRAME_INCORRECT;
        break;
    case 1:
        frame_status = FRAME_CORRECT;
        break;
    case 2:
        frame_status = FRAME_TENTATIVE;
        break;
    case 3:
        frame_status = FRAME_CORRECT;
        break;
    default:
        break;
    }

    if ((frame_status == FRAME_CORRECT) || (frame_status == FRAME_TENTATIVE)) {
        uint8_t header;
        uint32_t frame_mcr;

        header = pDesc->pFrame->hdr[0];
        frame_mcr = ((header & ~1) << 2) & 0xf;

        if (frame_mcr != MCR_MODE_CLR) {
            //mode changing is carried in the frame.
            if (pRS->ModeChangePermission == MODE_CHANGE_DENY) {
                frame_status = FRAME_MODE_VIOLATION;
            }
        }
    }

    return frame_status;
}

void prp_for_active(void)
{
    uint32_t slot_acq = MAC_GetSlotAcquisition();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
    //ScheduleParameter_t* pSP = MAC_GetScheduleParameter();
    //NodeProperty_t* pNP = MAC_GetNodeProperties();

    if (slot_acq == SENDING_FRAME) {
        PV_SetAckState(WAIT_FIRST_SUCCESSOR);
        SVC_AckInit();

        if(_is_data_frame()){
            MSG_SetStatus(pRS->CNIAddressOffset,FRAME_CORRECT);
        }
        
        return;
    }

    uint32_t res[2];
    uint32_t frame_status_ch[2];

    uint32_t chosed_ch;

    uint32_t slot_status;

    ttp_frame_desc_t* pDesc_ch[2];

    res[0] = _judge_valid(&frame_status_ch[0], CH0);
    res[1] = _judge_valid(&frame_status_ch[1], CH1);

    AckFunc func[2] = {NULL,NULL};
    /**
     * The reception of a correct frame from a sending node requires that the receiver sets
     * the membership flag of the sender to TRUE before checking the frame CRC.
     */
    CS_SetMemberBit(pRS->FlagPosition);

    pDesc_ch[0] = res[0] == _FRAME_VALID_ ? MAC_GetFrameDesc(CH0) : NULL;
    pDesc_ch[1] = res[1] == _FRAME_VALID_ ? MAC_GetFrameDesc(CH1) : NULL;

    if (ACK_FINISHED == PV_GetAckState()) {
        //ack is finished, the node has no need to perform ack algorithm
        if ((res[0] == _FRAME_INVALID_) && (res[1] == _FRAME_INVALID_)) {
            CS_ClearMemberBit(pRS->FlagPosition);
        } else {
            frame_status_ch[0] = pDesc_ch[0] != NULL ? _get_frame_status(pDesc_ch[0], pRS->FrameType)
                                                     : frame_status_ch[0];

            frame_status_ch[1] = pDesc_ch[1] != NULL ? _get_frame_status(pDesc_ch[1], pRS->FrameType)
                                                     : frame_status_ch[1];

            chosed_ch = frame_status_ch[0] < frame_status_ch[1] ? 0 : 1;
        }
    } else {
        if ((res[0] == _FRAME_INVALID_) && (res[1] == _FRAME_INVALID_)) {
            CS_ClearMemberBit(pRS->FlagPosition);
        } else {
            uint32_t type = pRS->FrameType;

            frame_status_ch[0] = pDesc_ch[0] != NULL ? _ack_result(pDesc_ch[0], type, 0, &func[0]) : frame_status_ch[0];
            frame_status_ch[1] = pDesc_ch[1] != NULL ? _ack_result(pDesc_ch[1], type, 1, &func[1]) : frame_status_ch[1];

            if (frame_status_ch[0] != frame_status_ch[1]) {
                chosed_ch = frame_status_ch[0] < frame_status_ch[1] ? 0 : 1;
                SVC_AckMerge(chosed_ch);
            }
        }

        func[chosed_ch] != NULL ? func[chosed_ch]() : (void)0;
    }

    slot_status = MIN(frame_status_ch[0], frame_status_ch[1]);
    if (!(frame_status_ch[0] != FRAME_NULL && frame_status_ch[1] != FRAME_NULL)) {
        //@see "Time Triggered Protocol Spec, Page 72"
        slot_status = FRAME_NULL;
    }

    if (slot_status == FRAME_CORRECT) {
        uint8_t header = pDesc_ch[chosed_ch]->pFrame->hdr[0];
        uint32_t frame_mcr = (header & ~1) << 2;

        _process_mcr(frame_mcr);

        /** for syncronization operation */
        if (pRS->SynchronizationFrame == SYN_FRAME) {
            uint32_t tsmp_frame;

            if ((frame_status_ch[0] == FRAME_CORRECT) && (frame_status_ch[1] == FRAME_CORRECT)) {
                tsmp_frame = (pDesc_ch[0]->rcv_timestamp + pDesc_ch[1]->rcv_timestamp) / 2;
            } else {
                tsmp_frame = pDesc_ch[chosed_ch]->rcv_timestamp;
            }
            SVC_SyncCalcOffset(tsmp_frame);
        }
        /** the data carried by frame will not be pulled into CNI in passive state */
        //MSG_SetStatus(pRS->CNIAddressOffset, FRAME_CORRECT);
        if (_is_data_frame()) {
            MAC_PullAppData(pDesc_ch[chosed_ch]);
        }
    }

    int32_t step = (int32_t)pRS->SlotDuration;

    /** for the assumption that channel 0 and channel 1 are the same */
    _is_data_frame() ? MSG_SetStatus(pRS->CNIAddressOffset, slot_status) : (void)0;

    pRS->ClockSynchronization == CLOCK_SYN_NEEDED ? SVC_ExecSyncSchema(step) : (void)0;
}

void prp_for_coldstart(void)
{
    uint32_t slot_acq = MAC_GetSlotAcquisition();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    if (slot_acq == SENDING_FRAME) {
        return;
    }

    uint32_t res_ch[2];
    uint32_t frame_status_ch[2];
    uint32_t slot_status;
    ttp_frame_desc_t* pDesc_ch[2];

    res_ch[0] = _judge_valid(&frame_status_ch[0],CH0);
    res_ch[1] = _judge_valid(&frame_status_ch[1],CH1);

    CS_SetMemberBit(pRS->FlagPosition);

    if ((res_ch[0] == _FRAME_INVALID_) && (res_ch[1] == _FRAME_INVALID_)) {
        CS_ClearMemberBit(pRS->FlagPosition);
    } else if (res_ch[0] == _FRAME_VALID_) {
        pDesc_ch[0] = MAC_GetFrameDesc(CH0);
        frame_status_ch[0] = _get_frame_status(pDesc_ch[0], pRS->FrameType);
    } else {
        pDesc_ch[1] = MAC_GetFrameDesc(CH1);
        frame_status_ch[1] = _get_frame_status(pDesc_ch[1], pRS->FrameType);
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
        ttp_frame_desc_t* pDesc_chosed;

        pDesc_chosed = frame_status_ch[0] == FRAME_CORRECT ? pDesc_ch[0] : pDesc_ch[1];
        uint8_t header = pDesc_chosed->pFrame->hdr[0];
        uint32_t frame_mcr = (header & ~1) << 2;

        _process_mcr(frame_mcr);

        /** for syncronization operation */
        if (pRS->SynchronizationFrame == SYN_FRAME) {
            uint32_t tsmp_frame;

            if ((frame_status_ch[0] == FRAME_CORRECT) && (frame_status_ch[1] == FRAME_CORRECT)) {
                tsmp_frame = (pDesc_ch[0]->rcv_timestamp + pDesc_ch[1]->rcv_timestamp) / 2;
            } else {
                tsmp_frame = pDesc_ch[frame_status_ch[0] == FRAME_CORRECT ? 0 : 1]->rcv_timestamp;
            }
            SVC_SyncCalcOffset(tsmp_frame);
        }
        /**No data will be carried in cold start mode */
    } else {
        CS_ClearMemberBit(pRS->FlagPosition);
    }

    int32_t step = (int32_t)pRS->SlotDuration;

    MAC_SetSlotStatus(slot_status);
    pRS->ClockSynchronization == CLOCK_SYN_NEEDED ? SVC_ExecSyncSchema(step) : (void)0;
}