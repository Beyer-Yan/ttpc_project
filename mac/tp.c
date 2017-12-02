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
#include "ttpmac.h"
#include "ttpdebug.h"
#include "protocol_data.h"
#include "protocol.h"
#include "led.h"
#include "ttpservice.h"
#include "clock.h"

//in unit of microticks
static volatile uint32_t _G_ATStartMicrotickTime = 0;

void tp(void)
{
    uint32_t slot_acquisition;

    _G_ATStartMicrotickTime = CLOCK_GetCurMicrotick();
    slot_acquisition = MAC_GetSlotAcquisition();

    CNI_UpdateCLFS();

	slot_acquisition==SENDING_FRAME ? MAC_StartTransmit() : MAC_StartReceive();
    slot_acquisition==SENDING_FRAME ? LED_On(LED_TX) : LED_Off(LED_TX);
	SVC_RaiseATSynchronousInterrupt();
}

uint32_t  MAC_GetATMicroticks(void)
{
    return _G_ATStartMicrotickTime;
}