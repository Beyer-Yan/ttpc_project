/**
 ******************************************************************************
 * @file    	ttpc_startup.c
 * @author  	Beyer
 * @email   	sinfare@foxmail.com
 * @version 	v1.0.0
 * @date    	2017.04
 * @brief   	startup entry
 ******************************************************************************
 * @attention
 *   
 * @log
 *   
 *
 ******************************************************************************
 */
#include "protocol.h"

int main()
{
    //platform init
    FSM_reset();
    FSM_start(); // endless loop

    //never run here
    //error happend
    //error process routine
}