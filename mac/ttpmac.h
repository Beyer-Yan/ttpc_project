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

#include "ttpdef.h"

/******************************* parameters access interface **************************/

/**
 * @defgroup TTP_MAC_SLOT_PARAMETERS_CONTROL
 */
/**@{*/

typedef struct mac_slot
{
	uint16_t LogicalSenderMultiplexID;
	uint16_t LogicalSenderSlot;

	uint16_t SlotDuration;          /**< in unit of macroticks */
	uint16_t PSPDuration;           /**< in unit of macroticks */
	uint16_t TransmissionDuration;  /**< in unit of macroticks */
	
	uint16_t DelayCorrectionTerms0; /**< delay correction term for channel 0, in unit of ns */
	uint16_t DelayCorrectionTerms1; /**< delay correction term for channel 1, in unit of ns */

	uint8_t  AppDataLength;
	uint8_t  RESV0;

	uint32_t CNIAddressOffset;     /**< offset relative to CNI base addr */


	uint8_t  FlagPosition;          /**< flag position in membership vector of e current sending node*/
	uint8_t  SlotFlags;
	uint16_t AtTime;               /* the time for action in unit of macrotick relative to the start of round slot*/
} __PACK RoundSlotProperty_t;


#define SlotFlags_FrameTypeExplicit      ((uint8_t)0x01)
#define SlotFlags_ModeChangePermission   ((uint8_t)0x02)
#define SlotFlags_ReintegrationAllowed   ((uint8_t)0x04)
#define SlotFlags_ClockSynchronization   ((uint8_t)0x08)
#define SlotFlags_SynchronizationFrame   ((uint8_t)0x10)

#define SENDING_FRAME 					 1
#define RECVING_FRAME                    0

/**
 * Get the round slot of the current slot.
 * @return  the round slot entry.
 */
RoundSlotProperty_t* MAC_GetRoundSlotProperties(void);

/**
 * Load the slot configuration parameters from the MEDL.
 * @param  mode the current mode
 * @param  RoundSlot the current slot
 * @return      the slot configuration parameters entry.
 * @attention   ensure that and the slot are updated at the start of PSP before
 *              calling this function.
 */
RoundSlotProperty_t* MAC_LoadSlotProperties(uint32_t mode,uint32_t RoundSlot);

/**
 * Attention that SETTER functions can only be called in initialization.
 */
uint32_t  MAC_GetClusterScheduleID(void);
uint32_t  MAC_GetAppID(void);

/**
 * The properties below are specified for the node at initialization phase
 */
typedef struct mac_node
{
	uint16_t LogicalNameMultiplexedID;
	uint16_t LogicalNameSlotPosition;
	uint8_t NodeFlags;
	uint8_t FlagPosition;
	uint16_t SendDelay;
} __PACK NodeProperty_t;

#define NodeFlags_PermanentPassive        ((uint8_t)0x01)
#define NodeFlags_MultiplexedMembership   ((uint8_t)0x02)

NodeProperty_t* MAC_GetNodeProperties(void);

uint32_t  	MAC_IsPassiveNode(void);
uint32_t 	MAC_IsMultiplexedMembershipNode(void);

/**
 * the parameters below are used for the basic communication behavior of a node 
 * and are necessary to integrate or startup a cluster, which are specified at
 * initialization phase.
 */

typedef struct mac_schedule
{
	uint8_t ScheduleFlags;

	uint8_t MinimumIntegrationCount;
	uint8_t MaximumColdStartEntry;
	uint8_t MaximumMembershipFailureCount;

	uint32_t MacrotickParameter; // in unit of ns
	uint32_t Precision;          // in uint of ns
	uint32_t ArrivalTimingWindow;// in unit of ns

	uint32_t StartupTimeout;     // in unit of macrotick
	uint32_t ListenTimeout;      // in unit of macrotick
	uint32_t ColdStartTimeout;   // in unit of macrotick
} __PACK ScheduleParameter_t;

#define ScheduleFlags_ColdStartAllowed             ((uint8_t)0x01)
#define ScheduleFlags_ColdStartIntegrationAllowed  ((uint8_t)0x02)
#define ScheduleFlags_ExternalCorrectionAllowed    ((uint8_t)0x04)

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

/**
 * This function returns the ratio of Macrotick to Microtick.
 *
 * @attention The integral multiple shall be guaranteed by the hardware. If not, the hardware 
 * shall not be allowed allowed/downloaded the corresponding MEDL 
 * 
 * @return the ratio of Macrotick to Microtick
 */
uint32_t  MAC_GetRatio(void);

/**@}*/// end of group TTP_MAC_SLOT_PARAMETERS_CONTROL

/**
 * @defgroup TTP_MAC_SLOT_CONTROL
 */
/**@{*/
/******************************* slot control interface **************************/
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

#define NORMAL_SLOT							0
#define LAST_SLOT_OF_CURRENT_TDMAROUND 		1
#define FIRST_SLOT_OF_SUCCESSOR_TDMAROUND	2
#define LAST_SLOT_OF_CURRENT_CLUSTER 		3
#define FIRST_SLOT_OF_CURRENT_CLUSTER   	4

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

/**
 * Set the status of the current slot, which is executed during acknowledgment stage.
 * @param  SlotStatus the slot status
 * @return            non
 */
void    MAC_SetSlotStatus(uint32_t SlotStatus);

/** 
 * mark whether a frame should be transmitted in this slot.
 * @param  SlotAcquisition SENDING_FRAME or RECEIVING_FRAME
 * @return                 non
 */
void  	MAC_SetSlotAcquisition(uint32_t SlotAcquisition);
uint32_t MAC_GetSlotAcquisition(void);

/**
 * Set the time properties of the slot.
 * @param  AT    the actual trigger point time for TP phase of the MEDL
 * @param  TP    the time duration of TP phase in unit of macrotick
 * @param  SD    the slot duration time for a slot in unit of macrotick
 * @param  PSP   the psp time duration of the slot in unit if macrotick
 * @param  delay the delayed Action Time
 * @return       non
 */
void     MAC_SetSlotTime(uint32_t AT,uint32_t TP, uint32_t PSP,uint32_t SD,uint32_t Delay);

uint32_t  MAC_GetSlotStartMacroticks(void);
uint32_t  MAC_GetSlotStartMicroticks(void);
uint32_t  MAC_GetATMicroticks(void);

/**@}*/// end of group TTP_MAC_SLOT_CONTROL

/**************************** mac control interface *********************************/

void      MAC_StartTransmit(void);
void      MAC_StopTransmit(void);

void      MAC_StartReceive(void);
void      MAC_StopReceive(void);

/* The functions below works on the TDMA circulation but do not affect the the clock */
void MAC_StartPhaseCirculation(void);
void MAC_StopPhaseCirculation(void);

/** clock adjust mode options*/
#define CLK_PHASE_ADJ           (uint16_t)0x0001
#define CLK_FREQ_ADJ            (uint16_t)0x0002

/**
 * Set the start time of the phase circulation, indicating the start macrotick point of the periodic
 * TDMA round.
 * @param CycleStartTime   the start macrotick time of the cluster cycle
 * @param TDMAStartOffset  the start offset of the TDMA round in the cluster cycle
 * @return                 non
 */
void MAC_SetPhaseCycleStartPoint(uint32_t CycleStartTime, uint32_t TDMAStartOffset /* 0 always for current version */);

/**
 * Offset adjusting for local clock
 * @param AdjMode the adjust mode, CLK_PHASE_ADJ or CLK_FREQ_ADJ
 * @param Offset  the clock offset, in unit of signed integer of microtick
 */
void MAC_AdjTime(uint16_t AdjMode, int16_t Offset);

/**@}*/// end of group TTP_MAC_ACCESS_CONTROL
#endif 
