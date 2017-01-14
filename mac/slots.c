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
  * PASSIVE state. However, one TDMA scheme will be processed while the controller
  * is in cold start state, which is not instantiated here.
  * 
  ******************************************************************************
  */	
#include "ttpc_mac.h"
#include "virhw.h"
#include "protocol_data.h"
#include "ttpdebug.h"
#include "medl.h"



/*********************************************************************************/
/**
 * the global variables definitoions, slot relative.
 */
/** slots partition parameters */
static uint32_t _G_ClusterCycles;
static uint32_t _G_TDMASlots;

/**
 * slots parameters, different for different slots. 
 * Attention that round_slots = _G_TDMARound * _G_TDMASlots + _G_Slot
 */
/** slots assignment */
static uint32_t _G_Slot        = 0;
static uint32_t _G_TDMARound   = 0;

/** slots timing parameters */
static uint32_t _G_ActualAT; 	/**< actual action time in unit of macrotick */
static uint32_t _G_PRP;			/**< prp start time in unit of macrotick */

/** status of a slot, which is used for the clique detection */
static uint8_t  _G_SlotStatus;

/** SENDING_FRAME or RECEIVING_FRAME */
static uint8_t _G_SlotAcquisitionFlag;

/*********************************************************************************/
/**
 * slots controll functions
 */

uint32_t  MAC_IsSendSlot(void)
{
	NodeProperty_t *pNP;
	RoundSlotProperty_t *pRP;
	uint32_t   ss   = 0;  /**< sending slot */
	TTP_ASSERT(pRS!=NULL);

	pNP = MAC_GetNodeProperties();
	pRP = MAC_GetRoundSlotProperties();

	#define CHK() ((pNP->LogicalNameSlotPosition==pRS->LogicalSenderSlot)&&
					(pNP->LogicalNameMulplexedID==0?1:pNP->LogicalNameMulplexedID==pRS->LogicalSenderMultiplexID))
	/** ensure that the slot has been updated */
	ss = CHK();
	#undef CHK

	return ss;
}


static __INLINE _calc_mode_num(uint32_t mode)
{
	uint32_t mode_num;

	switch(mode)
	{
		case MODE_1:mode_num = 0;
		case MODE_2:mode_num = 1;
		case MODE_3:mode_num = 2;
		case default:mode_num= 0;
	}
	return mode_num;
}


RoundSlotProperty_t* MAC_LoadSlotProperties(uint32_t mode,uint32_t tdma,uint32_t slot)
{

	uint32_t mode_num;
	return MEDL_GetRoundSlotAddr(mode_num,tdma,slot);
}

uint32_t MAC_UpdateSlot(void)
{
	uint32_t rs = CS_GetCurRoundSlot();
	uint32_t res;


	if((rs+1)==_G_ClusterCycles)
	{
		//points the first slot of a cluster cycle
		rs = 0;
		_G_TDMARound = 0;
		_G_Slot      = 0;
		res = FIRST_SLOT_OF_CURRENT_CLUSTER;
	}
	else if((rs+1)%_G_TDMASlots==_G_TDMASlots-1)
	{
		//points the last slot of a TDMA round
		rs++;
		_G_TDMARound++;
		_G_Slot++;
		res = LAST_SLOT_OF_CURRENT_TDMA_ROUND;
	}
	else
	{
		//points the first slot of a TDMA round
		if((rs+1)%_G_TDMASlots==0)
		{
			_G_TDMARound++;
			_G_Slot=0;
			res = FIRST_SLOT_OF_SUCCESSOR_TDMA_ROUND;
		}
		else
		{
			_G_Slot++;
			res = NORMAL_SLOT;
		}
		rs++;
	}

	CS_SetRoundSlot(rs);
	return res;
}

RoundSlotProperty_t* MAC_GetRoundSlotProperties(void)
{
	uint32_t mode = CS_GetCurMode();

	mode = _calc_mode_num(mode);
	return MEDL_GetRoundSlotAddr(mode_num,_G_TDMARound,_G_Slot);
}

uint32_t MAC_GetSlotStatus(void)
{
	return _G_SlotStatus;
}

uint32_t MAC_GetNodeSlot(void)
{
	return _G_Slot;
}

uint32_t MAC_GetTDMARound(void)
{
	return _G_TDMARound;
}

uint32_t MAC_GetTDMASlots(void)
{
	return _G_TDMASlots;
}

uint32_t MAC_GetClusterCycles(void)
{
	return _G_ClusterCycles;
}

uint32_t MAC_GetPspTsmp(void)
{
	return TIM_GetCapture();
}

void MAC_SetSlotAcquisition(uint32_t SlotAcquisition)
{
	_G_SlotAcquisitionFlag = SlotAcquisition;
}

uint32_t MAC_GetSlotAcquisition(void)
{
	return _G_SlotAcquisitionFlag;
}

void MAC_SetSlotStatus(uint32_t SlotStatus)
{
	_G_SlotStatus = SlotStatus;
}