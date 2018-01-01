/**
  ******************************************************************************
  * @file    	slots.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.12.27
  * @brief   	slot control
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * The TDMA slot phase will be enabled when the controller is in ACTIVE or
  * PASSIVE state. However, one TDMA scheme will be processed while the
  *controller
  * is in cold start state, which is not instantiated here.
  *
  ******************************************************************************
  */
#include "medl.h"
#include "protocol_data.h"
#include "ttpmac.h"
#include "ttpdebug.h"
#include "ttpservice.h"
#include "msg.h"
#include "clock.h"
#include "xfer.h"

/*********************************************************************************/
/**
 * the global variables definitions, slot relative.
 */
/** slots partition parameters */
// static volatile uint32_t _G_ClusterCycles;
// static volatile uint32_t _G_TDMASlots;

/**
 * slots parameters, different for different slots.
 * Attention that round_slots = _G_TDMARound * _G_TDMASlots + _G_Slot
 */
/** slots assignment */
static volatile uint32_t _G_Slot      = 0;
static volatile uint32_t _G_TDMARound = 0;

static uint32_t _G_ClusterCycleLength = 0;
static uint32_t _G_TDMACycleLength    = 0;

// /** slots timing parameters */
// static volatile uint32_t _G_ActualAT; /**< actual action time in unit of macrotick */
// static volatile uint32_t _G_PRP; /**< prp start time in unit of macrotick */

/** status of a slot, which is used for the clique detection */
static volatile uint8_t _G_SlotStatus;

/** SENDING_FRAME or RECEIVING_FRAME */
static volatile uint8_t _G_SlotAcquisitionFlag;

/** indicate the current updating status of the slot */
static volatile uint32_t _G_slot_pointer;

/*********************************************************************************/

uint32_t MAC_IsOwnNodeSlot(void)
{
    NodeProperty_t* pNP;
    RoundSlotProperty_t* pRS;

    pNP = MAC_GetNodeProperties();
    pRS = MAC_GetRoundSlotProperties();

    return ((pNP->LogicalNameSlotPosition == pRS->LogicalSenderSlot));
}

uint32_t MAC_IsSendSlot(void)
{
    NodeProperty_t* pNP;
    RoundSlotProperty_t* pRS;
    uint32_t ss = 0; /**< sending slot */

    pNP = MAC_GetNodeProperties();
    pRS = MAC_GetRoundSlotProperties();

    if(pNP->LogicalNameSlotPosition==pRS->LogicalSenderSlot){
        if(pNP->LogicalNameMultiplexedID!=0){
            if(pNP->LogicalNameMultiplexedID==pRS->LogicalSenderMultiplexID){
                ss = 1;
            }
        } else {
            ss = 1;
        }     
    }

    return ss;
}

uint32_t MAC_UpdateSlot(void)
{
    uint32_t rs = CS_GetCurRoundSlot();
    uint32_t res;

    rs = (rs+1)%_G_ClusterCycleLength;

    _G_TDMARound = rs/_G_TDMACycleLength;
    _G_Slot = rs%_G_TDMACycleLength;

    if(rs==0)
        res = FIRST_SLOT_OF_CURRENT_CLUSTER;
    else if(_G_Slot==0)
        res = FIRST_SLOT_OF_SUCCESSOR_TDMAROUND;
    else
        res = NORMAL_SLOT;
    
    /* 
    if(rs==0)
    {
        _G_TDMARound = 0;
        _G_Slot = 0;
    }
    else if(rs%_G_TDMACycleLength==0)

    if ((rs + 1) == _G_ClusterCycleLength) {
        // points the first slot of a cluster cycle
        rs = 0;
        _G_TDMARound = 0;
        _G_Slot = 0;
        res = FIRST_SLOT_OF_CURRENT_CLUSTER;
    } else if ((rs + 1) % _G_TDMACycleLength == _G_TDMACycleLength - 1) {
        // points the last slot of a TDMA round
        rs++;
        _G_TDMARound++;
        _G_Slot++;
        res = LAST_SLOT_OF_CURRENT_TDMAROUND;
    } else {
        // points the first slot of a TDMA round
        if ((rs + 1) % _G_TDMACycleLength == 0) {
            _G_TDMARound++;
            _G_Slot = 0;
            res = FIRST_SLOT_OF_SUCCESSOR_TDMAROUND;
        } else {
            _G_Slot++;
            res = NORMAL_SLOT;
        }
        rs++;
    }
    */

    CS_SetRoundSlot(rs);
    _G_slot_pointer = res;
    return res;
}

uint32_t MAC_GetSlotStatus(void)
{ 
     return _G_SlotStatus; 
}

void MAC_SetSlotStatus(uint32_t SlotStatus)
{
    static volatile uint32_t tentative = 0;

    switch (SlotStatus) {
    case FRAME_CORRECT:
        PV_IncCounter(AGREED_SLOTS_COUNTER);
        if(tentative) { PV_IncCounter(FAILED_SLOTS_COUNTER); tentative = 0; }
        break;

    case FRAME_TENTATIVE:
        tentative = 1;
        break;

    case FRAME_MODE_VIOLATION:
#warning "Formal verification needed to be performed"
        // treated as a correct frame ??
        PV_IncCounter(AGREED_SLOTS_COUNTER);
        if(tentative) { PV_IncCounter(FAILED_SLOTS_COUNTER); tentative = 0; }
        break;

    case FRAME_INCORRECT:
        PV_IncCounter(FAILED_SLOTS_COUNTER);
        break;

    case FRAME_NULL:
        // the slot is not confidered for clique detection
        break;

    case FRAME_INVALID:
        PV_IncCounter(FAILED_SLOTS_COUNTER);
        break;
    default :
        break;
    }
    _G_SlotStatus = SlotStatus;
}

/** setter implementation */
void MAC_SetSlot(uint32_t Slot)
{
     _G_Slot = Slot; 
}
void MAC_SetTDMARound(uint32_t tdma)
{ 
    _G_TDMARound = tdma;
}
void MAC_SetClusterCycleLength(uint32_t Length) 
{
    _G_ClusterCycleLength = Length;
}
void MAC_SetTDMACycleLength(uint32_t Length) 
{ 
    _G_TDMACycleLength = Length; 
}
void MAC_SetSlotAcquisition(uint32_t SlotAcquisition)
{ 
    _G_SlotAcquisitionFlag = SlotAcquisition;
}

/** getter implementation */
uint32_t MAC_GetNodeSlot(void) 
{ 
    return _G_Slot; 
}
uint32_t MAC_GetTDMARound(void) 
{ 
    return _G_TDMARound; 
}

uint32_t MAC_GetSlotAcquisition(void) 
{ 
    return _G_SlotAcquisitionFlag; 
}
uint32_t MAC_GetRoundSlot(void) 
{ 
    return (_G_TDMARound * _G_TDMACycleLength + _G_Slot);
}

uint32_t MAC_CheckSlot(void)
{
    return _G_slot_pointer;
}

void MAC_SetSlotTime(uint32_t ActAT, uint32_t TP,uint32_t PSP, uint32_t SD, uint32_t delay)
{
    ActAT = ActAT & 0xffff;
    uint16_t real_at =  ActAT + (delay & 0xffff);
    uint16_t real_prp = ActAT + (TP & 0xffff);
    uint16_t slot_end = ActAT + (SD & 0xffff) - PSP;

    INFO("--PREDISIGN TIMING--");
    INFO("AT :%u",real_at);
    INFO("PRP:%u",real_prp);
    INFO("END:%u",slot_end);
    CLOCK_SetTriggerAT(real_at);
    CLOCK_SetTriggerPRP(real_prp);
    CLOCK_SetTriggerSlotEnd(slot_end);
}


