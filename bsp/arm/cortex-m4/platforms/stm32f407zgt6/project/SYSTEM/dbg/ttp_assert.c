/**
  ******************************************************************************
  * @file    	ttp_sssert.c
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.11.15
  * @brief   	the implementation of the TTP_ASSERT interface
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
  
#include "ttpdebug.h"
#include "led.h"

void __TTP_Assert(char* s)
{
    LED_On(LED_ERR);
	/** write the error text into the error buffer */
    INFO("\r\nSYSTM ERROR");
	ERROR(s);
	DBG_Flush();

	while(1);
}

void __TTP_Tguard(char* s)
{
    LED_On(LED_ERR);
	/** write the error text into the error buffer */
    INFO("\r\nTIMING ERROR");
	ERROR(s);
	DBG_Flush();

	while(1);
}
