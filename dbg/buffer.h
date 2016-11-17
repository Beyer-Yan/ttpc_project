/**
  ******************************************************************************
  * @file    	buffer.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.8.21
  * @brief   	brief description about this file
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file aims to support the buffers that debug subsystem will use. The debug 
  * buffer is specified with some ring bufer regions that be assigned to different 
  * priorities and the contents of debug buffer are transimitted to the host asynchronously.
  * In a round slot of TTPC timing, special time area is designed for printing the debug 
  * contents to the host. So, in the design of the MEDL, the corresbonding time should be 
  * take in consideration in order that the TTPC controller has enough time to print all 
  * the debug information to the host.
  ******************************************************************************
  */
 #ifndef __BUFFER_H__
 #define __BUFFER_H__ 

#define INFO_SIZE			2048
#define ERROR_SIZE		1024

/**
 * The buffer will use the default buffer region as the debug buffer. The debug buffer is specified
 * in a paiticular memory region. All the interface of Seq will aceess the data of the specified region.
 * That is, all the memory space of the debug buffer is allocated statically.
 */
extern char __Info_buf[INFO_SIZE];
extern char __Error_buf[ERROR_SIZE];

/**
 * Exported functions
 */

/**
 * This function puts a char type elment to the info buffer.
 * @param  ch a char type element
 * @return    the char type element
 * @attention the "ch" will be deserted if the info buffer is full.
 */
int info_putc(int ch);

/**
 * This function puts a char type element to the error buffer.
 * @param  ch a char type element
 * @return    the char type element
 * @attention the "ch" will be deserted if the info buffer is full.
 */
int error_putc(int ch);

/**
 * This function will clear the info buffer.
 */
void info_clr(void);

/**
 * This function will clear the error buffer.
 */
void error_clr(void);

/**
 * This function will return the left space of the info buffer.
 * @return  the left space of the info buffer.
 */
int get_info_space(void);

/**
 * This function will return the left space of the error buffer.
 * @return  the left space of the error buffer.
 */
int get_error_space(void);

/**
 * This function will return the base address of the info buffer. 
 * @return the base address pointer of the info buffer.
 */
void *info_base_addr(void);

/**
 * This function will return the base address of the error buffer. 
 * @return the base address pointer of the error buffer.
 */
void *error_base_addr(void);


/**
 * These two functions will get current used size of the buffers.
 * @return  the used size of the buffers.
 */
int get_left_info_space(void);
int get_left_error_space(void);

/**
 * The macro definition of the "_base_addr" functions. If the corresbongding functions are
 * not implemented, then them will be expanded into macroes below.
 */
#define info_base_addr()		(__Info_buf)
#define error_base_addr() 	 	(__Error_buf)

 #endif