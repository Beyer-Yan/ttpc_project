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

static __INLINE uint32_t _check_clique(void)
{
	uint32_t res        = 0;
	uint32_t clique_res = 0;

	/**  clique detecting */
	clique_res = SVC_CliqueDetect();

	switch(clique_res)
	{
		case CLIQUE_MINORITY:
			FSM_sendEvent(FSM_EVENT_CLIQUE_MINORITY);
			CNI_SetSRBit(SR_CE);
			break;
		case CLIQUE_NO_ACTIVITY:
			FSM_sendEvent(FSM_EVENT_COMMUNICATION_BLACKOUT);
			CNI_SetSRBit(SR_CB);
			break;
		case CLIQUE_MAJORITY:
			FSM_sendEvent(FSM_EVENT_CLIQUE_MAJORITY);
			res = 1;
		case default:
			break;
	}

	PV_ClrCounter(AGREED_SLOTS_COUNTER);
	PV_ClrCounter(FAILED_SLOTS_COUNTER);
	
	return res;
}

static __INLINE void _prepare_for_receive(void)
{
	RoundSlotProperty_t *pRS = MAC_GetRoundSlotProperties();

	uint32_t mai = MAC_GetMacrotickParameter();

	if(pRS->SynchronizationFrame == SYN_FRAME)
	{
		uint32_t est_interval = pRS->DelayCorrectionTerms + 2*mai;
		SVC_SetEstimateArivalTimeInterval(es_time);
	}

	MAC_SetTime(pRS->AtTime,pRS->TransmissionDuration,pRS->SlotDuration); 

	MAC_SetSlotAcquisition(RECEIVING_FRAME);
}

static __INLINE void _prepare_for_transmit(void)
{
	RoundSlotProperty_t *pRS = MAC_GetRoundSlotProperties();

	uint32_t delay = pRS->SendDelay;

	/**
	 * A sending node shall perceive itself as fully operational in its PSP, and shall
	 * therefore set its membership flag in the membership vector and set its agreed 
	 * slot counter to 1.
	 * @see AS6003, Page 34, Line -7.
	 */
	CS_SetMemberBit(pRS->FlagPosition);
	PV_SetCounter(AGREED_SLOTS_COUNTER,1);

	MAC_SetTime(pRS->AtTime + delay,pRS->TransmissionDuration,pRS->SlotDuration);
	MAC_PushFrame();
	MAC_SetSlotAcquisition(SENDING_FRAME);

	/** init the ack state, only if the node plans to send in this slot */
	PV_SetAckState(ACK_INIT);
}

static __INLINE uint32_t _is_data_frame()
{
	// the legality of the slot configuration shall be checked upper application
	RoundSlotProperty_t* pRS = MAC_GetRoundSlotProperties();

    return ((pRS->FrameType==FRAME_TYPE_IMPLICIT)||(pSlot->AppDataLength)? 1 : 0 );
}

void psp_for_passive(void)
{

	uint32_t res = 0;
	RoundSlotProperty_t *pRS = MAC_GetRoundSlotProperties();

	MAC_UpdateSlot();

	/** check MEDL cofiguration */
	#warning "periodic checking for MEDL has not been implemented"

	if(MAC_IsFirstSLotOfCluster())
	{
		update_mode();
	}
	_load_slot_configuration();

	if(MAC_IsOwnNodeSlot())
	{
		if(!_check_clique()) return ;
	}

	if(MAC_IsSendSlot())
	{
		if( SVC_SlotAcquirement() )
		{
			if(CNI_IsModeChangeRequsted())
			{
				if(pRS->ModeChangePermission==MODE_CHANGE_DENY)
				{
					CNI_SetSRBit(SR_MV);
					FSM_sendEvent(FSM_EVENT_MODE_VIOLATION_ERR);
					res = 0;
					goto _end;
				}
				else
				{
					CS_SetMemberBit(pRS->FlagPosition); /**< now, membership acquired */
				}
			}
			res = 1; /**< actually, the controller transmitted into activa state  */
			FSM_sendEvent(FSM_EVENT_NODE_SLOT_ACQUIRED);
		}
	}

	_end:
	res ? _prepare_for_transmit() : _prepare_for_receive();
}

void psp_for_active(void)
{
	uint32_t res = 0;
	MAC_UpdateSlot();

	if(MAC_IsFirstSLotOfCluster())
	{
		update_mode();
	}
	_load_slot_configuration();

	if(MAC_IsOwnNodeSlot())
	{
		if(!_check_clique()) return ;
	}

	if(MAC_IsSendSlot())
	{

			if(SVC_CheckHostLifeSign())
			{
				//host life updated during active state
				FSM_sendEvent(FSM_EVENT_HOST_LIFE_UPDATED);
				//freeshot state
				PV_DisableFreeShot();
				res = 1;
			}
			else
			{
				FSM_sendEvent(FSM_EVENT_HOST_LIFE_NOT_UPDATED);
			}
	}

	RoundSlotProperty_t *pRS = MAC_GetRoundSlotProperties();
	//check the mode request field
	if(res!=0)
	{
		if(CNI_IsModeChangeRequsted())
		{
			if(pRS->ModeChangePermission==MODE_CHANGE_DENY)
			{
				CNI_SetSRBit(SR_MV);
				FSM_sendEvent(FSM_EVENT_MODE_VIOLATION_ERR);
				res = 0;
			}
		}
	}
	//check the whether the data is ready, if there is a data frame is to be send
	if(res!=0)
	{
		if(_is_data_frame())
		{
			if(!MSG_CheckMsgRF(pRS->CNIAddressOffset))
			{
				CNI_SetSRBit(SR_NR);
				res = 0;
			}
		}
	}

	res ? _prepare_for_transmit() : _prepare_for_receive();
}

void psp_for_coldstart(void)
{

}
 