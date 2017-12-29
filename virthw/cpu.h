/**
  ******************************************************************************
  * @file    	cpu.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2017.12
  * @brief   	This file provides the cpu unit related operation interfaces
  ******************************************************************************
  * @attention
  *
  * @desc
  ******************************************************************************
  */

#ifndef __CPU__H__
#define __CPU__H__

void CPU_InterruptEnable(void);
void CPU_InterruptDisable(void);

/** produce a software interrupt to the host */
void CPU_SoftwareInterrupt(void);

/** sleep, will be awaked only by external awake line, 
 *  ex. the controller on signal
 */
void CPU_Sleep(void);

/** register the interrupt handler when the cpu is awaked by interruption,
 *  ex. the controller on event, or disrupted when the FSM is running, 
 *  ex. the controller on event.
 */
void CPU_RegisterInterruptHandler(void (*func)(void));

#endif // !__INTERRUPT__H__


