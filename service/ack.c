/**
  ******************************************************************************
  * @file    	ack.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.11
  * @brief   	implementation of acknowledgment algorithm of TTPC
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file provides an implementation of the acknowledgment algorithm and the 
  * frame status checking of the ttpc controller.
  * controller.
  * 
  ******************************************************************************
  */
 
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "virhw.h"
#include "protocol_data.h"
#include "ttpservice.h"
#include "protocol.h"

#define TYPE_DECISION    1
#define TYPE_TENTATIVE   2
#define TYPE_BACKTRACE   3
#define TYPE_POSITIVE    4
#define TYPE_NEGATIVE    5



#define END              100 


/**
 * byte copy for inner use.
 * @param dst  the destination address
 * @param src  the source address
 * @param size the size the the data, which shall be large then or equal to 0. 
 */
// static inline void _byte_copy(uint8_t *dst, uint8_t *src, int size)
// {
//     while(size--)
//     {
//         *dst++ = *src++;
//     }
// }

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
// static uint32_t _frame_crc32_calc(uint8_t *pdata, int size, uint32_t type)
// {

//     int remain   = 0;
//     int quotient = 0;

//     //for 32-bit architecture
//     quotient = size%4;
//     remain   = size/4;

//     //case of the 4-byte-alignment access
//     uint32_t tmp;
//     int i = 0;

//     uint32_t crc32;
//     uint32_t crc32_mask[3] = {0xff,0xffff,0xffffff};

//     uint32_t ScheduleID = MAC_GetClusterScheduleID();

//     CRC_ResetData();
//     CRC_Calc(ScheduleID);

//     for(i=0;i<quotient;i++)
//     {
//         _byte_copy(&tmp,pdata+i*4,4);
//         CRC_Calc(tmp);
//     }
//     if(remain!=0)
//     {
//         _byte_copy(&tmp, pdata+quotient*4, remain);
//         CRC_Calc(tmp&crc32_mask[remain-1]);
//     }

//     /**
//      * check the frame type, if impicit frame is assembled, the implicit crc check
//      * shall be performed.
//      */
//     if(type==FRAME_TYPE_IMPLICIT)
//     {
//         c_state_t c_state;
//         MAC_GetCState(&c_state);
//         CRC_CalcBlock(&c_state,sizeof(c_state);
//     }
//     return CRC_GetCRC();
// }

/**
 * This function checks the crc of the given frame descriptor.
 * @param  pDesc the frame descriptor
 * @param  type  the frame type, FRAME_TYPE_IMPLICIT or FRAME_TYPE_EXPPLICIT
 * @return       0 if the crc check is not passed
 *               1 if the crc check is passed
 */
// static uint32_t _frame_crc32_check(ttpc_frame_desc_t* pDesc, uint32_t type)
// {
//     uint32_t frame_crc32;
//     uint32_t checked_crc32;
//     c_atate_t c_state;
//     RoundSlotProperty_t* pRS;


//     uint8_t  *crc_pos;

//     crc_pos = (uint8_t*)(pDesc->pFrame) + pDesc->length - sizeof(frame_crc32);
//     pRS     = MAC_GetRoundSlotProperties();

//     _byte_copy(&frame_crc32,crc_pos,sizeof(frame_crc32));

//     checked_crc32 = _frame_crc32_calc(pDesc->pFrame,pDesc->length - sizeof(frame_crc32),type);
//     if(checked_crc32!=frame_crc32) return F ;

//     if(type==FRAME_TYPE_EXPLICIT)
//     {
    
//         /**
//          * The cstate in the x frame has the same position as the cstate in the i_cs frame. The following
//          * expression is also correct:
//          *     _byte_copy(&c_state,pDesc->pFrame->i_cs->cstate,sizeof(c_state));
//          *
//          * If the frame has an explicit c-state, the c-states between the sender and the receiver have to
//          * be explicitly compared. If these two c-states differ, then the same case distinction as with a 
//          * c-state error of an error of an implicit c-state has to be made.
//          */
//         _byte_copy(&c_state,pDesc->pFrame->x->cstate,sizeof(c_state));
//         if(!CS_IsSame(&c_state)) return F;
//     }
//     return T;
// }
/*******************************************************************************/

/**
 * This function is used to store the message address and the membership flag position
 * of the node's first successor when the check_Ia fails but the check_Ib passes.
 * 
 * If the the condition that the check_Ia fails and the check_Ib passes occurs, the data
 * transmitted by the node's first successor is marked as TANTATIVE at this time, as it
 * is unknown whether the node or the node's first successor is correct.
 */
static void _process_FT(void)
{
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    PV_SetFSAddr(pRS->CNIAddressOffset);
    PV_SetFirstSuccessorMemPos(pRS->FlagPosition);

    //transits into stage 2
    PV_SetAckState(WAIT_SECOND_SUCCESSOR);
}

/**
 * Check 1a of Acknowledge stage 1.
 * If check 1a passed, the node, sending a frame that has not be acknowledged, assumes
 * that the tranmission was correct and remains in the membership. The node shall update
 * its slot status statistics with respect to the correct frame reception and shall reset
 * the acknowledge failure statistics. In this case, the decision about the node's membership
 * in this round is final.
 */
static void _process_T(void)
{
    NodeProperty_t*      pNP = MAC_GetNodeProperties();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    CS_SetMemberBit(pNP->FlagPosition);
    CS_SetMemberBit(pRS->FlagPosition);

    //PV_IncCounter(AGREED_SLOTS_COUNTER);
    PV_ClrCounter(MEMBERSHIP_FAILED_COUNTER);

    PV_SetAckState(ACK_FINISHED);
}

/**
 * Both check_Ia and check_Ib fail, it is assumed that a transient distuebance has corrupted 
 * the node's successor's frames or B is not operational at all.
 *
 * If transmission activity was observed on both channels, the node's first successor is 
 * considered to have transmited and failed, and the failed slots counter is incremented.
 *
 * If an invalid activity was obeseved only one channel, and silence on the other channel, 
 * transient noise is assumed, and neither the agreed slots nor the failed slots counter is 
 * incremented.
 *
 * For example, if the node transmited a mode change request in its frames, since the node's 
 * first successor did not update its c-state due to the inner errors, the c-state between 
 * the node and the node's first succossor would therefore differ by more than just the membership 
 * flags.  
 */
static void _process_FF(void)
{
    NodeProperty_t*      pNP = MAC_GetNodeProperties();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    //MAC_CheckChannelActivity() ? PV_IncCounter(FAILTED_SLOTS_COUNTER) : (void)0;

    CS_SetMemberBit(pNP->FlagPosition);
    CS_ClearMemberBit(pRS->FlagPosition);
}

/**
 * check IIa passes.
 * The node assumes that its original transmission was OK and the successor was error. The node remains
 * in the membership and increases the agreed slots counter and the failed slots counter. The membership
 * failed counter is set to 0. In this case, the decision about the node's membership in this round is
 * final.
 */
static void _process_FTT(void)
{
    NodeProperty_t*      pNP = MAC_GetNodeProperties();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    uint32_t first_successor_addr = PV_GetFSAddr();
    uint32_t first_successor_pos  = PV_GetFirstSuccessorMemPos();

    CS_SetMemberBit(pNP->FlagPosition);
    CS_ClearMemberBit(first_successor_pos);
    CS_SetMemberBit(pRS->FlagPosition);

    //PV_IncCounter(AGREED_SLOTS_COUNTER);
    //PV_IncCounter(FAILTED_SLOTS_COUNTER);
    PV_ClrCounter(MEMBERSHIP_FAILED_COUNTER);

    MSG_SetStatus(first_successor_addr,FRAME_INCORRECT);

    PV_SetAckState(ACK_FINISHED);
}

/**
 * check IIa fails but check IIb passed.
 * The node assumed that the original transmission was error and the successor was correct. The node 
 * looses its membership. The membership of the node's successor is set to 1. The node increases the 
 * agreed slots counter and the failed slots counter by 1 and marks the frame received from the 
 * second successor correct if the mode change request is permitted(if the frame carries the mode 
 * change request). The membership failed counter is increased by one. If it reaches the maximum
 * membership failure count value set in the  MEDL, the controller will throw a membership error(ME)
 * and freeze. Otherwise, the controller will only raise a membership loss(ML) interruption. the 
 * decision about the node's membership in this round is final in this case.
 * 
 */
static void _process_FTFT(void)
{
    NodeProperty_t*      pNP = MAC_GetNodeProperties();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    uint32_t first_successor_addr = PV_GetFSAddr();
    uint32_t first_successor_pos  = PV_GetFirstSuccessorMemPos();

    CS_ClearMemberBit(pNP->FlagPosition);
    CS_SetMemberBit(first_successor_pos);
    CS_SetMemberBit(pRS->FlagPosition);   

    MSG_SetStatus(first_successor_addr,FRAME_CORRECT);

    //PV_IncCounter(AGREED_SLOTS_COUNTER);
    //PV_IncCounter(FALTED_SLOTS_COUNTER);
    PV_IncCounter(MEMBERSHIP_FAILED_COUNTER); 

    uint32_t max_member_fail =  MAC_GetMaximumMembershipFailureCount();

    if(max_member_fail == PV_GetCounter(MEMBERSHIP_FAILED_COUNTER))
    {
        CNI_SetSRBit(SR_ME);
        FSM_sendEvent(FSM_EVENT_ACK_ERR);
    }
    else
    {
        //membership loss, the controller shall transmit into FREEZE state.
        CNI_SetISRBit(ISR_ML);
        FSM_sendEvent(FSM_EVENT_ACK_FAILED);
    }

    PV_SetAckState(ACK_FINISHED);
}

/**
 * Both check IIa and check IIb fail.
 * The membership flag position of the second successor will be removed in this case, and the next
 * node will become the second successor.
 */
static void _process_FTFF(void)
{
    NodeProperty_t*      pNP = MAC_GetNodeProperties();
    RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    uint32_t first_successor_addr = PV_GetFSAddr();
    uint32_t first_successor_pos  = PV_GetFirstSuccessorMemPos();

    CS_SetMemberBit(pNP->FlagPosition);
    CS_ClearMemberBit(first_successor_pos);
    CS_ClearMemberBit(pRS->FlagPosition);

    //PV_ClrCounter(AGREED_SLOTS_COUNTER);

    //MAC_CheckChannelActivity() ? PV_IncCounter(FALTED_SLOTS_COUNTER) : (void)0;
}


static void (*process[7])(void) = 
{
    NULL,
    _process_FT,
    _process_T,
    _process_FF,
    _process_FTT,
    _process_FTFT,
    _process_FTFF
};

struct dnode
{
  unsigned char type;
  unsigned char process_num;
  unsigned char check[2];   /**< T or F */
};

/** decision node definition */
static const struct dnode dtree[9] = 
{
    {TYPE_DECISION,  0, { 1, 2}},    /*0*/
    {TYPE_POSITIVE,  2, {END,END}},  /*1*/
    {TYPE_DECISION,  0, { 3, 4}},    /*2*/
    {TYPE_TENTATIVE, 1, { 5, 6}},    /*3*/
    {TYPE_BACKTRACE, 3, { 0, 0}},    /*4*/
    {TYPE_POSITIVE,  4, {END,END}},  /*5*/
    {TYPE_DECISION,  0, { 7, 8}},    /*6*/
    {TYPE_NEGATIVE,  5, {END,END}},  /*7*/
    {TYPE_BACKTRACE, 6, { 3, 3}}     /*8*/
};

struct ack_db
{
    uint32_t cur_pos;
    uint32_t cur_state;
    const struct dnode *tree; 
}

static struct ack_db adb[2] = 
{
    {
        .cur_pos   = 0,
        .cur_state = ACK_INIT,
        .tree      = dtree, 
    },
    {
        .cur_pos   = 0,
        .cur_state = ACK_INIT,
        .tree      = dtree,
    },
};

/**
 * This function searches the decision-tree of the acknowledge algorithm.
 * The function updates the index of the node of the decision-tree of the 
 * corresponding channel according to the branch.
 * 
 * If the searching process reaches the terminal node of the decision-tree, the 
 * node marked as TYPE_ULTIMATE, it will reset the index of the search process 
 * to the starting.
 * @param  branch the decision, T or F.
 * @param  adb_ch ack_db, 0 or 1 depending on the channel.
 * @param  pFunc  the pointer to the corresponding decision processor. If the ack
 *                is not finished, it will point to NULL.
 * @return        
 *                @arg 0 ,ack not finished, waiting for the next successor.
 *                @arg 1 ,ack finished, negtive.
 *                @arg 2 ,ack not finished, tentative.
 *                @arg 3 ,ack finished, positive.
 *                @arg 4 ,ack not finished, waiting for the next check.
 */
static uint32_t _dtree_search(uint32_t branch, uint32_t adb_ch, AckFunc *pFunc)
{
    struct ack_db* p = &adb[adb_ch];
    const struct dnode * pn;
    uint32_t res = 4;

    p->cur_pos = p->tree[p->cur_pos].check[branch];
    pn = &p->tree[p->cur_pos];

    switch(pn->type)
    {
        case TYPE_DECISION:
            res = 4;
            break;
        case TYPE_TENTATIVE:
            res = 2;
            p->cur_state = WAIT_SECOND_SUCCESSOR;
            break;
        case TYPE_BACKTRACE:
            res = 0;
            p->cur_pos = pn->check[0]; /**< for backtracing */
            break;
        case TYPE_POSITIVE:
            res = 3;
            p->cur_state = ACK_FINISHED;
            break;
        case TYPE_NEGATIVE:
            res = 1;
            p->cur_state = ACK_FINISHED;
            break;
        default:break;
    }

    *pFunc = process[pn->process_num];

    return res;
}

void SVC_AckInit(void)
{
    adb[0].cur_pos   = 0;
    adb[0].cur_state = WAIT_FIRST_SUCCESSOR;
    adb[0].tree      = dtree;

    adb[1] = adb[0];
}

void SVC_AckMerge(uint32_t ch)
{
    TTP_ASSERT( (ch==0)||(ch==1) );
    adb[1-ch] = adb[ch];
}

uint32_t SVC_Acknowledgment(uint32_t check_a, uint32_t check_b, uint32_t ch, AckFunc *pFunc )
{
    uint32_t res;

    res = _dtree_search(check_a,ch,pFunc);
    if(res==4)
        res = _dtree_search(check_b,ch,pFunc);

    return res; 
}

// uint32_t SVC_Acknowledgment(uint32_t check_a, uint32_t check_b, uint32_t ch, AckFunc *pFunc )
// {
//     RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
//     NodeProperty_t*      pNP = MAC_GetNodeProperties();

//     uint32_t search_res;
//     uint32_t ack_state;

//     ack_state = PV_GetAckState();

//     if(pRS->FrameType==FRAME_TYPE_EXPLICIT)
//     {
//         /**
//          * The reception of a correct frame frrom a sending node requires that the receiver sets
//          * the membership flag of the sender to TRUE before checking the frame CRC.
//          */
//         CS_SetMemberBit(pRS->FlagPosition);

//         crc_res = _frame_crc32_check(pDesc,FRAME_TYPE_EXPLICIT);
//         search_res = _dtree_search(crc_res,ch,pFunc);

//         if(search_res == 4) // search not finished
//             return _dtree_search(crc_res,ch,pFunc);
//     }
//     else // FRAME_TYPE_IMPLICIT
//     {
//         //check ia or iia
//         CS_SetMemberBit(pNP->FlagPosition); 
//         ack_state == WAIT_FIRST_SUCCESSOR ? CS_SetMemberBit(pRS->FlagPosition) : CS_ClearMemberBit(pRS->FlagPosition);
        
//         crc_res = _frame_crc32_check(pDesc,FRAME_TYPE_IMPLICIT);
//         search_res = _dtree_search(crc_res,ch,pFunc);

//         if(search_res == 4)
//         {
//             //check ib or iib
//             CS_ClearMemberBit(pNP->FlagPosition); 
//             CS_SetMemberBit(pRS->FlagPosition);

//             crc_res = _frame_crc32_check(pDesc,FRAME_TYPE_IMPLICIT);
//             return _dtree_search(crc_res,ch,pFunc);
//         }
//     }

//     return search_res;
// }
