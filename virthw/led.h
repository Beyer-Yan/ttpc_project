/**
  ******************************************************************************
  * @file    	led.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.04
  * @brief   	led interfaces for debuging
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */

#ifndef __LED_H__
#define __LED_H__

/** led kind definition */
#define LED_SYNC        1
#define LED_ERR         2
#define LED_TX          3
#define LED_RX          4
#define LED_CV          5

#define IS_LED_KIND(led)  ((led)==LED_SYNC|(led)==LED_ERR|(led)==LED_TX|(led)==LED_RX|(led)==LED_CV)

/** Exported functions */
void LED_On(int ledKind);
void LED_Off(int ledKind);
void LED_Toggle(int ledKind);

void LED_Twinkle(int ledKind);

void LED_DepInit(void);

#endif 
