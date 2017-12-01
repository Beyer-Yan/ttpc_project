/**
  ******************************************************************************
  * @file    	dbgio.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.8.21
  * @brief   	brief description about this file
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file aims to support the buffers that debug subsystem will use. The debug 
  * buffer is specified with some ring buffer regions that be assigned to different 
  * priorities and the contents of debug buffer are transmitted to the host asynchronously.
  * In a round slot of TTPC timing, special time area is designed for printing the debug 
  * contents to the host. So, in the design of the MEDL, the corresbonding time should be 
  * take in consideration in order that the TTPC controller has enough time to print all 
  * the debug information to the host.
  ******************************************************************************
  */
 #ifndef __DBGIO_H__
 #define __DBGIO_H__ 

#define IO_BUFFER_SIZE 16*1024

/**
 * The buffer will use the default buffer region as the debug buffer. The debug buffer is specified
 * in a paiticular memory region. All the interface of Seq will aceess the data of the specified region.
 * That is, all the memory space of the debug buffer is allocated statically.
 */
/**
 * Exported functions
 */
void IO_DepInit(void);

char* IO_GetBaseAddr(void);

void IO_Flush(int size);

#endif
