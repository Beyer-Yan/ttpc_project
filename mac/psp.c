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

static __INLINE void update_mode(void)
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

/**
 * PSP phase of TDMA circulation. 
 * If some error is happened during this circulation, this function will terminate
 * and return 0, otherwise return 1.
 * @return  0 some error happens
 *          1 function exits
 */
uint32_t psp(void)
{
	uint32_t res ;
	uint32_t clique_res;

	res = MAC_UpdateSlot();

	if(res == FIRST_SLOT_OF_CURRENT_CLUSTER)
	{
		update_mode();
	}

	/**
	 * ensure that this function shall be called after updating the slot and the 
	 * the c-state mode.
	 */
	uint32_t mode = CS_GetCurMode();
	uint32_t tdma = MAC_GetTDMARound();
	uint32_t slot = MAC_GetNodeSlot();

	//check then load the configuration of the current slot.
	RoundSlotProperty_t *pRS = MAC_LoadSlotProperties(mode,tdma,slot);

	if(res == FIRST_SLOT_OF_SUCCESSOR_TDMA_ROUND)
	{
		/**
		 * clique detecting 
		 */
		clique_res = SVC_CliqueDetect();
		if(clique_res != CLIQUE_MAJORITY)
		{
			switch(clique_res)
			{
				case CLIQUE_MINORITY:
					FSM_sendEvent(FSM_EVENT_CLIQUE_ERR);
					break;
				case CLIQUE_NO_ACTIVITY:
					FSM_sendEvent(FSM_EVENT_COMMUNICATION_BLACKOUT);
					break;
				case default:
					break;
			}
			return 0;
		}
	}

	if(MAC_IsSendSlot())
	{

	}


	
}
 	
 