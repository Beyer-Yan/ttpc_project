/**
  ******************************************************************************
  * @file    	debug.c
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
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
#include "ttpdebug.h"
#include "ttpdef.h"
#include <stdio.h>
#include <stdarg.h>
#include "dbgio.h"
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

static int _G_Size = 0;

void __Message(int priority, const char * format, ...)
{
	va_list ap;
	va_start(ap, format);

	if (!format) {
		return;
	}
    
    char* buffAddr = IO_GetBaseAddr();
    
    if(priority != DBG_INFO)
        _G_Size += snprintf(buffAddr + _G_Size,IO_BUFFER_SIZE-_G_Size,"%d:",priority);
    
    _G_Size += vsnprintf(buffAddr + _G_Size,IO_BUFFER_SIZE-_G_Size,format,ap);
    buffAddr[_G_Size++] = '\r';
    buffAddr[_G_Size++] = '\n';
        
	va_end(ap);
}

void DBG_Flush(void)
{
	if(_G_Size!=0)
        IO_Flush(_G_Size);
    _G_Size = 0;
}
