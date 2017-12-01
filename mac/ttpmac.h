/**
  ******************************************************************************
  * @file    	ttpmac.h
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
 
#ifndef __TTPMAC_H__
#define __TTPMAC_H__

////////////////////////////////////////////////////////////////////////////////
///message struct definitions                                                 //
////////////////////////////////////////////////////////////////////////////////
#include "ttpdef.h"

#define MAX_FRAME_LENGTH		240

/** frame type macro definitions */
#define FRAME_N             	0
#define FRAME_I             	1
#define FRAME_X             	2

/** for frame status field: the least significant 8-bits valid*/
#define FRAME_CORRECT 			(uint32_t)0x00000001
#define FRAME_TENTATIVE			(uint32_t)0x00000002
#define FRAME_MODE_VIOLATION	(uint32_t)0x00000003
#define FRAME_INCORRECT 		(uint32_t)0x00000004
#define FRAME_NULL				(uint32_t)0x00000005
#define FRAME_INVALID			(uint32_t)0x00000006

typedef enum Mac_err{
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

void      MAC_StartTransmit(void);
void      MAC_StopTransmit(void);

uint32_t  MAC_GetTransmittedFlags(void);


void      MAC_StartReceive(void);
void      MAC_StopReceive(void);

// * check whether the mac has received frame with returning 0 for receiving nothing, 
//  *  returning 1 for receiving frames.
//  *  receiving nothing.
 
// uint32_t  MAC_CheckReceived(void);

/**
 * This function returns the flags of frames received.
 * @param    Channel     the channel, CH0 or CH1	
 * @return  the flags of frames received, values below:
 *           @arg MAC_ERX_NON    nothing has been received
 *           @arg MAC_ERX_INV	 invalid frames has been received
 *           @arg MAC_ERX_COL 	 collision detected when receiving
 *           @arg MAC_ERX_ECRC 	 received a frame with crc failed
 *           @arg MAC_ERX_ELT    received a frame with length error
 *           @arg MAC_EOK        reveived a good frame.
 */
uint32_t  MAC_GetReceivedFlag(uint32_t Channel);

/**
 * Get the timestamps of the received frame
 * @param  channel the channel, CH0 or CH1
 * @return         the corresponding timestamps
 */
uint32_t  MAC_GetFrameTimestamp(uint32_t channel);

////////////////////////////////////////////////////////////////////////////////
///slots service definitions                                                  //
////////////////////////////////////////////////////////////////////////////////


typedef struct mac_slot 
{
	uint32_t LogicalSenderSlot;
	uint32_t LogicalSenderMultiplexID;

	uint32_t SlotDuration;				/**< in unit of macroticks */
	uint32_t PSPDuration;               /**< in unit of macroticks */
	uint32_t TransmissionDuration;      /**< in unit of macroticks */

	uint32_t DelayCorrectionTerms;		/**< in unit of ns */
	uint32_t CNIAddressOffset;			/**< offset relative to CNI base addr */
	uint32_t AppDataLength;
	uint32_t FlagPosition;				/**< flag position in membership vector of 
	                                         the current sending node*/
	uint32_t FrameType;					/**<  implicit or explicit */
	uint32_t ModeChangePermission;      
	uint32_t ReintegrationAllow;
	uint32_t ClockSynchronization;
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

#define SLOT_PHASE_PSP 					1
#define SLOT_PHASE_AT 					2
#define SLOT_PHASE_PRP 					3

#define SENDING_FRAME 					1
#define RECEIVING_FRAME  				2

#define NORMAL_SLOT							0
#define LAST_SLOT_OF_CURRENT_TDMAROUND 		1
#define FIRST_SLOT_OF_SUCCESSOR_TDMAROUND	2
#define LAST_SLOT_OF_CURRENT_CLUSTER 		3
#define FIRST_SLOT_OF_CURRENT_CLUSTER   	4

/**
 * Get the round slot of the current slot.
 * @return  the round slot entry.
 */
RoundSlotProperty_t* MAC_GetRoundSlotProperties(void);

// *
//  * This function should be called after the membership point. In the membership
//  * point, the frame status will be confirmed, then the slot status will be validated.
//  * The slot status is used for clique detection.		 
//  * @return  the slot status.
 
// uint32_t  MAC_GetSlotStatus(void);
/**
 * Set the status of the current slot, which is executed during acknowledgment stage.
 * @param  SlotStatus the slot status
 * @return            non
 */
void    MAC_SetSlotStatus(uint32_t SlotStatus);

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
 * At the start of PRP phase, the acknowledgment algorithm will be performed. 
 * If the ack process instantiated by hardware, the ack will be perform as
 * soon as the mac receives a frame. But for software implement, the time 
 * cost for performing acknowledgment algorithm shall be taken consideration.
 * This function shall be called after the ack is performed.
 * @param   channel    the channel, CH0 or CH1   
 * @return  the frame status
 */
uint32_t  MAC_GetFrameStatus(uint32_t channel);


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
	uint32_t LogicalNameMultiplexedID;
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
	uint32_t MaximumMembershipFailureCount;
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
uint32_t 	MAC_GetMaximumMembershipFailureCount(void);

/**
 * get the unified time interval of macrotick, normally the integral multiple of us
 * @return  the time interval of macrotick granularity, in unit of ns.
 */
uint32_t 	MAC_GetMacrotickParameter(void);
uint32_t 	MAC_GetPrecision(void);

////////////////////////////////////////////////////////////////////////////////
///mac contrl service                                                         //
////////////////////////////////////////////////////////////////////////////////
///
/** 
 * Get the TDMA slots for the current TDMA round, which shall be the same 
 * in all TDMA round of a cluster cycle.
 * @return  the slots number of the current TDMA round
 */
uint32_t  MAC_GetTDMARound(void);

/** get the node slot at current round cycle */
uint32_t  MAC_GetNodeSlot(void);

/** get the cycle slot in a cycle */
uint32_t  MAC_GetRoundSlot(void);

/** set the cycle length of current mode */
void      MAC_SetClusterCycleLength(uint32_t Length);

/** set the TDMA length of the current mode */
void      MAC_SetTDMACycleLength(uint32_t Length);

/**
 * Return the trigger timestamps of the slot phases, psp,at,prp. 
 * @param  Phase the psp, at, prp.
 * @return       the timestamps
 */

/**
 * This function returns the ratio of Macrotick to Microtick.
 *
 * @attention The integral multiple shall be guaranteed by the hardware. If not, the hardware 
 * shall not be allowed allowed/downloaded the corresponding MEDL 
 * 
 * @return the ratio of Macrotick to Microtick
 */
uint32_t  MAC_GetRatio();
uint32_t  MAC_GetSlotStartMacroticks(void);

uint32_t  MAC_GetSlotStartMicroticks(void);
uint32_t  MAC_GetATMicroticks(void);

/**
 * update the node slot and the TDMA round and the round slot. If the updated slot
 * points the first slot of the cluster, the function will return FIRST_SLOT_OF_CURRENT_CLUSTER.
 * If the updated slot points the last one of a TDMA round, the function will return 
 * FIRST_SLOT_OF_CURRENT_CLUSTER. Otherwise, the function will return NORMAL_SLOT.
 * @return  @arg NORMAL_SLOT
 *          @arg LAST_SLOT_OF_CURRENT_TDMAROUND
 *          @arg FIRST_SLOT_OF_CURRENT_CLUSTER
 */
uint32_t  MAC_UpdateSlot(void);
void      MAC_SetSlot(uint32_t Slot);
void      MAC_SetTDMARound(uint32_t tdma);
/**
 * Set the time properties of the slot.
 * @param  ActAT the actual trigger point time for TP phase
 * @param  TP    the time duration of TP phase in unit of macrotick
 * @param  SD    the slot duration time for a slot in unit of macrotick
 * @param  PSP   the psp time duration of the slot in unit if macrotick
 * @return       non
 */
void     MAC_SetTime(uint32_t ActAT,uint32_t TP, uint32_t PSP,uint32_t SD);

/**
 * Set the start time of the phase circulation, indicating the start macrotick point of the periodic
 * TDMA round.
 * @param CycleStartTime   the start macrotick time of the cluster cycle
 * @param TDMAStartOffset  the start offset of the TDMA round in the cluster cycle
 * @return                 non
 */
void MAC_SetPhaseCycleStartPoint(uint32_t CycleStartTime, uint32_t TDMAStartOffset /* 0 always for current version */);

void     MAC_StartPhaseCirculation(void);
void     MAC_StopPhaseCirculation(void);

/**
 * This function checks pointer of the current slot.
 * @return  the pointer status.
 *              @arg NORMAL_SLOT
 *              @arg LAST_SLOT_OF_CURRENT_TDMAROUND
 *              @arg FIRST_SLOT_OF_SUCCESSOR_TDMAROUND
 *              @arg LAST_SLOT_OF_CURRENT_CLUSTER
 *              @arg FIRST_SLOT_OF_CURRENT_CLUSTER
 * @attention 
 *   The function performs the same as the function MAC_UpdateSlot seemingly. The
 *   only difference between the two is that the function doesn't update the slot.
 *   The function should be called after the slot is updated.
 */
uint32_t MAC_CheckSlot(void);

//void     MAC_SetWindow(uint32_t MiddleAxis);

/** clock adjust mode options*/
#define CLK_PHASE_ADJ           (uint16_t)0x0001
#define CLK_FREQ_ADJ            (uint16_t)0x0002

/**
 * Offset adjusting for local clock
 * @param AdjMode the adjust mode, CLK_PHASE_ADJ or CLK_FREQ_ADJ
 * @param Offset  the clock offset, in unit of signed integer of microtick
 */
void MAC_AdjTime(uint16_t AdjMode, int16_t Offset);

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