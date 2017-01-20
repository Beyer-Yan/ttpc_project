/**
  ******************************************************************************
  * @file    	tp.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.01.18
  * @brief   	the transmission phase process
  ******************************************************************************
  * @attention
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

static __INLINE void _raise_AT_synchronous_interrupt(void)
{

	#error "to be done";
}

/**
 * Update the global time field of the c-state.
 * At each action time, the cluster time field (CTF register) contains the same
 * value as the c-state time field. During the TDMA slot, the cluster time advances
 * in steps of macroticks, while the c-state time is only updated each AT time.
 */
static __INLINE void _tp_update_gt(void)
{
	uint32_t cluster_time = CNI_GetCTF();
	CS_SetGTF(cluster_time);
}

uint32_t tp(uint32_t ps)
{
	uint32_t slot_acquisition = MAC_GetSlotAcquisition();

	_tp_update_gt();

	slot_acquisition==SENDING_FRAME ? MAC_StartTransmit() : MAC_StartReceive();

	_raise_AT_synchronous_interrupt();

	return 1;
}