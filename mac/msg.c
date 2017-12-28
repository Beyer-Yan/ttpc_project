/**
  ******************************************************************************
  * @file    	msg.c
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.11
  * @brief   	the implementation of the message interface
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
#include "msg.h"
#include "ttpmac.h"
#include "medl.h"
#include "xfer.h"
#include "ttpservice.h"
#include "protocol_data.h"
#include "crc.h"
#include "ttpdebug.h"
#include "clock.h"

/************* global variables definitions ***********************************/

static TTP_ChannelFrameDesc _ch0 = 
{
    .rcv_timestamp = 0,
    .length        = 0,
    .pFrame        = NULL
};
static TTP_ChannelFrameDesc _ch1 = 
{
    .rcv_timestamp = 0,
    .length        = 0,
    .pFrame        = NULL   
};

static TTP_FrameDesc _G_TTP_FrameDesc =
{
    .pCH0 = &_ch0,
    .pCH1 = &_ch1
};

/*************  functions definitions *******************************************/

static inline uint32_t __calc_frame_type(void)
{
    RoundSlotProperty_t* pSlot;

    pSlot = MAC_GetRoundSlotProperties();

    if(pSlot->FrameType==FRAME_TYPE_IMPLICIT)
    {
        return FRAME_N;
    }
    else
    {
        if(!pSlot->AppDataLength)
        {
            return FRAME_I;
        }
        else
        {
            return FRAME_X;
        }
    }
}


static uint32_t __assemble_ttp_frame(void)
{
    uint8_t  header;
    uint32_t mcr;
    uint32_t frame_type;

    RoundSlotProperty_t* pSlot;
    c_state_t c_state;

    //error detecting
    frame_type    = __calc_frame_type();
    pSlot         = MAC_GetRoundSlotProperties();
    mcr           = CNI_GetCurMCR();

    //CNI_ClrMCR();
    CRC_ResetData();
    uint32_t id = MEDL_GetSchedID();
    CRC_PushData((uint8_t*)&id,CSID_SIZE);

    // /**
    //  * @brief If no mode change was requested the mode change field 
    //  * shall be set to the value of "MCR_MODE_CLR" according to 
    //  * AS6003 8.6.1, page 38/56
    //  */ 
    
    header = ((mcr)>>2)|(pSlot->FrameType&1);

    CRC_PushData(&header,1);   
    DRV_PushData(&header,1);

    //assemble frame body.
    CS_GetCState(&c_state);

    if(frame_type==FRAME_N)
    {
        /**
         * @see 5.3.1, AS6003, page 16/56
         */
        TTP_ASSERT(pSlot->AppDataLength<=MAX_FRAME_LENGTH-TTP_HEADER_LENGTH-TTP_CRC_LENGTH);

        DRV_PushData(MSG_GetMsgAddr(pSlot->CNIAddressOffset),pSlot->AppDataLength);

        CRC_PushData(MSG_GetMsgAddr(pSlot->CNIAddressOffset),pSlot->AppDataLength);
        CRC_PushData((uint8_t*)&c_state,sizeof(c_state_t));
        
        uint32_t crc32_res = CRC_GetResult();
        DRV_PushData((uint8_t*)&crc32_res,4);
    }
    else // either FRAME_I or FRAME_X
    {
        /**
         * @see 5.3.2, AS6003, page 16/56
         */
        //frame_i assemble
        DRV_PushData((uint8_t*)&c_state,sizeof(c_state_t));
        CRC_PushData((uint8_t*)&c_state,sizeof(c_state_t));

        uint32_t crc32_res = CRC_GetResult();
        DRV_PushData((uint8_t*)&crc32_res,4);

        /**
         * attention that for 24-bits crc check mode, the 8-padding should be tailed
         * to the standard crc24.
         */
        if(frame_type==FRAME_X)
        {
            /**
             * @see 5.3.4, AS6003, page 18/56
             */
            TTP_ASSERT(pSlot->AppDataLength<=MAX_FRAME_LENGTH-TTP_HEADER_LENGTH-2*TTP_CRC_LENGTH-CSTATE_LENGTH);

            CRC_PushData((uint8_t*)&crc32_res,4);
            CRC_PushData(MSG_GetMsgAddr(pSlot->CNIAddressOffset),pSlot->AppDataLength);

            DRV_PushData(MSG_GetMsgAddr(pSlot->CNIAddressOffset),pSlot->AppDataLength);
            crc32_res = CRC_GetResult();
            DRV_PushData((uint8_t*)&crc32_res,4);
        }    
    }
    return MAC_EOK;     
}

/**
 * This function assembles cold start frame.
 * @return  [description]
 */
MAC_err_t MSG_PrepareCSFrame(void)
{
    uint8_t header;
    uint32_t crc32;

    c_state_t c_state;
    ScheduleParameter_t* pSch;
    NodeProperty_t*      pNode;

    header = (MCR_NO_REQ>>2)&FRAME_TYPE_EXPLICIT;

    pSch = MAC_GetScheduleParameter();
    pNode= MAC_GetNodeProperties();
    CS_GetCState(&c_state);

    /** check if the node has the qualification to cold-start */
    TTP_ASSERT(pSch->ColdStartAllow==COLD_START_ALLOWED);
    TTP_ASSERT(pNode->MultiplexedMembershipFlag==MONOPOLIZED_MEMBERSHIP);
    TTP_ASSERT(pNode->PassiveFlag==NOT_PASSIVE);

    CRC_ResetData();
    uint32_t id = MEDL_GetSchedID();

    CRC_PushData((uint8_t*)&id,CSID_SIZE);
    CRC_PushData(&header,1); 
    CRC_PushData((uint8_t*)&c_state,sizeof(c_state));
    crc32 = CRC_GetResult();

    DRV_PushData(&header,1);
    DRV_PushData((uint8_t*)&c_state,sizeof(c_state));
    DRV_PushData((uint8_t*)&crc32,4);

    return MAC_EOK;
}

/**
 * For safety critical transmission process, the contents of both CH0
 * and CH1 are definitely same. We choose not to implement the feature
 * that different channels can send different frames. Maybe that will
 * be taken consideration for the next version.
 * @return  MAC_err_t
 */
MAC_err_t MSG_PushFrame(void)
{
    __assemble_ttp_frame();
}

void MSG_PullAppData(TTP_ChannelFrameDesc* pDesc)
{
    RoundSlotProperty_t* pSlot;
    uint32_t type;
    uint32_t msg_size;
    uint8_t *msg_addr;
    uint8_t *payload_addr;

    if(pDesc==NULL)
        return;

    pSlot = MAC_GetRoundSlotProperties();
    type = __calc_frame_type();
    msg_size = pSlot->AppDataLength;
    msg_addr = MSG_GetMsgAddr(pSlot->CNIAddressOffset);

    TTP_ASSERT(type==FRAME_N || type==FRAME_X);

    if(type==FRAME_N)   
        payload_addr = pDesc->pFrame->n.payload;
    else
        payload_addr = pDesc->pFrame->x.payload;
    
    int i= 0;
    for(;i<msg_size;i++)
        msg_addr[i] = payload_addr[i];
}

static inline uint32_t _status_adapter(uint32_t drv_status)
{
    uint32_t res;
    switch(drv_status)
    {
        case DRV_OK  : res = MAC_EOK;       break;
        case RXD_COL : res = MAC_ERX_COL;   break;
        case CRC_ERR : res = MAC_ERX_ECRC;  break;
        case PHY_ERR : res = MAC_EPHY;      break;
        case LTH_ERR : res = MAC_ERX_LTH;   break;
        case DRV_INV : res = MAC_ERX_INV;   break;
        default : res = MAC_EOTHER;    break;
    }
    return res;
}

TTP_FrameDesc* MSG_GetFrameDesc(void)
{
    DataPacketTypeDef* pDataPacket;

    pDataPacket = DRV_PullData();
    if(pDataPacket==NULL)
        return NULL;

    _G_TTP_FrameDesc.pCH0->length = pDataPacket->ch0->length;
    _G_TTP_FrameDesc.pCH0->pFrame = (TTP_FrameStructDesc*)pDataPacket->ch0->BufferAddr;
    _G_TTP_FrameDesc.pCH0->rcv_timestamp = CLOCK_GetCaptureRX0();
    _G_TTP_FrameDesc.pCH0->status = _status_adapter(pDataPacket->ch0->status);

    _G_TTP_FrameDesc.pCH1->length = pDataPacket->ch1->length;
    _G_TTP_FrameDesc.pCH1->pFrame = (TTP_FrameStructDesc*)pDataPacket->ch1->BufferAddr;
    _G_TTP_FrameDesc.pCH1->rcv_timestamp = CLOCK_GetCaptureRX1();
    _G_TTP_FrameDesc.pCH1->status = _status_adapter(pDataPacket->ch1->status);

    return &_G_TTP_FrameDesc;               
}

uint32_t  MSG_GetFrameTimestamp(uint32_t channel)
{
    TTP_ASSERT(channel==CH0 || channel==CH1);

    return channel==CH0?CLOCK_GetCaptureRX0():CLOCK_GetCaptureRX1();
}

/**
 * This function eliminates the error code mismatch between hardware
 * level and mac level.
 * @return  the error code of mac level
 */
uint32_t MSG_GetTransmittedFlags(void)
{
    uint32_t res = DRV_CheckTransmitted();

    switch(res)
    {
        case DRV_OK : res = MAC_EOK;      break;
        case PHY_ERR: res = MAC_EPHY;     break;
        default: res = MAC_EOTHER;        break;
    }
    return res;
}

uint32_t  MSG_CheckReceived(uint32_t Channel)
{
    return DRV_CheckReceived(Channel);
}

