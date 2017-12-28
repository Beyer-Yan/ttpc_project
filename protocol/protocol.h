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

#include "ttpdef.h"

/**
 * @defgroup Protocol_Layer
 */	
/**@{*/

////////////////////////////////////////////////////////////////////////////////
///state machine definitions                                                  //
////////////////////////////////////////////////////////////////////////////////

/**
 * @defgroup StateMachine_State
 */
/**@{*/
/** state names macro definitions*/
#define MAX_STATE_NUM 	    11

//state definitions
#define FSM_FREEZE 			0
#define FSM_INIT 			1
#define FSM_LISTEN 			2
#define FSM_COLD_START 		3
#define FSM_SUB_CS 			4
#define FSM_ACTIVE 			5
#define FSM_PASSIVE 		6
#define FSM_AWAIT 			7
#define FSM_TEST  			8
#define FSM_DOWNLOAD 		9
#define FSM_ERROR 		    10

/**@}*/// end of group StateMachine_State

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

void FSM_TransitIntoState(uint32_t NextState);
void FSM_TransitIntoStateUrgent(uint32_t NextState);

/**
 * @defgroup Protocol_Processor
 */
/**@{*/
/** toState */
extern void FSM_toFreeze(void);
extern void FSM_toInit(void);
extern void FSM_toListen(void);
extern void FSM_toColdStart(void);
extern void FSM_toSubColdStart(void);
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
extern void FSM_doSubColdStart(void);
extern void FSM_doActive(void);
extern void FSM_doPassive(void);
extern void FSM_doAwait(void);
extern void FSM_doTest(void);
extern void FSM_doDownload(void);

/**@}*/// end of group Protocol_Processor

/**@}*/// end of group Protocol_Layer
	

#endif





