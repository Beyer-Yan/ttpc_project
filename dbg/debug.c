/**
  ******************************************************************************
  * @file    	debug.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.8.22
  * @brief   	the brief implementation of the debug system interface of TTPC
  ******************************************************************************
  * @attention
  *
  * @desc
  * The debug buffer is flushed from device to the host synchronized, which means
  * the debugging information will be buffered if debugging interfaces are called. 
  * The transmission from the device to the host will launched whenever the function
  * dbg_flush is called.
  * 
  ******************************************************************************
  */
#include <ttpdebug.h>
#include <stdio.h>
#include <stdarg.h>
#include "buffer.h"

/**
 *      Debug system layer structure
 * 
   +----------------------------------------------------------+  
   |                                                          |  
   |                 STANDARD LIBC LIBRARY                    |  
   |                                                          |  
   +--------------------------/-\-----------------------------+  
                              | |                                
                              | |                                
   +--------------------------\-/-----------------------------+  
   |                                                          |  
   |                 IO REDIRECT INTERFACE                    |  
   |                                                          |  
   +------------/-\------------------------/-\----------------+  
                | |                        | |                   
                | |                        | |                   
   +------------\-/-------------+          | |                   
   |                            |          | |                   
   |  PLATFORM-DEPENDENT BUFFER |          | |                   
   |                            |          | |                   
   +----------------------------+          | |                   
                                           | |                   
                                           | |                   
   +---------------------------------------\-/---------------+   
   |                                                         |   
   |                   IO INTERFACE LAYER                    |   
   |                                                         |   
   +----/-\------------/-\-------------/-\------------/-\----+   
        | |            | |             | |            | |        
        | |            | |             | |            | |        
   +----\-/----+  +----\-/----+   +----\-/----+  +----\-/----+   
   |           |  |           |   |           |  |           |   
   |    USB    |  |    ETH    |   |    SPI    |  |   OTHERS  |   
   |           |  |           |   |           |  |           |   
   +-----------+  +-----------+   +-----------+  +-----------+   
 */

/**
 * @brief This function should be hardware dependent.
 * @param buf  the base address of the buffer to be transmitted.
 * @param size the size to be transmitted.
 */
extern void real_flush(char *buf, int size);

void __Message(int priority, const char * format, ...)
{
	va_list ap;
	va_start(ap, format);

	if (!format) {
		return;
	}

	switch(priority)
	{
		case DBG_EGERG:
		case DBG_ALERT:
		case DBG_CRIT:
		case DBG_WARNING:
		case DBG_NOTICE:
			fprintf(stdout, "%d:",priority);
			vfprintf(stdout,format,ap);
			//fprintf(stdout, "\n");
			break;
		case DBG_ERR:
			vfprintf(stderr,format,ap);
			break;
    case DBG_INFO:
      vfprintf(stdout,format,ap);
      break;
		default:
			fprintf(stderr,"unk\n");
			break;
	}

	va_end(ap);

}

void dbg_flush(void)
{
	int size = 0;

	size = get_error_space();
	if(size>0)
	{
		real_flush(error_base_addr(),size);
		error_clr();
	}

	size = get_info_space();
	if(size>0)
	{
		real_flush(info_base_addr(),size);
		info_clr();
	}

}