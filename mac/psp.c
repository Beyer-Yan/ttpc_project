/**
  ******************************************************************************
  * @file    	psp.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.12
  * @brief   	the pre-send-phase process
  ******************************************************************************
  * @attention
  *
  * The Pre-Send-Phase(PSP) is an exclusive phase for the TDMA slot. When the
  * synchronization architecture is set up, TDMA phase will start to work. When
  * the controller is in ACTIVE or PASIVE state, the phase circulation will live.
  * In addition, in COLD START state, one slot phase transaction must be proceed,
  * which is not instantiated here.
  * 
  * @desc
  *
  * 
  ******************************************************************************
  */
 #include "ttpc_mac.h"
 #include "ttpdebug.h"
 #include "protocol_data.h"
 #include "protocol.h"
 #include "virhw.h"

/**
 * This function shall be called at the start time of a slot.
 */

static __INLINE void _update_mode(void)
{
	dmc = CS_GetCurDMC();
	if(dmc!=DMC_NO_REQ)
	{
		switch(dmc)
		{
			case DMC_MODE_1: mode = MODE_1;break;
			case DMC_MODE_2: mode = MODE_2;break;
			case DMC_MODE_3: mode = MODE_3;break;
			case default:break;
		}
		CS_SetMode(mode);
		CS_SetDMC(DMC_NO_REQ);
		PV_DisableFreeShot();
	}
}

static __INLINE void _load_slot_configuration(void)
{
	/**
	 * ensure that this function shall be called after updating the slot and the 
	 * the c-state mode.
	 */
	uint32_t mode = CS_GetCurMode();
	uint32_t tdma = MAC_GetTDMARound();
	uint32_t slot = MAC_GetNodeSlot();

	//check then load the configuration of the current slot.
	MAC_LoadSlotProperties(mode,tdma,slot);
}

static __INLINE void __process_clique_event(uint32_t clique_res)
{
	switch(clique_res)
	{
		case CLIQUE_MINORITY:
			FSM_sendEvent(FSM_EVENT_CLIQUE_ERR);
			break;
		case CLIQUE_NO_ACTIVITY:
			FSM_sendEvent(FSM_EVENT_COMMUNICATION_BLACKOUT);
			break;
		case CLIQUE_MAJORITY:
			FSM_sendEvent(FSM_EVENT_CLIQUE_MAJORITY);
		case default:
			break;
	}
}

static __INLINE uint32_t _check_clique(void)
{
	uint32_t res        = 0;
	uint32_t clique_res = 0;

	if(MAC_IsOwnNodeSlot())
	{
		/**  clique detecting */
		clique_res = SVC_CliqueDetect();

		if(clique_res==CLIQUE_MAJORITY)
			res = 1;
		__process_clique_event(clique_res);
	}

	PV_ClrCounter(AGREED_SLOTS_COUNTER);
	PV_ClrCounter(FAILED_SLOTS_COUNTER);
	
	if(res===0)
	{
		CNI_SetSRBit(SR_CE);
	}
	return res;
}

static __INLINE void _prepare_for_receive(void)
{

	MAC_SetTime(pRS->AtTime,pRS->TransmissionDuration,pRS->SlotDuration); 
	MAC_SetSlotAcquisition(RECEIVING_FRAME);
}

static __INLINE void _prepare_for_transmit(void)
{
	RoundSlotProperty_t pRS = MAC_GetRoundSlotProperties();

	uint32_t delay = pRS->SendDelay;

	MAC_SetTime(pRS->AtTime + delay,pRS->TransmissionDuration,pRS->SlotDuration);
	MAC_PushFrame();
	MAC_SetSlotAcquisition(SENDING_FRAME);
}

static __INLINE void _process_mcr(uint32_t mcr)
{
	uint32_t dmc = DMC_NO_REQ;
	switch(mcr)
	{
		case MCR_MODE_1: dmc = DMC_MODE_1; break;
		case MCR_MODE_2: dmc = DMC_MODE_2; break;
		case MCR_MODE_3: dmc = DMC_MODE_3; break;
		case default: break;
	}
	CS_SetDMC(dmc);
}

static __INLINE uint32_t _is_data_frame()
{
	// the legality of the slot configuration shall be checked upper application
	RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();
	uint32_t res = 0;
    (pRS->FrameType==FRAME_TYPE_IMPLICIT)||(pSlot->AppDataLength)?res=1:res=0;

    return res;
}
/**
 * PSP phase of TDMA circulation. 
 * If some error is happened during this circulation, this function will terminate
 * and return 0, otherwise return 1.
 * @return  0 some error happens
 *          1 function exits
 */
uint32_t psp(uint32_t ps)
{
	uint32_t res = 0;

	MAC_UpdateSlot();

	if(MAC_IsFirstSLotOfCluster())
	{
		update_mode();
	}

	_load_slot_configuration();
	_check_clique()? (void)0 : return 0;

	if(MAC_IsSendSlot())
	{
		if(ps==PS_PASSIVE){
			 res=SVC_SlotAcquirement();
			 if(res!=0){
			 	//slot acquired
			 	FSM_sendEvent(FSM_EVENT_NODE_SLOT_ACQUIRED|FSM_EVENT_HOST_LIFE_UPDATED);
			 	res = 1;
			 }
		}
		else
		{
			if(SVC_CheckHostLifeSign()){
				//host life updated during active state
				FSM_sendEvent(FSM_EVENT_HOST_LIFE_UPDATED);
				res = 1;
			}
			else if(FREE_SHOT_ENABLE == PV_GetFreeShotFlag()){
				//freeshot state
				PV_DisableFreeShot();
				res = 1;
			}
			else{
				FSM_sendEvent(FSM_EVENT_HOST_LIFE_NOT_UPDATED);
			}
		}
	}

	RoundSlotProperty_t *pRS = MAC_GetRoundSlotProperties();
	//check the mode request field
	if(res!=0)
	{
		if(CNI_IsModeChangeRequsted())
		{
			if(pRS->ModeChangePermission==MODE_CHANGE_DENY){
				CNI_SetSRBit(SR_MV);
				FSM_sendEvent(FSM_EVENT_MODE_VIOLATION_ERR);
				res = 0;
			}else{
				uint32_t mcr = CNI_GetCurMCR();
				_process_mcr(mcr);
				CNI_ClrMCR();
			}
		}
	}
	//check the whether the data is ready, if there is a data frame is to be send
	if(res!=0)
	{
		if(_is_data_frame())
		{
			if(!MSG_CheckMsgRF(pRS->CNIAddressOffset)){
				CNI_SetSRBit(SR_NR);
				res = 0;
			}
		}
	}

	res?_prepare_for_transmit() : _prepare_for_receive();

	return 1;
}
 	
 