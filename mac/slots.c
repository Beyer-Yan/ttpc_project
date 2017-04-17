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
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "virhw.h"
#include "ttpservice.h"

#define FrMT 1
#define MAX_STEPS 200

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
static volatile uint32_t _G_Slot = 0;
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

static volatile uint32_t _G_slot_pointer;

/*********************************************************************************/
/**
 * slots controll functions
 */

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

RoundSlotProperty_t* MAC_LoadSlotProperties(uint32_t mode, uint32_t tdma,
    uint32_t slot)
{

    uint32_t mode_num;

    mode_num = CALC_MODE_NUM(mode);

    return MEDL_GetRoundSlotAddr(mode_num, tdma, slot);
}

uint32_t MAC_UpdateSlot(void)
{
    uint32_t rs = CS_GetCurRoundSlot();
    uint32_t res;

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

    CS_SetRoundSlot(rs);
    _G_slot_pointer = res;
    return res;
}

RoundSlotProperty_t* MAC_GetRoundSlotProperties(void)
{
    uint32_t mode = CS_GetCurMode();

    uint32_t mode_num = CALC_MODE_NUM(mode);
    return MEDL_GetRoundSlotAddr(mode_num, _G_TDMARound, _G_Slot);
}

uint32_t MAC_GetSlotStatus(void) { return _G_SlotStatus; }

void MAC_SetSlotStatus(uint32_t SlotStatus)
{
    static volatile uint32_t tentative = 0;

    switch (SlotStatus) {
    case FRAME_CORRECT:
        PV_IncCounter(AGREED_SLOTS_COUNTER);
        tentative ? PV_IncCounter(FAILED_SLOTS_COUNTER) : (void)0;
        tentative ? tentative = 0 : (void)0;
        break;

    case FRAME_TENTATIVE:
        tentative = 1;
        break;

    case FRAME_MODE_VIOLATION:
#warning "Formal verification needed to be performed"
        // treated as a correct frame ??
        PV_IncCounter(AGREED_SLOTS_COUNTER);
        tentative ? PV_IncCounter(FAILED_SLOTS_COUNTER) : (void)0;
        tentative ? tentative = 0 : (void)0;
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
void MAC_SetSlot(uint32_t Slot) { _G_Slot = Slot; }
void MAC_SetTDMARound(uint32_t tdma) { _G_TDMARound = tdma; }
void MAC_SetClusterCycleLength(uint32_t Length) { _G_TDMACycleLength = Length;}
void MAC_SetTDMACycleLength(uint32_t Length) { _G_TDMACycleLength = Length; }
void MAC_SetSlotAcquisition(uint32_t SlotAcquisition){ _G_SlotAcquisitionFlag = SlotAcquisition;}

/** getter implementation */
uint32_t MAC_GetNodeSlot(void) { return _G_Slot; }
uint32_t MAC_GetTDMARound(void) { return _G_TDMARound; }
uint32_t MAC_GetPspTsmp(void) { return TIM_GetCapturePSP(); }
uint32_t MAC_GetRatio() { return TIM_GetRatio(); }
uint32_t MAC_GetSlotAcquisition(void) { return _G_SlotAcquisitionFlag; }
uint32_t MAC_GetRoundSlot(void) { return (_G_TDMARound * _G_TDMACycleLength + _G_Slot);}

void MAC_SetTime(uint32_t ActAT, uint32_t TP, uint32_t SD)
{
    uint16_t stime = TIM_GetCaptureMacrotickPSP();
    uint16_t real_at = stime + ActAT & 0xffff;
    uint16_t real_prp = real_at + TP & 0xffff;
    uint16_t slot_end = stime + SD & 0xffff;

    TIM_SetTriggerAT(real_at);
    TIM_SetTriggerPRP(real_prp);
    TIM_SetTriggerUser0(slot_end);
}

void MAC_StartPhaseCirculation() { /*to be done*/ }

void MAC_StopPhaseCirculation(){ /*to be done*/ }

uint32_t MAC_CheckSlot(void)
{
    return _G_slot_pointer;
}

void MAC_AdjTime(uint16_t AdjMode, int16_t Offset, int16_t Steps)
{
    // AdjMode is set CLK_FREQ_ADJ forcedly

    TTP_ASSERT(Steps < MAX_STEPS);

    int32_t cor_val[MAX_STEPS] = { 0 };

    int32_t ratio = (int32_t)TIM_GetRatio();

    int i = 0;
    int j = 0;

    int _step = Steps / (FrMT + 1);

    int32_t quotient = Offset / _step;
    int32_t remain = Offset % _step;

    for (j = 0, i = 0; i < _step; i++, j += FrMT + 1) {
        cor_val[j] = quotient;
    }

    int32_t tmp = ABS(remain);
    for (i = 0, j = 0; i < tmp; i++, j += FrMT + 1) {
        cor_val[j] += remain > 0 ? 1 : -1;
    }

    for (i = 0; i < Steps; i++) {
        cor_val[i] += ratio;
    }

    TIM_RatioAdj(cor_val, Steps);
}
