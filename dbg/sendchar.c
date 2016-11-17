/**
  ******************************************************************************
  * @file    	sendchar.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.8.22
  * @brief   	brief implementation of sending interface in retartget.c
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file functions as a middle level between retarget.c and the real
  * sending interface. Different platforms have different sending interface, 
  * it is necessary to separate the real sending interface and the target. 
  ******************************************************************************
  */	
#include "buffer.h"

int send_info(int ch)
{
	return info_putc(ch);
}

int send_error(int ch)
{
	return error_putc(ch);
}