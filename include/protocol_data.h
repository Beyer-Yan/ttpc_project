/**
  ******************************************************************************
  * @file    	protocol_data.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.12.20
  * @brief   	the necessary variables of data encpasulating for protocol executing
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
#ifndef __PROTOCOL_DATA_H__
	#define __PROTOCOL_DATA_H__
/*******************************************************************************/
/**
 * the namespace "PV_" means "Protocol variables". 
 */
/** ensure the 4-bytes alignment for the PV_data */
#include "ttpdef.h"
extern uint8_t PV_data[20];
/** counters */
#define PV_csc  	(PV_data[0])	/**< cold start counter */
#define PV_ic 		(PV_data[1])	/**< integration counter */
#define PV_asc 		(PV_data[2]) 	/**< agreed slots counter */
#define PV_fsc 		(PV_data[3]) 	/**< failed slots counter */
#define PV_mfc 		(PV_data[4]) 	/**< membership failed counter */

/** ack relatived */
#define PV_as 		(PV_data[5]) 	/**< ack state */

/** ack relatived */
#define PV_fsmp 	(PV_data[6])	/**< first successor membership position */

/** PV_data[7] is reserved */
/** addr offset of cni of first successor, 
 * PV_data[8] and PV_data[9] are occupied
 */
#define PV_fsaddr	((uint16_t*)(PV_data+8))[0]

/** necessary flags */
#define PV_bbf 		(PV_data[10]) 	/**< big bang flag */
#define PV_fsf 		(PV_data[11])	/**< free-shot flag */
#define PV_oc 		(PV_data[12]) 	/**< observed channel */

/*******************************************************************************/
/**
 * macro objects definition of counters
 */
#define COLD_START_COUNTER 			0
#define INTEGRATION_COUNTER 		1
#define AGREED_SLOTS_COUNTER 		2
#define FAILED_SLOTS_COUNTER 		3
#define MEMBERSHIP_FAILED_COUNTER 	4

/**
 * the operation interface definition of counters
 * @param counter
 *        @arg  COLD_START_COUNTER 		
 		  @arg	INTEGRATION_COUNTER 	
 		  @arg	AGREED_SLOTS_COUNTER 	
 		  @arg 	FALTED_SLOTS_COUNTER 	
 		  @arg	MEMBERSHIP_FAILED_COUNTER
 */
#define PV_SetCounter(counter,value)    (PV_data[counter]=value)
#define PV_IncCounter(counter)			(PV_data[counter]++)
#define PV_DecCounter(counter) 			(PV_data[counter]--)
#define PV_ClrCounter(counter) 			(PV_data[counter]=0)
#define PV_GetCounter(counter)			(PV_data[counter])

/** ack relatived state and operation definitions */
#define ACK_INIT 						0
#define WAIT_FIRST_SUCCESSOR 			1
#define WAIT_SECOND_SUCCESSOR 			2
#define ACK_FINISHED 					3
/** ack operations definitions */
#define PV_GetAckState() 				(PV_as) 
#define PV_SetAckState(state) 			(PV_as = state)
#define PV_ClrAckState() 				(PV_as = 0)

/** record the message address of the first successor  */
#define PV_SetFSAddr(addr) 				(PV_fsaddr = addr)
#define PV_GetFSAddr() 					(PV_fsaddr)
#define PV_SetFirstSuccessorMemPos(pos) (PV_fsmp = pos)
#define PV_GetFirstSuccessorMemPos() 	(PV_fsmp)

/** necessary flags relatived */
#define BIG_BANG_ENABLE 				0
#define BIG_BANG_DISABLE 				1
#define FREE_SHOT_ENABLE  				0
#define FREE_SHOT_DIsABLE				1

#define PV_GetBigBangFlag()				(PV_bbf)
#define PV_EnableBigBang()		 		(PV_bbf = BIG_BANG_ENABLE)
#define PV_DisableBigBang() 			(PV_bbf = BIG_BANG_DISABLE)

#define PV_GetFreeShotFlag() 			(PV_fsf)
#define PV_EnableFreeShot() 			(PV_fsf = FREE_SHOT_DISABLE)	
#define PV_DisableFreeShot() 			(PV_fsf = FREE_SHOT_ENABLE)	

/** observed channel  */
#define CH0                      		0
#define CH1                      		1

#define PV_GetObservedChannel() 		(PV_oc)
#define PV_SetObservedChannel(ch) 		(PV_oc = ch)	

#endif

