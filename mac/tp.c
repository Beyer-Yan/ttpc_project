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

#include "ttpservice.h"

//in unit of microticks
static volatile uint32_t _G_ATStartMicrotickTime = 0;

void tp(void)
{
    uint32_t slot_acquisition;

    _G_ATStartMicrotickTime = TIM_GetCurMicrotick();
    slot_acquisition = MAC_GetSlotAcquisition();

    CNI_UpdateCLFS();

	slot_acquisition==SENDING_FRAME ? MAC_StartTransmit() : MAC_StartReceive();

	SVC_RaiseATSynchronousInterrupt();
}

uint32_t  MAC_GetATMicroticks(void)
{
    return _G_ATStartMicrotickTime;
}