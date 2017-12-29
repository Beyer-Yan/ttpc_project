/**
 ******************************************************************************
 * @file    	test.c
 * @author  	Beyer
 * @email   	sinfare@foxmail.com
 * @version 	v1.0.0
 * @date    	20170331
 * @brief   	the file implements the virtual host interface
 ******************************************************************************
 * @attention
 *   
 * @log
 *   
 *
 ******************************************************************************
 */
#include "ttpdef.h"
#include "host.h"

void HOST_Init(void)
{
    TTP_HLSR = 1;
    TTP_CR0 &= CR_CO;
}

void HOST_Alive(void)
{
    TTP_HLSR = 1;
}