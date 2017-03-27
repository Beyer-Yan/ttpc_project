/**
  ******************************************************************************
  * @file    	ttp_sssert.c
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.11.15
  * @brief   	the implementation of the TTP_ASSERT interface
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */	
 
#include "ttp_debug.h"
#include "ttpdef.h"
#include "ttpservice.h"

void __TTP_Assert(char* txt)
{
	//1.close the interruption
	//2.flush the stderr buf
	//3.puts the txt
	//4.puts the register file
	//5.terminate the processor
	
	/** flush the old text */
	dbg_flush();
	SVC_CloseRootInt();

	/** write the error text into the error buffer */
	INFO("-----------------------------------------\n");
	ERROR(txt);
	dbg_flush();

	/** puts the register file */
	while(1)
	{
		SVC_Sleep();
	}
}
