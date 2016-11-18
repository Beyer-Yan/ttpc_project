/**
  ******************************************************************************
  * @file    	protocol.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.10
  * @brief   	This file provides the high level interfaces to the TTPC protocol.
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */	
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <ttpdef.h>

/**
 * @defgroup Protocol_Layer
 */	
/**@{*/


other module


////////////////////////////////////////////////////////////////////////////////
///state machine definitions                                                  //
////////////////////////////////////////////////////////////////////////////////

/**
 * @defgroup StateMachine_Transition_Num
 */
/**@{*/

/**
 * Standard transition number that standard TTPC shall include
 */

/** INIT -> LISTEN */
#define TRANSITION_NUM_1 		0 

/** LISTEN -> PASSIVE */		
#define TRANSITION_NUM_2 		1 

/** LISTEN -> COLD_START */
#define TRANSITION_NUM_3 		2

/** ACTIVE/PASSIVE -> FREEZE */ 	
#define TRANSITION_NUM_4 		3 

/** LISTEN/COLD_START/ACTIVE/PASSIVE/INIT/AWAIT/TEST/DOWNLOAD -> FREEZE */
#define TRANSITION_NUM_5 		4

/** COLD_START -> ACTIVE */
#define TRANSITION_NUM_6 		5

/** COLD_START -> LISTEN */
#define TRANSITION_NUM_7 		6

/** ACTIVE -> PASSIVE */
#define TRANSITION_NUM_8 		7

/** PASSIVE -> ACTIVE */
#define TRANSITION_NUM_9 		8

/** FREEZE -> INIT */
#define TRANSITION_NUM_10 		9

/** COLD_START -> PASSIVE */
#define TRANSITION_NUM_11 		10

/**
 * Implemetation specific transition number that TTPC should include
 */

/** FREEZE -> TEST */
#define TRANSITION_NUM_12 		11

/** FREEZE -> AWAIT */
#define TRANSITION_NUM_13 		12

/** TEST -> FREEZE */
#define TRANSITION_NUM_14 		13

/** LISTEN/AWAIT -> DOWNLOAD */
#define TRANSITION_NUM_15 		14

/** DOWNLOAD -> FREEZE */
#define TRANSITION_NUM_16 		15

/**@}*/// end of group StateMachine_Transition_Num

/**
 * 
 * @defgroup StateMachine_Events
 */
/**@{*/

/**
 * to be coding !!
 */

#define INIT_OK

#define CORRECT_FRAME_RECEIVED

#define LISTEN_TIMEOUT_EXPIRED
#define COLD_START_ALLOWED
#define HOST_LIFE_UPDATED

#define CLIQUE_ERR
#define COMMUNICATION_BLACKOUT
#define SYNC_ERR
#define ACK_ERR
#define PERIODIC_MEDL_CRC_FAILED

#define CO_ON
#define INIT_ERR

#define MAJORITY_CLIQUE
#define LEAST_2_CONTROLLERS_ALIVE

#define MINORITY_CLIQUE
#define MAX_COLD_START_ENTRIES_EXCEEDED
#define TRAFFIC_DETECT_DURING_STARTUP_TIMEOUT

#define MODE_VIOLATION_ERR
#define ACK_FAILED

#define NODE_SLOT_ACQUIRED

#define STATUS_FIELD_RESET
#define CONTROLLER_ID_UPDATED

#define BIST_SET
#define CA_SET
#define TEST_OK
#define FRAME_INDICATING_DOWNLOAD_MODE
#define DOWNLOAD_OK

#define CO_OFF
#define HOST_LIFE_NOT_UPDATED
#define CA_CLR
#define BIST_CLR

/**@}*/// end of group StateMachine_Events


typedef struct FSM_State 
{
	/**
	 * the state number of current state. This value
	 * is used to update the PS field of the status_field
	 * of the CNI, also, is used to record the current 
	 * protocol state number for transiting.
	 */
	uint32_t state_num;
	/**
	 * preparations to do for state machine when 
	 * transiting to this state.
	 */
	void (*toState)(void); 

	/**
	 * proceedings to do for state machine
	 * in this state.
	 */
	void (*doState)(void);
}FSM_State;

/** send a event or events to state machine */
void FSM_start(void);

/** DO NOT CALL THIS FUNCTION IN AN ISR !! */
void FSM_reset(void);

void FSM_setHook(void (*hook)(uint32_t ps));
void FSM_sendEvent(uint32_t Event);

uint32_t FSM_getCurState(void);

/**
 * @defgroup Protocol_Processor
 */
/**@{*/
/** toState */
extern void FSM_toFreeze(void);
extern void FSM_toInit(void);
extern void FSM_toListen(void);
extern void FSM_toColdStart(void);
extern void FSM_toActive(void);
extern void FSM_toPassive(void);
extern void FSM_toAwait(void);
extern void FSM_toTest(void);
extern void FSM_toDownload(void);

/** doState */
extern void FSM_doFreeze(void);
extern void FSM_doInit(void);
extern void FSM_doListen(void);
extern void FSM_doColdStart(void);
extern void FSM_doActive(void);
extern void FSM_doPassive(void);
extern void FSM_doAwait(void);
extern void FSM_doTest(void);
extern void FSM_doDownload(void);

/**@}*/// end of group Protocol_Processor

/**@}*/// end of group Protocol_Layer
	

#endif