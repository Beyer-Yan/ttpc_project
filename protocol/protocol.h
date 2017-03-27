/**
  ******************************************************************************
  * @file    	protocol.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
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

#define FSM_EVENT_INIT_OK

#define FSM_EVENT_CSTATE_FRAME_RECEIVED

#define FSM_EVENT_LISTEN_TIMEOUT_EXPIRED
#define FSM_EVENT_COLD_START_ALLOWED
#define FSM_EVENT_HOST_LIFE_UPDATED

#define FSM_EVENT_CLIQUE_ERR
#define FSM_EVENT_COMMUNICATION_BLACKOUT
#define FSM_EVENT_SYNC_ERR
#define FSM_EVENT_ACK_ERR
#define FSM_EVENT_PERIODIC_MEDL_CRC_FAILED

#define FSM_EVENT_CO_ON
#define FSM_EVENT_INIT_ERR

#define FSM_EVENT_CLIQUE_MAJORITY
#define FSM_EVENT_LEAST_2_CONTROLLERS_ALIVE

#define FSM_EVENT_CLIQUE_MINORITY
#define FSM_EVENT_MAX_COLD_START_ENTRIES_EXCEEDED
#define FSM_EVENT_TRAFFIC_DETECT_DURING_STARTUP_TIMEOUT

#define FSM_EVENT_MODE_VIOLATION_ERR
#define FSM_EVENT_ACK_FAILED

#define FSM_EVENT_NODE_SLOT_ACQUIRED

#define FSM_EVENT_STATUS_FIELD_RESET
#define FSM_EVENT_CONTROLLER_ID_UPDATED

#define FSM_EVENT_BIST_SET
#define FSM_EVENT_CA_SET
#define FSM_EVENT_TEST_OK
#define FSM_EVENT_FRAME_INDICATING_DOWNLOAD_MODE
#define FSM_EVENT_DOWNLOAD_OK

#define FSM_EVENT_CO_OFF
#define FSM_EVENT_HOST_LIFE_NOT_UPDATED
#define FSM_EVENT_CA_CLR
#define FSM_EVENT_BIST_CLR

/**@}*/// end of group StateMachine_Events

struct SubSeqRoutine
{
	volatile uint32_t* pIndicator;
	void (*func[3])(void);
};

struct Processor
{
	void (*doState)();
	struct SubSeqRoutine *pSubRoutine;
};

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
	struct Processor doStateProcessor;

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