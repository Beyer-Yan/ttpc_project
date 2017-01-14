/**
  ******************************************************************************
  * @file    	ttpdebug.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.8
  * @brief   	some debug interface definitions
  ******************************************************************************
  * @attention
  * The timeing requirement should be satisfied in run-time debuging. 
  * @desc
  * This file defines the basic debug interfaces for printing the formated internal
  * status of TTP controller out. Remember that debuging information is printed from
  * the TTP ontroller to the host in specific communication hardware,such as USART,
  * Ethernet and so on. In special timing slot, the information will be "printed out".
  * 
  ******************************************************************************
  */
 
#ifndef __TTPDEBUG_H__
#define __TTPDEBUG_H__

/**
 * @defgroup TTP_Debug
 */
/**@{*/

/**
 * @name DEBUG level definition
 */
/**@{*/


#define DBG_EMERG				1
#define DBG_ALERT				2
#define DBG_CRIT				3
#define DBG_ERR					4
#define DBG_WARNING				5
 
#define DBG_NOTICE				6
#define DBG_INFO				7

#define IS_DBG_LV(LV)			(((LV)==DBG_EGERG) || \
								 ((LV)==DBG_ALERT) || \
								 ((LV)==DBG_CRIT)  || \
								 ((LV)==DBG_ERR)   || \
								 ((LV)==DBG_WARNING) || \
								 ((LV)==DBG_NOTICE)  || \
								 ((LV)==DBG_INFO))

/* end name group DEBUG level definition */
/**@}*/

/**
 * This function will flush the debug buffer out to the concrete target, such as
 * the host. Whenever this function is called, the flush operation will be 
 * fired and the transmission  between the host and the device is launched right
 * now. At the same time, the buffers are reset.
 * @attention This function is hardware-relative.
 */
void dbg_flush(void);

#ifdef  TTP_DEBUG
    extern void __Message(int priority,const char *format,...);
    #define message(lv,fm,...) 		__Message(lv,fm,##__VA_ARGS__)
#else
    #define message(lv,fm,...)      ((void)0)
#endif

/**
 * @name DEBUG level function definition
 */
/**@{*/
#define EMERGENCY(x, ...) 		message(DBG_EMERG, x, ##__VA_ARGS__)
#define ALERT(x, ...)     		message(DBG_ALERT, x, ##__VA_ARGS__)
#define CRITICAL(x, ...)  		message(DBG_CRIT, x, ##__VA_ARGS__)
#define ERROR(x, ...)  			message(DBG_ERR, x, ##__VA_ARGS__)
#define WARNING(x, ...)   		message(DBG_WARNING, x, ##__VA_ARGS__)
#define NOTICE(x, ...)    		message(DBG_NOTICE, x, ##__VA_ARGS__)

#define INFO(x, ...)   			message(DBG_INFO, x, ##__VA_ARGS__)

/* end name group DEBUG level function definition */
/**@}*/

/**
 * @defgroup TTP_assert
 */
/**@{*/

#ifndef TTP_DEBUG
    #define TTP_ASSERT(exp)     ((void)0)
#else
    extern void __TTP_Assert(char*);
    #define _STR(x)             _VAL(x)
    #define _VAL(x)             #x
    #define TTP_ASSERT(exp)     ((exp)?(void)0:\
                                __TTP_Assert(__FILE__":"_STR(__LINE__)":"#exp))
#endif

/**@}*/// end of group TTP_assert


/**@}*/


#endif