/**
 ******************************************************************************
 * @file    	cpu.c
 * @author  	Beyer
 * @email   	sinfare@foxmail.com
 * @version 	v1.0.0
 * @date    	20170331
 * @brief   	the implementation of the cpu.h interfaces
 ******************************************************************************
 * @attention
 *   
 * @log
 *   
 *
 ******************************************************************************
 */

#include <misc.h>
#include "cpu.h"
#include "core_cmFunc.h"

void CPU_InterruptEnable(void)
{
    __enable_irq(); 
}
void CPU_InterruptDisable(void)
{
    __disable_irq();
}

void CPU_SoftwareInterrupt(void)
{
    //todo;
}

void CPU_Sleep(void)
{
    __WFI();
}

void CPU_RegisterInterruptHandler(void (*func)(void))
{
    
}







