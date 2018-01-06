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
static volatile uint32_t _G_ATStartMicrotickTime __SECTION("PV_SECTION") = 0;

void tp(void)
{
    uint32_t slot_acquisition;

    _G_ATStartMicrotickTime = CLOCK_GetCurMicrotick();
    
    uint32_t x = CLOCK_GetCurMacrotick();
    
    slot_acquisition = MAC_GetSlotAcquisition();

	if(slot_acquisition==SENDING_FRAME){
        MAC_StartTransmit();
    }
        
    else
        MAC_StartReceive();

    CNI_UpdateCLFS();
    
    //uint32_t t1 = test_x_function(_G_ATStartMicrotickTime);
    //uint32_t t2 = CLOCK_GetCaptureRX1();
    
    //uint32_t diff1 = t1;
    //uint32_t diff2 = t2 - _G_ATStartMicrotickTime;
    
    //uint32_t total = diff1+diff2;
    
    if(slot_acquisition==SENDING_FRAME)
        LED_On(LED_TX);
    else
        LED_Off(LED_TX);
    
    //INFO("diff:%u,%u,%u",diff1,diff2,total);
    //if(slot_acquisition==SENDING_FRAME)
        //INFO("TP            -- TIME:%-6u   ----------SEND",x);
    //else
        //INFO("TP            -- TIME:%u",x);
    DBG_Flush();
    //other test interface shall be specified here.

	SVC_RaiseATSynchronousInterrupt();
}

uint32_t  MAC_GetATMicroticks(void)
{
    return _G_ATStartMicrotickTime;
}
