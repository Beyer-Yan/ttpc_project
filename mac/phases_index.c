/**
  ******************************************************************************
  * @file    	phases_index.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.03
  * @brief   	define the phase index of the three-phases-circulation of MAC
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
 
#include <ttpdef.h>



/**
 * 0 for PSP, 1 for TP, 2 for PRP
 */
volatile uint32_t phase_indicator = 0;