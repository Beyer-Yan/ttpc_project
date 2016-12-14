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
#include "ttpc_mac.h"
#include "medl.h"
#include "virhw.h"
#include "ttpservice.h"

static uint8_t recv_byte_stream[2][256] = {0};
static uint8_t send_byte_stream[2][256] = {0};

#define TTP_FRAME_HEADER_OFFSET     14

/**
 * The mac_addr_t should be platform-dependent. We implement the TTPC protocol
 * based on 802.3 mac specification. So the MAC address is set in 6-bytes size.
 */
typedef uint8_t hwaddr[6];

static hwaddr src = {
    0x02,0x00,0x00
};

static hwaddr dst = {
    0xff,0xff,0xff,0xff,0xff,0xff
};

/**
 * record the th length of ttp frame assembled.
 */
static short __G_ttp_frame_length = 0;


////////////////////////////////////////////////////////////////////////////////
///send frame operation                                                       //
////////////////////////////////////////////////////////////////////////////////

static void __byte_copy(uint8_t* dst, uint8_t* src, int size)
{
    while(size--)
    {
        *dst++ = *src++;
    }
}

static void __byte_copy_to_frame_buf(int index,uint8_t *src,int size)
{
    __byte_copy(send_byte_stream[CH0]+index,src,size);
    __byte_copy(send_byte_stream[CH1]+index,src,size);
}

/**
 * This function calculates the crc32 of the given buf.
 * 4-bytes-alignment is not needed for the buf address because of the inner
 * alignment logic, however, leading to the poor performance. 
 * @param  buf    the buf to be crc checked 
 * @param  length the length of the buf in unit of byte
 * @return        the 32-bit crc check code
 * @attention     4-bytes-alignment is required for the low level hardware.
 *                But the address of the buf is, maybe, not 4-byte-alignment,
 *                mostly. A 4-byte temporary variable is introduced in case
 *                it, despite the poor performance.
 */
static uint32_t __ttp_frame_crc32_calc(uint8_t* buf,int length)
{
    #error "to be completed"
}

/**
 * This function resets the CRC32 computation unit
 */
static void __ttp_frame_crc32_reset()
{
    /**
     * @see void CRC_ResetData(void) in file virhw.h, line 233
     */
    CRC_ResetData();
}

/**
 * This function/macro calculates the frame type to be assembled.
 * @attention round slot properties should be set before this function
 * is called.   
 * @return the frame type to be assembled.
 */

#define __calc_frame_type()    \
        (MAC_GetRoundSlotProperties()->FrameType==FRAME_TYPE_IMPLICIT?\
        FRAME_N:(MAC_GetRoundSlotProperties()->AppDataLength?FRAME_X:\
        FRAME_I))

static uint32_t (__calc_frame_type)(void)
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

// static void __assemble_frame_body_n()
// {

// }

// static void __assemble_frame_body_i()
// {

// }

// static void __asemble_frame_body_x()
// {

// }

static uint32_t 
__check_frame_legality(uint32_t frame_type, \
                       uint32_t mcr\
                       uint8_t* cni_base, \
                       RoundSlotProperty_t* pSlot)
{
    if((frame_type==FRAME_N)||(frame_type==FRAME_X))
    {
        if(((uint8_t*)cni_base + pSlot->CNIAddressOffset)[0]!=STATUS_BIT_PATTERN)
        {
            return MAC_ERS;
        }
        if(pSlot->AppDataLength==0)
        {
            return MAC_ENON_DATA;
        }
        if(pSlot->AppDataLength>MAX_FRAME_LENGTH)
        {
            return MAC_ESIZE_OVER;
        }
    }
    if(mcr>=MCR_MODE_CLR)
    {
        /**
         * @attention ensure the continuous numeric definition of MCR field,
         * meaning that DO NOT CHANGE THE MCR DEFINITION in ttpdef.h. IF YOU
         * HAVE TO CHANGE IT, REMEMBLE TO CHANGE THE IMPLEMENTATION. 
         */
        return MAC_EMODE;
    }

    if(pSlot->ModeChangePermission==MODE_CHANGE_DENY)
    {
        return MAC_EMODE;
    }
    return MAC_EOK;
}


static uint32_t __assemble_ttp_frame(void)
{
    uint8_t  header;
    uint32_t crc32;
    uint32_t mcr;
    uint32_t frame_type;
    uint8_t* cni_base_addr;
    uint8_t  check_res;

    //to record the filling index of the two send_byte_stream.
    int _index;

    RoundSlotProperty_t* pSlot;
    c_state_t c_state;

    //error detecting
    frame_type    = __calc_frame_type();
    _index        = TTP_FRAME_HEADER_OFFSET;
    cni_base_addr = CNI_GetCNIBaseAddr();
    pSlot         = MAC_GetRoundSlotProperties();
    mcr           = CNI_GetCurMCR();

    check_res = __check_frame_legality(frame_type,mcr,cni_base_addr,pSlot);
    if(check_res!=MAC_EOK)
    {
        __G_ttp_frame_length = 0;
        return check_res;
    }

    /**
     * @brief If no mode change was requested the mode change field 
     * shall be set to the value of "MCR_MODE_CLR" according to 
     * AS6003 8.6.1, page 38/56
     */ 
    
    header = ((mcr==MCR_NO_REQ?MCR_MODE_CLR:mcr)>>2)|(pSlot->FrameType&1);

    __byte_copy_to_frame_buf(_index,&header,sizeof(header));

    _index += sizeof(header);

    //assemble frame body.
    MAC_CSGetCState(&c_state);

    if(frame_type==FRAME_N)
    {
        /**
         * @see 5.3.1, AS6003, page 16/56
         */
        /** skip the 1 byte status field for application data */
        __byte_copy_to_frame_buf(_index,
                                 cni_base_addr + pSlot->CNIAddressOffset + 1,
                                 pSlot->AppDataLength);
        _index += pSlot->appDataLength;

        //padding the crc region
        __ttp_frame_crc32_reset();
        crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID());
        crc32 = __ttp_frame_crc32_calc(send_byte_stream[CH0]+TTP_FRAME_HEADER_OFFSET,
                                       _index-TTP_FRAME_HEADER_OFFSET);
        crc32 =__ttp_frame_crc32_calc(&c_state,sizeof(c_state_t));

        __byte_copy_to_frame_buf(_index,&crc32,4);
        _index += 4;   

    }
    else // either FRAME_I or FRAME_X
    {
        /**
        * @see 5.3.2, AS6003, page 16/56
        */
       //frame_i assemble
       __byte_copy_to_frame_buf(_index,&c_state,sizeof(c_state_t));
       _index += sizeof(c_state_t);

       __ttp_frame_crc32_reset();
       crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID());
       crc32 =  __ttp_frame_crc32_calc(send_byte_stream[CH0]+TTP_FRAME_HEADER_OFFSET,
                                       _index-TTP_FRAME_HEADER_OFFSET);
       /**
        * attention that for 24-bits crc check mode, the 8-padding should be tailed
        * to the standard crc24.
        */
       __byte_copy_to_frame_buf(_index,&crc32,4);

       _index += 4;

       if(frame_type==FRAME_X)
       {
            /**
             * @see 5.3.4, AS6003, page 18/56
             */
            __byte_copy_to_frame_buf(_index,
                                     cni_base_addr + pSlot->CNIAddressOffset + 1,
                                     pSlot->AppDataLength);
            _index += pSlot->appDataLength;

            __ttp_frame_crc32_reset();
            crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID());
            crc32 = __ttp_frame_crc32_calc(send_byte_stream[CH0]+TTP_FRAME_HEADER_OFFSET,
                                           _index-TTP_FRAME_HEADER_OFFSET);
            __byte_copy_to_frame_buf(_index,&crc32,4);
            _index += 4;
       }    
    }
    __G_ttp_frame_length = _index-TTP_FRAME_HEADER_OFFSET;
    return MAC_EOK;     
}

/**
 * This function assembles a 802.3 frame. The global __G_ttp_frame_length 
 * variable is regard as the length of client data.
 * @return             the total size of the 802.3 frame excluding
 *                     the 7-bytes PLS and 1-byte SFD.
 */
static uint32_t __assemble_eth_frame(void)
{
    int _index = 0;
    __byte_copy_to_frame_buf(_index,dst,6);
    _index += 6;
    __byte_copy_to_frame_buf(_index,src,6);
    _index += 6;
    __byte_copy_to_frame_buf(_index,&client_size,2);

    _index += 2; //not necessary
    /**< 6 bytes dst, 6 bytes src, 2 bytes type/length */
    /** TTP_FRAME+HEADER_OFFSET */
    return client_size + 6 + 6 + 2; 
}
/**
 * This function assembles cold start frame.
 * @return  [description]
 */
MAC_err_t MAC_PrepareSCFrame(void)
{
    uint8_t header;
    uint32_t crc32;
    int _index = TTP_FRAME_HEADER_OFFSET;
    uint32_t tsf;
    c_state_t c_state;
    ScheduleParameter_t* pSch;
    NodeProperty_t*      pNode;

    header = (MCR_NO_REQ>>2)&FRAME_TYPE_EXPLICIT;

    pSch = MAC_GetScheduleParameter();
    pNode= MAC_GetNodeProperty();

    /** check if the node has the qualification to cold-start */
    TTP_ASSERT(pSch->ColdStartAllow==COLD_START_ALLOWED);
    TTP_ASSERT(pNode->MultiplexedMembershipFlag==MONOPOLIZED_MEMBERSHIP);
    TTP_ASSERT(pNode->PassiveFlag==NOT_PASSIVE);

    tsf = CNI_GetTSF();

    /** set c-state field, located in CNI */
    MAC_CSSetGTF(tsf);
    MAC_CSSetRoundSlot(pNode->LogicalNameSlotPosition);
    MAC_CSSetMode(MODE_CS_ID);
    MAC_CSSetDMC(DMC_NO_REQ);
    MAC_CSClearMemberAll();
    MAC_CSSetMemberBit(pNode->FlagPosition);

    /**
    * copy c-state of the CNI to the local c-state variable for
    * frame assembling because of the bit-mismatch. 
    * @see  description of file ttpc_mac.h, line 147 to line 153
    */
    MAC_CSGetCState(&c_state);

    __byte_copy_to_frame_buf(_index,&header,sizeof(header));
    _index += 1;

    __byte_copy_to_frame_buf(_index,&c_state,sizeof(c_state));
    _index += sizeof(c_state);

    __ttp_frame_crc32_reset();
    crc32 = __ttp_frame_crc32_calc(MEDL_GetSchedID());
    crc32 = __ttp_frame_crc32_calc(send_byte_stream[CH0]+TTP_FRAME_HEADER_OFFSET,
                                   _index-TTP_FRAME_HEADER_OFFSET);
    __byte_copy_to_frame_buf(_index,&crc32,4);
    _index += 4;
    __G_ttp_frame_length = _index - TTP_FRAME_HEADER_OFFSET;

    __assemble_eth_frame(_index-TTP_FRAME_HEADER_OFFSET);

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

    res = __assemble_ttp_frame();
    /**prepare to transmit */
    if(res==MAC_EOK)
    {
        DRV_PrepareToTransmitOfCH0(send_byte_stream[CH0],
                               __G_ttp_frame_length+TTP_FRAME_HEADER_OFFSET);
        DRV_PrepareToTransmitOfCH1(send_byte_stream[CH1],
                               __G_ttp_frame_length+TTP_FRAME_HEADER_OFFSET);
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
        case TXD_COL: res = MAC_ETX_COL;  break;
        case PHY_ERR: res = MAC_EPHY;     break;
        case default: res = MAC_EOTHER;   break;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////
///receive frame operation                                                    //
////////////////////////////////////////////////////////////////////////////////
/** check whether the mac has received frame with returning 0 for receiving nothing, 
 *  returning 1 for receiving frames.
 *  receiving nothing.
 */
uint32_t  MAC_CheckReceived(void)
{
    return DRV_CheckReceived();
}

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
        case default : res = MAC_EOTHER;    break;
    }
    return res;
}

