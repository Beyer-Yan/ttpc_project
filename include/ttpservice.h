/**
  ******************************************************************************
  * @file    	ttpservice.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.8
  * @brief   	brief description about this file
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file aims to provide the basic operation interface for byte processing. 
  * 
  ******************************************************************************
  */
 #ifndef __TTPSERVICE_H__
 #define __TTPSERVICE_H__

/**
 * @defgroup TTPSERVICE
 */
/*@{*/

/** memory align */

/**
 * @def TTP_ALIGN(size,align)
 * Return the most contiguous size aligned at specified width. For example, TTP_ALIGN(11,3)
 * will return 12.
 */
#define TTP_ALIGN(size,align)				(((size)+(align)-1)&~((align)-1))

/**
 * @def TTP_ALIGN_DOWN(size,align)
 * Return the down number of the specified aligned width. For example, TTP_ALIGN(11,3) will
 * return 9.
 */
#define TTP_ALIGN_DOWN(size,align)			((size)&~((align)-1))

/** MAX and MIN macro without jumps definition */
#define MIN(a,b) 							((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))
#define MAX(a,b) 							((a) ^ (((a)^(b)) & (((a) > (b)) - 1)))

/** ABS macro definition, remember that int32_t type is needed */
#define ABS(n)								((n) ^ ((n) >> 31)) - ((n) >> 31)

/**
 * If the root interrupt is closed, all the interruptions will be 
 * closed.
 */
void close_root_interrupt(void);
void open_root_interrupt(void);

/**
 * enforce the sleep of the ttp controller. When the ttp controller comes into 
 * the sleep mode, only the interrruption can activate the controller.
 */
void ttp_sleep(void);

/**
 * freeze the ttp controller, set the corresbonding error bits
 */
void ttp_freeze(void);

/**
 * @todo more utillties should be added here.
 */

/*@}*/


 #endif
