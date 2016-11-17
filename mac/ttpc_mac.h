/**
  ******************************************************************************
  * @file    	ttpc_mac.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
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

/**
 * TTPC messages are asigned to specified slots statically. The particular 
 * message received will be put in the slot-relative message area according
 * to its slot.
 * The maximum value of a TTPC message in unit of byte. see AS6003, page 53
 */
#define MAX_FRAME_LENGTH				240

typedef enum{
	MAC_EOK=0,				/**< No errors happend */
	MAC_ESLOT_NUM,			/**< Current slot > MAX_SLOT_NUMBER */
	MAC_ENON_DATA,			/**< No data to be thansmitted  */
	MAC_ESIZE_OVER,			/**< The size of the message oversizes */
	MAC_ERS,				/**< The message is not be confirmed by the host */
	MAC_EMODE, 				/**< detect a mode vialation */
	MAC_EOTHER				/**< Other unknown errors */
}MAC_err_t;

#warning "incompatible crc bits with AS6003"
typedef uint32_t crc32_t;
typedef crc32_t  crc_t;

/**
 * We set the size of message area forcely and fixedly for every round slot. The
 * message to be received on CH0 and CH1 will be pulled in a dual-cell receiving 
 * buffer respectively. And the message to be transimitted redundantly on CH0
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
 |                          |                        \                   \ 
 \                          \                        \                   | 
 /-/----------/-/-----------/-/--------/-/-----------/-/-----------------\ 
 |*|          |*|           |*|        |*|           |*|                 | 
 |*|          |*|           |*|        |*|           |*|                 | 
 |*|          |*|           |*|        |*|           |*|                 | 
 |*|          |*|           |*|        |*|           |*|                 | 
 \-\----------\-\-----------\-\--------\-\-----------\-\-----------------\ 
 /    entry for two frames  /                        /entry for one frame/ 
 |   to be received on CH0  |                        |to be transmitted  | 
 |   and CH1                |                        |redundantly        | 
 ------------------------------------------------------------------------- 
//  */

#define STATUS_BIT_PATTERN     ((uint8_t)0xEA)

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

/** getters below */
uint32_t    MAC_CSGetCurRoundSlot(void);
uint32_t    MAC_CSGetCurMode(void);
uint32_t    MAC_CSGetCurDMC(void);

uint32_t    MAC_CSGetCurGTF(void);

uint32_t    MAC_CSGetMemberBit(int Position);

/** setters below */
void 		MAC_CSSetRoundSlot(int RoundSlot);
void		MAC_CSSetMode(uint32_t Mode);
void 		MAC_CSSetDMC(uint32_t DMC);
void 		MAC_CSSetGTF(uint32_t GTF);
void        MAC_CSSetCState(c_state_t* c_state);
void  		MAC_CSSetMemberBit(int PositionNumber);

/** clear operation for membership flag */
void 		MAC_CSClearMemberAll(void);
void        MAC_CSClearMemberBit(int PositionNumber);

/** macro definition for c_state accessment */
#define MAC_CSGetCurRoundSlot() 	(C_STATE_CP&0x07ff)
#define MAC_CSGetCurMode()			(C_STATE_CP&0x3800)
#define MAC_CSGetCurDMC()			(C_STATE_CP&0xc000)

/**
 * The macro fill the cs variable. DO NOT PASS A PONTER FOR THE MACRO. LEGALITY WILL
 * NOT BE CHECKED.
 * @param  cs a c_state_t type variable.
 * @return    non
 */
#define MAC_CSGetCState(cs)			do{(cs)->GlobalTime     = C_STATE_GT&0xffff;\
                                       (cs)->ClusterPosition= C_STATE_CP&0xffff;\
                                       (cs)->Membership[0]  = C_STATE_MV0&0xffff;\
                                       (cs)->Membership[1]  = C_STATE_MV1&0xffff;\
                                       (cs)->Membership[2]  = C_STATE_MV2&0xffff;\
                                   	   (cs)->Membership[3]  = C_STATE_MV3&0xffff;\
                                   	}while(0)

#define MAC_CSGetCurGTF()			(C_STATE_GT)
#define MAC_CSGetMemberBit(pos)		(((*(C_STATE_BASE+((pos)/16)+2))>>((pos)%16))&1)


#define MAC_CSSetRoundSlot(rs)		(C_STATE_CP|=((rs)&0x07ff))
#define MAC_CSSetMode(md)			(C_STATE_CP|=((md)&0x3800))
#define MAC_CSSetDMC(mc)			(C_STATE_CP|=((mc)&0xc000))
#define MAC_CSSetGTF(gt)			(C_STATE_GT=(gt)&0xffff)



/** Be sure the cs is a pointer. Legality will not be checked. */
#define MAC_CSSetCState(cs) 		do{C_STATE_GT =(cs)->GlobalTime&0xffff;\
									   C_STATE_CP =(cs)->ClusterPosition&0xffff;\
									   C_STATE_MV0=(cs)->Membership[0]&0xffff;\
									   C_STATE_MV1=(cs)->Membership[1]&0xffff;\
									   C_STATE_MV2=(cs)->Membership[2]&0xffff;\
									   C_STATE_MV3=(cs)->Membership[3]&0xffff;\
									}while(0)

#define MAC_CSSetMemberBit(pos)		((*(C_STATE_BASE+((pos)/16)+2))|=1<<((pos)%16))



#define MAC_CSClearMemberAll()		do{C_STATE_MV0=0;\
									   C_STATE_MV1=0;\
									   C_STATE_MV2=0;\
									   C_STATE_MV3=0;\
									}while(0)

#define MAC_CSClearMemberBit(pos)	((*(C_STATE_BASE+((pos)/16)+2))&=~(1<<((pos)%16)))


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
typedef struct ttpc_frame
{
	ttpc_header_t  header;
	uint8_t        appDataLength;
	union
	{
		struct
		{
			void* pAppData;
		}N_frame;

		struct
		{
			c_state_t  c_state;
		}I_CS_frame;

		struct
		{
			c_state_t  c_state;
			crc_t      crc;
			void*      pAppData;
		}X_frame;
	}frame_body_t;

	crc_t    pFooter;
}ttpc_frame_desc_t;

/** for frame status field: the least significant 8-bits valid*/

#define FRAME_CORRECT 					(uint32_t)0x00000001
#define FRAME_TANTATIVE					(uint32_t)0x00000002
#define FRAME_MODE_VIOLATION			(uint32_t)0x00000003
#define FRAME_INCORRECT 				(uint32_t)0x00000004
#define FRAME_NULL						(uint32_t)0x00000005
#define FRAME_INVALID					(uint32_t)0x00000006

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

uint32_t  MAC_GetTransmittedFlags(void);

/** check whether the mac has received frame and perform the frame status algorithm */
uint32_t  MAC_CheckReceived(void); 
 
/**
 * Be sure that the function MAC_CheckReceived has been called before calling the 
 * function below. If nothing or wrong frame has been received, the contents of 
 * the desc[2] will be set NULL. Attention that the contents of the desc[2] will
 * also be set null if the MAC_CheckReceived function is not be called.
 */
void      MAC_PullFrames(ttpc_frame_desc_t *desc[2]);

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

#define SET 							(uint32_t)0x00000001
#define RESET 							(uint32_t)0x00000000


void      MAC_SetRoundSlotProperties(uint32_t Mode,uint32_t RoundSlot);
RoundSlotProperty_t* MAC_GetRoundSlotProperties(void);

uint32_t  MAC_IsFlagSet(uint32_t Flags);
uint32_t  MAC_GetSlotStatus(void);
void      MAC_UpdateRoundSlot();

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
NodeProperty_t* MAC_GetNodeProperty(void);

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