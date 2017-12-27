/**
  ******************************************************************************
  * @file    	clique.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.03
  * @brief   	clique detection algorithm implementation
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */

#include "ttpmac.h"
#include "ttpdebug.h"
#include "protocol_data.h"
#include "ttpservice.h"

uint32_t SVC_CliqueDetect(void)
{
	uint32_t failed_slot_counter = PV_GetCounter(FAILED_SLOTS_COUNTER);
	uint32_t agreed_slot_counter = PV_GetCounter(AGREED_SLOTS_COUNTER);

	if(1>=(failed_slot_counter+agreed_slot_counter))
	{
	 	return CLIQUE_NO_ACTIVITY;
	}

	if(agreed_slot_counter>failed_slot_counter)
	{
	    return CLIQUE_MAJORITY;
	}
	else
	{
		return CLIQUE_MINORITY;
	}
}
