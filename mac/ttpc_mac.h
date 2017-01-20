/**
  ******************************************************************************
  * @file    	ttpc_mac.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.09
  * @brief   	This file provides the MAC related operation interfaces
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
 
#ifndef __TTPC_MAC_H__
#define __TTPC_MAC_H__

////////////////////////////////////////////////////////////////////////////////
///message struct definitions                                                 //
////////////////////////////////////////////////////////////////////////////////
#include <ttpdef.h>


#define MAX_FRAME_LENGTH		240



/** frame type macro definitions */
#define FRAME_N             	0
#define FRAME_I             	1
#define FRAME_X             	2

/** for frame status field: the least significant 8-bits valid*/
#define FRAME_CORRECT 			(uint32_t)0x00000001
#define FRAME_TANTATIVE			(uint32_t)0x00000002
#define FRAME_MODE_VIOLATION	(uint32_t)0x00000003
#define FRAME_INCORRECT 		(uint32_t)0x00000004
#define FRAME_NULL				(uint32_t)0x00000005
#define FRAME_INVALID			(uint32_t)0x00000006

typedef enum{
	MAC_EOK=0,				/**< No errors happens */
	MAC_ESLOT_NUM,			/**< Current slot > MAX_SLOT_NUMBER */
	MAC_ENON_DATA,			/**< No data to be transmitted  */
	MAC_ESIZE_OVER,			/**< The size of the message oversizes */
	MAC_ERS,				/**< The message is not be confirmed by the host */
	MAC_EMODE, 				/**< detect a mode violation */

	MAC_ETX_COL,			/**< collision detected during transmitting */
	MAC_ETX_INV,			/**< invalid transmission time */

	MAC_ERX_NON,			/**< nothing has been received */
	MAC_ERX_COL,			/**< collision detected during receiving */
	MAC_ERX_INV,			/**< invalid frames has been received */
	MAC_ERX_ECRC,           /**< received a frame with crc failed */
	MAC_ERX_LTH,            /**< received a frame with length error */

	MAC_EPHY,				/**< physical hardware fault */
	MAC_EOTHER				/**< Other unknown errors */
}MAC_err_t;

#warning "incompatible crc bits with AS6003"
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
 */

/**
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
//  */
// typedef union msg_cell
// {
// 	struct
// 	{
// 		uint8_t* msg0_status;
// 		uint8_t* msg0_length;
// 		uint8_t* msg0_data;
// 		uint8_t* msg1_status;
// 		uint8_t* msg1_length;
// 		uint8_t* msg1_data;
// 	}receive_cell;

// 	struct
// 	{
// 		uint8_t* msg_send_status;
// 		uint8_t* send_length;
// 		uint8_t* msg_send_data;
// 	}send_cell;

// }msg_cell_desc_t;

#warning "incompatible header bits with AS6003"

/**                                            
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
	uint8_t header
}ttpc_header_t;

////////////////////////////////////////////////////////////////////////////////
///c-state  access service                                                    //
////////////////////////////////////////////////////////////////////////////////
/**
 * C-state structure definition
 *                                                                            
 15                                                                      0 
 +-----------------------------------------------------------------------+ 
 |                      GLOBAL TIME  BASE:16                             | 
 +-------+-----------+---------------------------------------------------+ 
 | DMC:2 |  MODE:3   |                      ROUND SLOT:11                | 
 +-------+-----------+---------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 15-0                             | 
 +-----------------------------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 31-16                            | 
 +-----------------------------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 47-48                            | 
 +-----------------------------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 63-48                            | 
 +-----------------------------------------------------------------------+ 
 */
/**
 * Notice the bit-incompatibility between the c-state definition below and the 
 * the definition of the CNI from the ttpdef.h.
 * For the CNI, 32-bits register is required, but 16-bits for AS6003. The type
 * c_state_t si used to eliminate the bit-differences between the standard
 * definition and the CNI definition for the convenience of CRC check.
 */
typedef struct ttpc_state
{
	uint16_t GlobalTime;
	uint16_t ClusterPosition;
	uint16_t Membership[4];
}c_state_t;

/**
 * The macro fills the cs variable. PARAMETER LEGALITY WILL
 * NOT BE CHECKED. BE SURE THAT THE CS IS A POINTER, OR 
 * COMPILE ERROR WILL OCCUR.
 * @param  cs a c_state_t type variable.
 * @return    non
 */
#define MAC_GetCState(pcs)			do{(pcs)->GlobalTime     = C_STATE_GT&0xffff;\
                                	       (pcs)->ClusterPosition= C_STATE_CP&0xffff;\
                                	       (pcs)->Membership[0]  = C_STATE_MV0&0xffff;\
                                	       (pcs)->Membership[1]  = C_STATE_MV1&0xffff;\
                                	       (pcs)->Membership[2]  = C_STATE_MV2&0xffff;\
                                	   	   (pcs)->Membership[3]  = C_STATE_MV3&0xffff;\
                                 	}while(0)

/** Be sure the cs is a pointer. Legality will not be checked. */
#define MAC_SetCState(pcs) 			do{C_STATE_GT =(pcs)->GlobalTime&0xffff;\
										   C_STATE_CP =(pcs)->ClusterPosition&0xffff;\
										   C_STATE_MV0=(pcs)->Membership[0]&0xffff;\
										   C_STATE_MV1=(pcs)->Membership[1]&0xffff;\
										   C_STATE_MV2=(pcs)->Membership[2]&0xffff;\
										   C_STATE_MV3=(pcs)->Membership[3]&0xffff;\
									}while(0)

/**
 * judge whether the c-state cs is the same as local c-state. Be sure that the cs is 
 * a pointer to the c-state structure.
 * @return 1 cs is the same as local c-state
 *         0 cs is not the same as local c-state
 */
#define CS_IsSame(pcs)          (((pcs)->GlobalTime     ==(C_STATE_GT&0xffff))  && \
									((pcs)->ClusterPosition==(C_STATE_CP&0xffff))  && \
									((pcs)->Membership[0]  ==(C_STATE_MV0&0xffff)) && \
									((pcs)->Membership[1]  ==(C_STATE_MV1&0xffff)) && \
									((pcs)->Membership[2]  ==(C_STATE_MV2&0xffff)) && \
									((pcs)->Membership[3]  ==(C_STATE_MV3&0xffff)))

/** p
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
typedef struct TTP_frame
{
    // uint8_t dst[6];
    // uint8_t src[6];
    // uint8_t lth[2];

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
             */
            uint8_t payload[0];
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
            uint8_t payload[0];
        }x; 
    };
        
}*pTTP_frame;


typedef struct ttp_frame_desc
{
	/** the timestamp of the received frame in uint of microtick */
	uint32_t       rcv_timestamp;
	pTTP_frame     pFrame;

}ttp_frame_desc_t;

/**                                                               
               31-09              8-6       5-3        2-0       
 +-----------------------------+---------+---------+----------+  
 |              REV            |   SLTS  |   FS1   |   FS0    |  
 +-----------------------------+---------+---------+----------+  
                                                            
 *               SLTS = MIN(FS0,FS1)                                                                
 */

/**
 * The setting of the two interfaces below needs negotiating. MAC_PrepareSCFrame
 * function should be called at startup time and DO NOT CALL IT AT NORMAL OPERATION
 * MODE OF THE CONTROLLER. The MAC_PushFrame function only assembles the frame I,X and
 * N.
 */
MAC_err_t MAC_PushFrame(void);
MAC_err_t MAC_PrepareCSFrame(void);

void      MAC_StartTransmit(void);
void      MAC_StopTransmit(void);

uint32_t  MAC_GetTransmittedFlags(void);


void      MAC_StartReceive(void);
void      MAC_StopReceive(void);
/** check whether the mac has received frame with returning 0 for receiving nothing, 
 *  returning 1 for receiving frames.
 *  receiving nothing.
 */
uint32_t  MAC_CheckReceived(void);

/**
 * This function returns the flags of frames received.	
 * @return  the flags of frames received, values below:
 *           @arg MAC_ERX_NON    nothing has been received
 *           @arg MAC_ERX_INV	 invalid frames has been received
 *           @arg MAC_ERX_COL 	 collision detected when receiving
 *           @arg MAC_ERX_ECRC 	 received a frame with crc failed
 *           @arg MAC_ERX_ELT    received a frame with length error
 *           @arg MAC_EOK        reveived a good frame.
 */
uint32_t  MAC_GetReceivedFlag(void);

/**
 * Get the timestamps of the received frame
 * @param  channel the channel, CH0 or CH1
 * @return         the corresponding timestamps
 */
uint32_t  MAC_GetFrameTimestamp(uint32_t channel);

/**
 * At the start of PRP phase, the acknowledgment algorithm will be performed. 
 * If the ack process instantiated by hardware, the ack will be perform as
 * soon as the mac receives a frame. But for software implement, the time 
 * cost for performing acknowledgment algorithm shall be taken consideration.
 * This function shall be called after the ack is performed.
 * @param   channel    the channel, CH0 or CH1	 
 * @return  the frame status
 */
uint32_t  MAC_GetFrameStatus(uint32_t channel);

/**
 * Push the data of the frame received to the corresponding CNI address. The frame
 * shall be a data frame.
 * @param  channel the channel, CH0 or CH1
 * @return         non
 */
void      MAC_PushAppData(uint32_t channel);

/**
 * This function fills the description of the frame received. If the frame 
 * received is incorrect or null or invalid, the function will return 0.
 * @param desc[2]   the array pointer of the frame descriptions
 * @return   0      for invalid or incorrect or null frame,
 *           1      for the other frame.
 * @attention if the frame received is not ok, the desc pointer will be set
 * NULL. 
 */
uint32_t  MAC_GetFrameDesc(ttpc_frame_desc_t* desc[2]);

////////////////////////////////////////////////////////////////////////////////
///slots service definitions                                                  //
////////////////////////////////////////////////////////////////////////////////


typedef struct mac_slot 
{
	uint32_t LogicalSenderSlot;
	uint32_t LogicalSenderMultiplexID;
	uint32_t SlotDuration;				/**< in unit of macroticks */
	uint32_t TransmissionDuration;
	uint32_t DelayCorrectionTerms;		/**< in unit of ns */
	uint32_t CNIAddressOffset;			/**< offset relative to CNI base addr */
	uint32_t AppDataLength;
	uint32_t FlagPosition;				/**< flag position in membership vector of 
	                                         the current sending node*/
	uint32_t FrameType;					/**<  implicit or explicit */
	uint32_t ModeChangePermission;
	uint32_t ReintegrationAllow;
	uint32_t ClockSychronization;
	uint32_t SynchronizationFrame;
	/* the time for action in unit of macrotick relative to the start of round slot*/
	uint32_t AtTime;					

}RoundSlotProperty_t;

/** RoundSlotProperty_t.ModeChangePermission */
#define MODE_CHANGE_PERMIT 				(uint32_t)0x00000001
#define MODE_CHANGE_DENY				(uint32_t)0x00000000

/** RoundSlotProperty_t.ReintegrationAllow */
#define REINTEGRATION_ALLOWED 			(uint32_t)0x00000001
#define REINTEGRATION_NOT_ALLOWED 		(uint32_t)0x00000000

/** RoundSlotProperty_t.ClockSynchronization */
#define CLOCK_SYN_NEEDED 				(uint32_t)0x00000001
#define CLOCK_SYN_NOT_NEEDED			(uint32_t)0x00000000

/** RoundSlotProperty_t.SynchronizationFrame */
#define SYN_FRAME 						(uint32_t)0x00000001
#define NOT_SYN_FRAME					(uint32_t)0x00000000

/** RoundSlotProperty_t.FrameType */
#define FRAME_TYPE_IMPLICIT 			(uint32_t)0x00000000
#define FRAME_TYPE_EXPLICIT 			(uint32_t)0x00000001

#define SLOT_PHASE_PRP 					1
#define SLOT_PHASE_AT 					2
#define SLOT_PHASE_PRP 					3

#define SENDING_FRAME 					1
#define RECEIVING_FRAME  				2

#define NORMAL_SLOT							0
#define LAST_SLOT_OF_CURRENT_TDMAROUND 		1
#define FIRST_SLOT_OF_SUCCESSOR_TDMA_ROUND	2
#define LAST_SLOT_OF_CURRENT_CLUSTER 		3
#define FIRST_SLOT_OF_CURRENT_CLUSTER   	4

/**
 * Get the round slot of the current slot.
 * @return  the round slot entry.
 */
RoundSlotProperty_t* MAC_GetRoundSlotProperties(void);

/**
 * This function should be called after the membership point. In the membership
 * point, the frame status will be confirmed, then the slot status will be validated.
 * The slot status is used for clique detection.		 
 * @return  the slot status.
 */
uint32_t  MAC_GetSlotStatus(void);
uint32_t  MAC_GetNodeSlot(void);
uint32_t  MAC_GetTDMARound(void);

// /** 
//  * Get the TDMA slots for the current TDMA round, which shall be the same 
//  * in all TDMA round of a cluster cycle.
//  * @return  the slots number of the current TDMA round
//  */
// uint32_t  MAC_GetTDMASlots(void);

// /** 
//  * Get cluster cycles for the current mode. For different modes, the corresponding 
//  * cluster cycle may be different.
//  * @return  the cluster cycle of the current mode.
//  */
// uint32_t  MAC_GetClusterCycles(void);

/**
 * Check whether the current slot is the node own slot. These functions shall be called
 * after the slot is updated.
 *
 * The function MAC_IsOwnNodeSlot only checks the logical name part of the whole logical
 * name field, the multiplexed logical name part is not checked.
 *
 * The function MAC_IsSending slot check both the logical name part and the multiplexed
 * part of the whole logical name.
 * 
 * @return  1 if yes, 0 if not.
 */	
uint32_t  MAC_IsOwnNodeSlot(void);
uint32_t  MAC_IsSendSlot(void);
uint32_t  MAC_IsFirstSLotOfCluster(void);

/**
 * Return the trigger timestamps of the slot phases, psp,at,prp. 
 * @param  Phase the psp, at, prp.
 * @return       the timestamps
 */
uint32_t  MAC_GetPspTsmp(void);

/**
 * update the node slot and the TDMA round and the round slot. If the updated slot
 * points the first slot of the cluster, the function will return FIRST_SLOT_OF_CURRENT_CLUSTER.
 * If the updated slot points the last one of a TDMA round, the function will return 
 * FIRST_SLOT_OF_CURRENT_CLUSTER. Otherwise, the function will return NORMAL_SLOT.
 * @return  @arg NORMAL_SLOT
 *          @arg LAST_SLOT_OF_CURRENT_TDMA_ROUND
 *          @arg FIRST_SLOT_OF_CURRENT_CLUSTER
 */
uint32_t  MAC_UpdateSlot(void);

/**
 * Load the slot configuration parameters from the MEDL.
 * @param  mode the current mode
 * @param  tdma the current TDMA round
 * @param  slot the current slot
 * @return      the slot configuration parameters entry.
 * @attention   ensure that the tdma and the slot are updated at the start of PSP before
 *              calling this function.
 */
RoundSlotProperty_t* MAC_LoadSlotProperties(uint32_t mode,uint32_t tdma,uint32_t slot);

/** 
 * mark whether a frame should be transmitted in this slot.
 * @param  SlotAcquisition SENDING_FRAME or RECEIVING_FRAME
 * @return                 non
 */
void  	MAC_SetSlotAcquisition(uint32_t SlotAcquisition);
uint32_t MAC_GetSlotAcquisition(void);

/**
 * Set the time properties of the slot
 * @param  ActAT the actual trigger point time for TP phase
 * @param  TP    the time duration of TP phase in unit of macrotick
 * @param  SD    the slot duration time for a slot in unit of macrotick
 * @return       non
 */
void     MAC_SetTime(uint32_t ActAT,uint32_t TP,uint32_t SD);

void     MAC_StartPhaseCirculation(void);
void     MAC_StopPhaseCirculation(void);

/**
 * Set the status of the current slot, which is executed during acknowledgment stage.
 * @param  SlotStatus the slot status
 * @return            non
 */
void    MAC_SetSlotStatus(uint32_t SlotStatus);

/**
 * This function checks pointer of the current slot.
 * @return  the pointer status.
 *              @arg NORMAL_SLOT
 *              @arg LAST_SLOT_OF_CURRENT_TDMAROUND
 *              @arg FIRST_SLOT_OF_SUCCESSOR_TDMA_ROUND
 *              @arg LAST_SLOT_OF_CURRENT_CLUSTER
 *              @arg FIRST_SLOT_OF_CURRENT_CLUSTER
 * @attention 
 *   The function performs the same as the function MAC_UpdateSlot seemingly. The
 *   only difference between the two is that the function doesn't update the slot.
 *   The function should be called after the slot is updated.
 */
uint32_t MAC_CheckSlot(void);

////////////////////////////////////////////////////////////////////////////////
///identification function definitions                                        //
////////////////////////////////////////////////////////////////////////////////

/**
 * Attention that SETTER functions can only be called in initialization.
 */

void      MAC_SetClusterScheduleID(uint32_t ScheduleID);
void      MAC_SetAppID(uint32_t AppID);
uint32_t  MAC_GetClusterScheduleID(void);
uint32_t  MAC_GetAppID(void);

////////////////////////////////////////////////////////////////////////////////
///the role of node specification                                             //
////////////////////////////////////////////////////////////////////////////////

/**
 * The properties below are specified for the node at initialization phase
 */

typedef struct mac_node
{
	uint32_t LogicalNameSlotPosition;
	uint32_t LogicalNameMulplexedID;
	uint32_t PassiveFlag;				/**< Marks the node as a permanent passive */
	uint32_t MultiplexedMembershipFlag;
	uint32_t FlagPosition;				/**< flag position in membership vector */
	uint32_t SendDelay;					/**< int unit of macrotick */
}NodeProperty_t;

/** NodeProperty_t.PassiveFlag */
#define PERMANENT_PASSIVE				(uint32_t)0x00000001
#define NOT_PASSIVE						(uint32_t)0x00000000

/** NodeProperty_t.MultiplexedMembershipFlag */
#define MULTIPLEXED_MEMBERSHIP			(uint32_t)0x00000001
#define MONOPOLIZED_MEMBERSHIP			(uint32_t)0x00000000

void 		MAC_SetNodeProperties(NodeProperty_t *NodeProperty);
NodeProperty_t* MAC_GetNodeProperties(void);

uint32_t  	MAC_IsPassiveNode(void);
uint32_t 	MAC_IsMultiplexedMembershipNode(void);

////////////////////////////////////////////////////////////////////////////////
///schedule parameters definitions                                            //
////////////////////////////////////////////////////////////////////////////////

/**
 * the parameters below are used for the basic communication behavior of a node 
 * and are necessary to integrate or startup a cluster, which are specified at
 * initialization phase.
 */

typedef struct mac_schedule
{
	uint32_t ColdStartAllow;
	uint32_t ColdStartIntegrationAllow;
	uint32_t ExternalRateCorrectionAllow;
	uint32_t MinimumIntegrationCount;
	uint32_t MaximumColdStartEntry;
	uint32_t MximumMembershipFailureCount;
	uint32_t MacrotickParameter;
	uint32_t Precision;
	uint32_t CommunicationRate;

	uint32_t StartupTimeout;
	uint32_t ListenTimeout;
	uint32_t ColdStartTimeout;
}ScheduleParameter_t;

/** ScheduleParameter_t.ColdStartAllow */
#define COLD_START_ALLOWED 				(uint32_t)0x00000001
#define COLD_START_NOT_ALLOWED 			(uint32_t)0x00000000

/** ScheduleParameter_t.ColdStartIntegrationAllow */
#define COLD_START_INTEGRATION_ALLOWED 	(uint32_t)0x00000001
#define COLD_START_INTEGRATION_NOT_ALLOWED (uint32_t)0x00000000

/** ScheduleParameter_t.ExternalRateCorrectionAllow */
#define EXTERNAL_CORRECTION_ALLOWED 	(uint32_t)0x00000001
#define EXTERNAL_CORRECTION_NOT_ALLOWED (uint32_t)0x00000000

/** ScheduleParameter_t.CommunicationRate */
#define COMMUNICATION_RATE_10M 			(uint32_t)0x0000000A
#define COMMUNICATION_RATE_100M 		(uint32_t)0x00000064

void  		MAC_SetScheduleParameter(ScheduleParameter_t* ScheduleParameter);
ScheduleParameter_t* MAC_GetScheduleParameter(void);
uint32_t 	MAC_GetMinimumIntegrationCount(void);
uint32_t 	MAC_GetMaximumColdStartEntry(void);
uint32_t 	MAC_GetMximumMembershipFailureCount(void);
uint32_t 	MAC_GetMacrotickParameter(void);
uint32_t 	MAC_GetPrecision(void);

////////////////////////////////////////////////////////////////////////////////
///mac contrl service                                                         //
////////////////////////////////////////////////////////////////////////////////

/**
 * The parameters below shall be considered at mode changing phase.
 */

/**
 *                                                         
		          TDMA ROUND BOUNDARY                           		                                                        
		                    |           |                       
		        +--+--+--+--|--+--+--+--|                       
		 mode 1 |  |  |  |  |  |  |  |  |                       
		        +--+--+--+--|--+--+--+--|                       
		        +--+--+--+--|--+--+--+--|--+--+--+--+           
		 mode 2 |  |  |  |  |  |  |  |  |  |  |  |  |           
		        +--+--+--+--|--+--+--+--|--+--+--+--+           
		        +--+--+--+--|           |                       
		 mode x |  |  |  |  |           |    MAX CLUSTER CYCLES 
		        +--+--+--+--|           |                       
		                    |           |                       		                                                        
		              MAX TDMA CYCLES                           
 */


#endif 