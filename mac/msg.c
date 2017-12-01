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
#include "medl.h"
#include "xfer.h"
#include "ttpservice.h"
#include "protocol_data.h"

#define FRAME_BUFFER_SIZE         256
//#define ETH_FRAME_HEADER_SIZE   14
#define TTP_FRAME_HEADER_SIZE     1

/** 
 * 4-byte alignment for buffers below .
 * Every channel buffer of buffers below includes the
 * ethernet dst,src and type/len field 
 */
//static volatile uint8_t recv_byte_stream[2][FRAME_BUFFER_SIZE] = {{0},{0}};
//static volatile uint8_t send_byte_stream[2][FRAME_BUFFER_SIZE] = {{0},{0}};

/**
 * The mac_addr_t should be platform-dependent. We implement the TTPC protocol
 * based on 802.3 mac specification. So the MAC address is set in 6-bytes size.
 */
/*
typedef uint8_t hwaddr[6];

static hwaddr src = {
    0x02,0x00,0x00
};

static hwaddr dst = {
    0xff,0xff,0xff,0xff,0xff,0xff
};
*/
/**
 * record the th length of ttp frame assembled.
 */
//static volatile uint16_t __G_ttp_frame_length = 0;


////////////////////////////////////////////////////////////////////////////////
///send frame operation                                                       //
////////////////////////////////////////////////////////////////////////////////
/*
static inline void __byte_copy_to_frame_buf(uint8_t* dst, uint8_t* src, int size)
{
    uint8_t *_dst_ch0 = dst;
    uint8_t *_dst_ch1 = dst + FRAME_BUFFER_SIZE;

    while(size--)
    {
        *_dst_ch0++ = *src;
        *_dst_ch1++ = *src++;
    }
}
*/
// static inline void __byte_copy_to_frame_buf(uint8_t *dst,uint8_t *src,int size)
// {
//     __byte_copy(dst,src,size);
//     __byte_copy(dst+256,src,size);
// }

/**
 * This function calculates the frame type to be assembled.
 * @attention round slot properties should be set before this function
 * is called.   
 * @return the frame type to be assembled.
 */

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
    uint32_t crc32;
    uint32_t mcr;
    uint32_t frame_type;
    uint8_t  check_res;

    RoundSlotProperty_t* pSlot;
    c_state_t c_state;

    pTTP_frame *pFrame;

    //error detecting
    frame_type    = __calc_frame_type();
    pSlot         = MAC_GetRoundSlotProperties();
    mcr           = CNI_GetCurMCR();

    //CNI_ClrMCR();
    
    // /**
    //  * @brief If no mode change was requested the mode change field 
    //  * shall be set to the value of "MCR_MODE_CLR" according to 
    //  * AS6003 8.6.1, page 38/56
    //  */ 
    
    header = ((mcr)>>2)|(pSlot->FrameType&1);

    /** posit the ttp_frame in the send_buffer */
    pFrame = (pTTP_frame)(&send_byte_stream[CH0][0] + ETH_FRAME_HEADER_SIZE);

    __byte_copy_to_frame_buf(pFrame->hdr,&header,sizeof(header));

    //assemble frame body.
    MAC_GetCState(&c_state);

    if(frame_type==FRAME_N)
    {
        /**
         * @see 5.3.1, AS6003, page 16/56
         */
        __byte_copy_to_frame_buf(pFrame->n.paylaod,
                                 MSG_GetMsgAddr(pSlot->CNIAddressOffset),
                                 pSlot->AppDataLength);
        //padding the crc region
        __ttp_frame_crc32_reset();
        crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID(),CSID_SIZE);
        crc32 = __ttp_frame_crc32_calc((uint8_t*)pFrame, pSlot->AppDataLength+TTP_FRAME_HEADER_SIZE);
        crc32 = __ttp_frame_crc32_calc(&c_state,sizeof(c_state_t));

        __byte_copy_to_frame_buf(pFrame->n.paylaod+pSlot->AppDataLength,&crc32,4);

        __G_ttp_frame_length = TTP_FRAME_HEADER_SIZE + pSlot->AppDataLength + 4;

    }
    else // either FRAME_I or FRAME_X
    {
        /**
        * @see 5.3.2, AS6003, page 16/56
        */
       //frame_i assemble
       __byte_copy_to_frame_buf(pFrame->i_cs.cstate,&c_state,sizeof(c_state_t));

       __ttp_frame_crc32_reset();
       crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID(),CSID_SIZE);
       crc32 = __ttp_frame_crc32_calc((uint8_t*)pFrame,sizeof(c_state_t)+TTP_FRAME_HEADER_SIZE);
       /**
        * attention that for 24-bits crc check mode, the 8-padding should be tailed
        * to the standard crc24.
        */
       __byte_copy_to_frame_buf(pFrame->crc32,&crc32,4);
       __G_ttp_frame_length = TTP_FRAME_HEADER_SIZE + sizeof(c_state_t) + 4;

       if(frame_type==FRAME_X)
       {
            /**
             * @see 5.3.4, AS6003, page 18/56
             */
            __byte_copy_to_frame_buf(pFrame->x.payload,\
                                     MSG_GetMsgAddr(pSlot->CNIAddressOffset),
                                     pSlot->AppDataLength);

            __ttp_frame_crc32_reset();
            crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID(),CSID_SIZE);
            crc32 = __ttp_frame_crc32_calc((uint8_t*)pFrame,
                                           pSlot->AppDataLength+TTP_FRAME_HEADER_SIZE+sizeof(c_state_t)+4);
            __byte_copy_to_frame_buf((uint8_t*)&pFrame->x.paylaod+pSlot->AppDataLength,&crc32,4);
            
            __G_ttp_frame_length += pSlot->AppDataLength;
       }    
    }
    return MAC_EOK;     
}

/**
 * This function assembles cold start frame.
 * @return  [description]
 */
MAC_err_t MAC_PrepareSCFrame(void)
{
    uint8_t header;
    uint32_t crc32;
    uint32_t tsf;
    uint16_t total_size; 

    c_state_t c_state;
    ScheduleParameter_t* pSch;
    NodeProperty_t*      pNode;
    pTTP_frame           pFrame;

    header = (MCR_NO_REQ>>2)&FRAME_TYPE_EXPLICIT;

    pSch = MAC_GetScheduleParameter();
    pNode= MAC_GetNodeProperty();
    pFrame = (pTTP_frame)(&send_byte_stream[CH0][0] + ETH_FRAME_HEADER_SIZE);

    /** check if the node has the qualification to cold-start */
    TTP_ASSERT(pSch->ColdStartAllow==COLD_START_ALLOWED);
    TTP_ASSERT(pNode->MultiplexedMembershipFlag==MONOPOLIZED_MEMBERSHIP);
    TTP_ASSERT(pNode->PassiveFlag==NOT_PASSIVE);

    // tsf = CNI_GetTSF();

    // /** set c-state field, located in CNI */
    // CS_SetGTF(tsf);
    // CS_SetRoundSlot(pNode->LogicalNameSlotPosition);
    // CS_SetMode(MODE_CS_ID);
    // CS_SetDMC(DMC_NO_REQ);
    // CS_ClearMemberAll();
    // CS_SetMemberBit(pNode->FlagPosition);

    // /**
    // * copy c-state of the CNI to the local c-state variable for
    // * frame assembling because of the bit-mismatch. 
    // * @see  description of file ttpc_mac.h, line 191 to line 197
    // */
    // MAC_GetCState(&c_state);

    __byte_copy_to_frame_buf(pFrame->hdr,&header,sizeof(header));

    __byte_copy_to_frame_buf(pFrame->cstate,&c_state,sizeof(c_state));

    __ttp_frame_crc32_reset();
    crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID(),CSID);
    crc32 = __ttp_frame_crc32_calc((uint8_t*)pFrame,sizeof(c_state_t)+TTP_FRAME_HEADER_SIZE);
    __byte_copy_to_frame_buf(pFrame->crc32,&crc32,4);

    __G_ttp_frame_length = TTP_FRAME_HEADER_SIZE + sizeof(c_state_t) + 4;

    total_size = __assemble_eth_frame();

    DRV_PrepareToTransmitOfCH0(send_byte_stream[CH0],total_size);
    DRV_PrepareToTransmitOfCH1(send_byte_stream[CH1],total_size);

    return MAC_EOK;
}

/**
 * For safety critical transmission process, the contents of both CH0
 * and CH1 are definitely same. We choose not to implement the feature
 * that different channels can send different frames. Maybe that will
 * be taken consideration for the next version.
 * @return  MAC_err_t
 */
MAC_err_t MAC_PushFrame(void)
{
    uint32_t res;
    uint16_t total_size;

    res = __assemble_ttp_frame();
    /**prepare to transmit */
    if(res==MAC_EOK)
    {
        total_size = __assemble_eth_frame();

        DRV_PrepareToTransmitOfCH0(send_byte_stream[CH0],total_size);
        DRV_PrepareToTransmitOfCH1(send_byte_stream[CH1],total_size);
    }
    return res;
}

/**
 * This function eliminates the error code mismatch between hardware
 * level and mac level.
 * @return  the error code of mac level
 */
uint32_t MAC_GetTransmittedFlags(void)
{
    uint32_t res = DRV_CheckTransmitted();

    switch(res)
    {
        case DRV_OK : res = MAC_EOK;      break;
        case PHY_ERR: res = MAC_EPHY;     break;
        default: res = MAC_EOTHER;   break;
    }
    return res;
}

// ////////////////////////////////////////////////////////////////////////////////
// ///receive frame operation                                                    //
// ////////////////////////////////////////////////////////////////////////////////
// /** check whether the mac has received frame with returning 0 for receiving nothing, 
//  *  returning 1 for receiving frames.
//  *  receiving nothing.
//  */
// uint32_t  MAC_CheckReceived(void)
// {
//     return DRV_CheckReceived();
// }

/**
 * This function returns the flags of frames received.  
 * @return  the flags of frames received, values below:
 *           @arg MAC_ERX_NON    nothing has been received
 *           @arg MAC_ERX_INV    invalid frames has been received
 *           @arg MAC_ERX_COL    collision detected when receiving
 *           @arg MAC_ERX_ECRC   received a frame with crc falied
 *           @arg MAC_ERX_ELT    reveived a frame with length error
 *           @arg MAC_EOK        reveived a good frame.
 */
uint32_t  MAC_GetReceivedFlag(void)
{
    DataStreamTypeDef stream;
    uint32_t res;

    res = DRV_CheckReceived();
    stream = DRV_GetReceived();

    if(!res)
    {
        return MAC_ERX_NON;
    }

    switch(stream.status)
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

