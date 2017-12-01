/**
  ******************************************************************************
  * @file    	bg.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2017.12
  * @brief   	This file provides the BG unit related operation interfaces
  ******************************************************************************
  * @attention
  *
  * @desc
  ******************************************************************************
  */
#ifndef __BG__H
#define __BG_H__
#include "ttpdef.h"
/**
 * @defgroup TTPC_WDG
 */
/**@{*/
void WDG_DepInit(void);
void WDG_SetWindowValue(uint8_t WindowValue);
void WDG_SetCounter(uint8_t Counter);
void WDG_Start(void);
void WDG_Stop(void);
void WDG_EnableIT(void);
void WDG_DisenableIT(void);     

/**@}*/// end of group TTPC_WDG

#endif // !__BG__H


