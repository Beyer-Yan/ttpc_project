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
 * @defgroup StateMachine_Transition_Num
 */
/**@{*/

/**
 * Standard transition number that standard TTPC shall include
 */

/** LISTEN/COLD_START/SUB_COLD_START/ACTIVE/PASSIVE/INIT/AWAIT/TEST/DOWNLOAD -> FREEZE */
#define TS1 		0 

/** INIT -> LISTEN */		
#define TS2 		1 

/** LISTEN -> PASSIVE */
#define TS3 		2

/** LISTEN -> COLD_START */ 	
#define TS4 		3 

/** ACTIVE/PASSIVE -> FREEZE */
#define TS5 		4

/** COLD_START -> ACTIVE */
#define TS6 		5

/** COLD_START -> LISTEN */
#define TS7 		6

/** ACTIVE -> PASSIVE */
#define TS8 		7

/** PASSIVE -> ACTIVE */
#define TS9 		8

/** FREEZE -> TEST */
#define TS10 		9

/** INIT -> FREEZE */
#define TS11 		10

 /** FREEZE -> AWAIT */
#define TS12		11

/** FREEZE -> INIT */
#define TS13 		12

/** TEST -> FREEZE */
#define TS14 		13

/** LISTEN/AWAIT -> DOWNLOAD */
#define TS15 		14

/** DOWNLOAD -> FREEZE */
#define TS16 		15

/** COLD_START -> SUB_COLD_START */
#define TS17 		16

/** SUB_COLD_START -> LISTEN */
#define TS18 		17

/** SUB_COLD_START -> COLD_START */
#define TS19 		18

/** COLD_START -> PASSIVE */
#define TS20 		19

/**@}*/// end of group StateMachine_Transition_Num

/**
 * 
 * @defgroup StateMachine_Events
 */
/**@{*/


#define FSM_EVENT_INIT_OK							((uint32_t)0x00000001)
#define FSM_EVENT_CSTATE_FRAME_RECEIVED				((uint32_t)0x00000002)
#define FSM_EVENT_LISTEN_TIMEOUT_EXPIRED			((uint32_t)0x00000004)
#define FSM_EVENT_COLD_START_ALLOWED				((uint32_t)0x00000008)
#define FSM_EVENT_HOST_LIFE_UPDATED					((uint32_t)0x00000010)
#define FSM_EVENT_CLIQUE_MINORITY					((uint32_t)0x00000020)
#define FSM_EVENT_COMMUNICATION_BLACKOUT			((uint32_t)0x00000040)
#define FSM_EVENT_SYNC_ERR							((uint32_t)0x00000080)
#define FSM_EVENT_ACK_ERR							((uint32_t)0x00000100)
#define FSM_EVENT_PERIODIC_MEDL_CRC_FAILED			((uint32_t)0x00000200)
#define FSM_EVENT_CO_OFF							((uint32_t)0x00000400)
#define FSM_EVENT_CLIQUE_MAJORITY					((uint32_t)0x00000800)
#define FSM_EVENT_LEAST_2_CONTROLLERS_ALIVE			((uint32_t)0x00001000)
#define FSM_EVENT_HOST_LIFE_NOT_UPDATED				((uint32_t)0x00002000)
#define FSM_EVENT_MODE_VIOLATION_ERR				((uint32_t)0x00004000)
#define FSM_EVENT_ACK_FAILED						((uint32_t)0x00008000)
#define FSM_EVENT_NODE_SLOT_ACQUIRED				((uint32_t)0x00010000)
#define FSM_EVENT_BIST_SET							((uint32_t)0x00020000)
#define FSM_EVENT_CO_ON								((uint32_t)0x00040000)
#define FSM_EVENT_INIT_ERR							((uint32_t)0x00080000)
#define FSM_EVENT_CA_SET							((uint32_t)0x00100000)
#define FSM_EVENT_BIST_CLR							((uint32_t)0x00200000)
#define FSM_EVENT_CA_CLR							((uint32_t)0x00400000)
#define FSM_EVENT_TEST_OK							((uint32_t)0x00800000)
#define FSM_EVENT_FRAME_DOWNLOAD					((uint32_t)0x01000000)
#define FSM_EVENT_DOWNLOAD_OK						((uint32_t)0x02000000)
#define FSM_EVENT_TRAFFIC_DETECT_DURING_STO			((uint32_t)0x04000000)
#define FSM_EVENT_MAX_COLD_START_ENTRIES_EXCEEDED	((uint32_t)0x08000000)
#define FSM_EVENT_RSV0								((uint32_t)0x00000000)
#define FSM_EVENT_RSV1								((uint32_t)0x00000000)
#define FSM_EVENT_RSV2								((uint32_t)0x00000000)
#define FSM_EVENT_RSV3								((uint32_t)0x00000000)

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