/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

#define STD_INFO	1
#define STD_ERROR	2

extern int  send_info(int ch);  /* in sendchar.c */
extern int 	send_error(int ch);	/* in sendchar.c */

struct __FILE { int handle; int type;}; 
FILE __stdout = {0,STD_INFO};
FILE __stdin;
FILE __stderr = {0,STD_ERROR};

int fputc(int ch, FILE *f) 
{
	if(f->type==STD_INFO){
		send_info(ch);
	}
	else if(f->type==STD_ERROR){
		send_error(ch);
	}
	return ch;
}

void __ttywrch(int ch)
{
	ch = ch;
}

void _sys_exit(int return_code) {
  while (1);    /* endless loop */
}
