/**
  ******************************************************************************
  * @file    	interrupt.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2017.12
  * @brief   	This file provides the interrupt unit related operation interfaces
  ******************************************************************************
  * @attention
  *
  * @desc
  ******************************************************************************
  */

#ifndef __INTERRUPT__H__
#define __INTERRUPT__H__

void INT_Enable(void);
void INT_Disable(void);

void INT_Pulse(void);

#endif // !__INTERRUPT__H__


