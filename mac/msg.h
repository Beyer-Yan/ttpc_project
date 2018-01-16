/**
  ******************************************************************************
  * @file    	ttpservice.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.11
  * @brief   	brief description about this file
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file aims to provide the basic operation interface for TTP frame assembling
  *
  ******************************************************************************
  */
#ifndef __MSG_H__
#define __MSG_H__

#include "ttpdef.h"
//#include "ttpmac.h"


#define MAX_FRAME_LENGTH		240
#define CSTATE_LENGTH           16
#define TTP_HEADER_LENGTH       1
#define TTP_CRC_LENGTH          4

/** frame type macro definitions */
#define FRAME_N             	0
#define FRAME_I             	1
#define FRAME_X             	2

/** for frame status field: the least significant 8-bits valid*/
#define FRAME_CORRECT 			(uint32_t)0x00000000
#define FRAME_TENTATIVE			(uint32_t)0x00000001
#define FRAME_MODE_VIOLATION	(uint32_t)0x00000002
#define FRAME_INCORRECT 		(uint32_t)0x00000003
#define FRAME_NULL				(uint32_t)0x00000004
#define FRAME_INVALID			(uint32_t)0x00000005

typedef uint32_t crc32_t;
typedef crc32_t  crc_t;
/**
 * TTPC messages are assigned to specified slots statically. The particular 
 * message received will be put in the slot-relative message area according
 * to its slot.
 * The maximum value of a TTPC message in unit of byte. see AS6003, page 53
 * 
 * We set the size of message area forcedly and fixedly for every round slot. The
 * message to be received on CH0 and CH1 will be pulled in a dual-cell receiving 
 * buffer respectively. And the message to be transmitted redundantly on CH0
 * and CH1 is pushed from the sending single-cell buffer.
 * The status field is a one-byte memory cell, indicating the receiving status when 
 * the TTPC controller has received a message. Moreover, when the TTPC controller 
 * sends a message, the host must write a specific bit-pattern to the status field 
 * ,then the TTPC controller reads it and clears it. If the specific bit-pattern is 
 * not written by the host, the TTPC controller will raise a NR(Frame Not Ready)
 * interruption then freeze.
 *                                                                             
 /                          /                        /                   / 
 |-------slot 0-------------|------- slot 1--------- |----- slot 3-------| 
 |                          |                        |                   | 
 |                          |                        |                   | 
 /-/----------/-/-----------/-/--------/-/-----------/-/-----------------| 
 |*|          |*|           |*|        |*|           |*|                 | 
 |*|          |*|           |*|        |*|           |*|                 | 
 |*|          |*|           |*|        |*|           |*|                 | 
 |*|          |*|           |*|        |*|           |*|                 | 
 \-\----------\-\-----------\-\--------\-\-----------\-\-----------------|
 /    entry for two frames  /                        /entry for one frame/ 
 |   to be received on CH0  |                        |to be transmitted  | 
 |   and CH1                |                        |redundantly        | 
 ------------------------------------------------------------------------- 

#warning "incompatible header bits with AS6003"

  *    3                  1        0            
  *    +------------------/--------+            
  *    |                  |        |            
  *    |      /           |   \    |            
  *    |     /            |    \   |            
  *    |    /             |     \  |            
  *    +---/--------------\------\-+            
  *       /                       \              
  *      -                         '            
  *   Mode Change request           Frame type  
 */
/**
 * Attention that the high 4 bits is reserved, but only 4 bits is used in
 * AS6003.
 */

typedef struct ttpc_header
{
	uint8_t header;
}ttpc_header_t;


/** 
 *                                                                                
         |            |                                  |             | 
         |  HEADER    |          V-PAYLOAD               |     FOOTER  | 
         |            |                                  |             | 
         +------------+----------+------------+----------+-------------+ 
 N-FRAME |   HEADER   |            PAYLOAD               |    CRC      | 
         |            |                                  |             | 
         +------------+----------+------------+----------+-------------+ 
         |   HEADER   |            C-STATE               |    CRC      | 
 I-FRAME |            |                                  |             | 
         +------------+----------+------------+----------+-------------+ 
         |   HEADER   |            C-STATE               |    CRC      | 
CS-FRAME |            |                                  |             | 
         +------------+----------+------------+----------+-------------+ 
         |   HEADER   | C-STATE  |    CRC0    | PAYLOAD  |    CRC1     | 
X-FRAME  |            |          |            |          |             | 
         +------------+----------+-----------------------+-------------+ 
 */
/**
 * frame type assembly definition .
 */
/**
 * field splitting for frames. Definition below is only used to splitting
 * frames.
 * @attention zero-size array is only supported by gnu c extension.
 */
 
#pragma anon_unions
typedef struct
{
    /** ttpc header */
    uint8_t hdr[1];

    /** anonymous union definition */
    union
    {
        struct
        {
            /**
             * flexible array is supported by iso-c99 and above, but 
             * at least one fixed-memory variable is needed.
             * at least 1 byte data needed
             */
            uint8_t payload[1];
        }n;
        struct
        {
            uint8_t cstate[12]; 
            uint8_t crc32[4];
        }i_cs;
        struct
        {
            uint8_t cstate[12]; 
            uint8_t crc32_0[4];
            uint8_t payload[1];
        }x; 
    };
        
}TTP_FrameStructDesc;

typedef struct
{
	/** the timestamp of the received frame in uint of microtick */
	uint32_t       rcv_timestamp;

    /** total size of a TTP/C frame received, including the crc32 */
    uint32_t       length;

    uint32_t       status;

    /** pointer to the RECV frame buffer */
	TTP_FrameStructDesc* pFrame;

}TTP_ChannelFrameDesc;

typedef struct
{
    TTP_ChannelFrameDesc *pCH0;
    TTP_ChannelFrameDesc *pCH1;
}TTP_FrameDesc;

/**
 * The setting of the two interfaces below needs negotiating. MAC_PrepareSCFrame
 * function should be called at startup time and DO NOT CALL IT AT NORMAL OPERATION
 * MODE OF THE CONTROLLER. The MAC_PushFrame function only assembles the frame I,X and
 * N. 
 * @return ttp_error_code
 */
uint32_t MSG_PushFrame(void);
uint32_t MSG_PrepareCSFrame(void);

/**
 * Push the data of the frame received to the corresponding CNI address. The frame
 * shall be a data frame.
 * @param  pDesc the chosen frame description
 * @return         non
 */
void MSG_PullAppData(TTP_ChannelFrameDesc* pDesc);

/**
 * This function fills the description of the frame received. If the frame 
 * received is  null, the function will return NULL.
 * @return          the array pointer of the frame descriptions of the corresponding
 * @attention if the frame received is not ok, the function will return NULL. 
 */
TTP_FrameDesc* MSG_GetFrameDesc(void);

/**
 * Get the timestamps of the received frame
 * @param  channel the channel, CH0 or CH1
 * @return         the corresponding timestamps
 */
uint32_t  MSG_GetFrameTimestamp(uint32_t channel);

/**
 * This function eliminates the error code mismatch between hardware
 * level and mac level.
 * @return  the error code of mac level
 */
uint32_t  MSG_GetTransmittedFlags(void);

/**
 * This function returns the flags of frames received.
 * @param    Channel     the channel, CH0 or CH1	
 * @return   0 non frame, 1 received a frame
 */
uint32_t  MSG_CheckReceived(uint32_t Channel);

// /**
//  * At the start of PRP phase, the acknowledgment algorithm will be performed. 
//  * If the ack process instantiated by hardware, the ack will be perform as
//  * soon as the mac receives a frame. But for software implement, the time 
//  * cost for performing acknowledgment algorithm shall be taken consideration.
//  * This function shall be called after the ack is performed.
//  * @param   channel    the channel, CH0 or CH1   
//  * @return  the frame status
//  */
// uint32_t  MSG_GetFrameStatus(uint32_t channel);


/*******************************************************************************/
/**
 * messages operations definitions below, which are the part of CNI access
 * services
 */

/**
 * the function MSG_CheckMsgRF is used to read Message-Ready-Flag of the corresponding
 * message, which is stored in the CNI, posited in address MSG_POOL_BASE + offset + 1.
 * When the controller sends a message, it must check the validity of this field, then 
 * clear it. When the controller receives a message, the Message-Ready-Flag is used 
 * for message status indicating.
 *
 * the status of frame-ready indicating. When the host ensures that the frame to 
 * be sent is OK, it shall write the particular bit pattern "0xEA" to the first
 * byte offset of the CNIAddressOffset of the corresponding slot configuration 
 * parameters, which means that the position of the application data starts from
 * the second byte. 
 * The host shall confirm the frame by writing the particular bit-pattern to the 
 * corresponding position before the controller sends the frame. if not, the
 * controller will raise the NR(Frame Not Ready) interruption and stop running.
 * The controller will clear the bit-pattern to zero after reading it out.
 */
#define STATUS_BIT_PATTERN     			((uint8_t)0xEA)

static inline uint32_t MSG_CheckMsgRF(uint32_t offset)
{
	uint32_t res = 0;

	res = ((MSG_POOL_BASE+(offset))[0]==STATUS_BIT_PATTERN)?1:0;
	(MSG_POOL_BASE+(offset))[0]=0;

	return res;
}

#define MSG_SetStatus(offset,status) 	((MSG_POOL_BASE+(offset))[0]=(status))	

//ensure that the CNI memory is a part of the memory of TTPC controller, having 
//the same address space. This macro-function returns the base address of the message
//field of the CNI. Notice the address type of uint8_t.
//attention the 1 byte status or NR.
#define MSG_GetMsgAddr(offset) 			(msg_pool+(offset)+1)	

#endif // !__MSG_H__
