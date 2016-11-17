/**
  ******************************************************************************
  * @file    	state_machine.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.10
  * @brief   	The state machine of the TTPC
  ******************************************************************************
  * @attention
  *
  * @desc
  * The kernel of the TTPC protocol stack.
  * 
  ******************************************************************************
  */
#include "protocol.h"

#define MAX_TRANSITION_NUM 		16
#define MAX_STATE_NUM 			10

/**
 * This variable is used to record the events happend, mapping out the transition number
 * to be activated.
 */
static valotile uint32_t __G_event_bit_pattern = 0;

/**
 * This variable is used to record the permanent events, such as POWER_ON, AWAIT_CLR
 * TEST_CLR and so on. The permanent events happended will not be cleared if the 
 * corresbonding transition number is activated according to some events snapshot.  
 */
static valotile uint32_t __G_permanent_events = 0; 

/**
 * This variable is used to record the current state where the controller situates at
 * present.
 */
static const  FSM_State* valotile __G_cur_state = NULL;

/**
 * hook functions specified by users will be called when state transition is activated
 * @param       hook 		the pointer of the hook function.
 * @return      null
 */
static void (*__Gf_hook)(uint32_t ps) = NULL;


#define FSM_RUNNING		1
#define FSM_STOPPED		0
/**
 * flag recording the run state 
 * @arg FSM_RUNNING
 * @arg FSM_STOPPED
 */
static valotile char __G_start_flag = FSM_STOPPED;

/**
 * schedule table filling
 */
static const FSM_State __G_state[MAX_STATE_NUM] = 
{
	/* FREEZE  */ {.state_num = 0, .toState = , .doState = },
	/* INIT    */ {.state_num = 1, .toState = , .doState = },
	/* LISTEN  */ {.state_num = 2, .toState = , .doState = },
	/* COL_ST  */ {.state_num = 3, .toState = , .doState = },
	/* ACTIVE  */ {.state_num = 4, .toState = , .doState = },
	/* PASSIVE */ {.state_num = 5, .toState = , .doState = },
	/* AWAIT   */ {.state_num = 6, .toState = , .doState = },
	/* TEST    */ {.state_num = 7, .toState = , .doState = },
	/* DOWNL   */ {.state_num = 8, .toState = , .doState = },
	/* ERROR   */ {.state_num = 9, .toState = NULL, .doState = NULL}
};

/**
 * The macro definitions below are used to simplify the name spellings of the
 * state pointers and the state names.
 */
/** state pointers macro definitions*/
#define S_N 				NULL 				/**< for NULL pointer */
#define S_X 				&__G_state[9] 		/**< pointer to ERROR state */
#define S_0 				&__G_state[0] 		/**< pointer to FREEZE state */
#define S_1 				&__G_state[1] 		/**< pointer to INIT state */
#define S_2 				&__G_state[2] 		/**< pointer to LISTEN state */
#define S_3 				&__G_state[3] 		/**< pointer to COLD_START state */
#define S_4 				&__G_state[4] 		/**< pointer to ACTIVE state */
#define S_5 				&__G_state[5] 		/**< pointer to PASSIVE state */
#define S_6 				&__G_state[6] 		/**< pointer to AWAIT state */
#define S_7 				&__G_state[7] 		/**< pointer to TEST state */
#define S_8 				&__G_state[8] 		/**< pointer to DOWNLOAD state */

/** state names macro definitions*/
#define S_FREEZE 			S_0
#define S_INIT 				S_1
#define S_LISTEN 			S_2
#define S_COLD_START 		S_3
#define S_ACTIVE 			S_4
#define S_PASSIVE 			S_5
#define S_AWAIT 			S_6
#define S_TEST  			S_7
#define S_DOWNLOAD 			S_8
#define S_ERROR 			S_X
/**
		 *          0     1    2    3    4    5   6    7    8    9    10   11   12   13   14   15  
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 0 FREEZE | X  | X  | X  | X  | X  | X  | X  | X  | X  | 1  | X  | 7  | 6  | X  | X  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 1 INIT   | 2  | X  | X  | X  | 0  | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 2 LISTEN | X  | 5  | 3  | X  | 0  | X  | X  | X  | X  | X  | X  | X  | X  | X  | 8  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 3 COL_ST | X  | X  | X  | X  | 0  | 4  | 2  | X  | X  | X  | 5  | X  | X  | X  | X  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 4 ACTIVE | X  | X  | X  | 0  | 0  | X  | X  | 5  | X  | X  | X  | X  | X  | X  | X  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 5 PASSIV | X  | X  | X  | 0  | 0  | X  | X  | X  | 4  | X  | X  | X  | X  | X  | X  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 6 AWAIT  | X  | X  | X  | X  | 0  | X  | X  | X  | X  | X  | X  | X  | X  | X  | 8  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 7 TEST   | X  | X  | X  | X  | 0  | X  | X  | X  | X  | X  | X  | X  | X  | 0  | X  | X  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 8 DOWNL  | X  | X  | X  | X  | 0  | X  | X  | X  | X  | X  | X  | X  | X  | X  | X  | 0  |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		 X ERROR  | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA |
		          +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 */

static const FSM_State* const __G_transition_table[MAX_STATE_NUM][MAX_TRANSITION_NUM] = 
{

 /*                 0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15  */
 /*          */ 
 /* 0 FREEZE */  { S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_1, S_X, S_7, S_6, S_X, S_X, S_X  },
 /*          */ 
 /* 1 INIT   */  { S_2, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X  },
 /*          */
 /* 2 LISTEN */  { S_X, S_5, S_3, S_X, S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_8, S_X  },
 /*          */
 /* 3 COL_ST */  { S_X, S_X, S_X, S_X, S_0, S_4, S_2, S_X, S_X, S_X, S_5, S_X, S_X, S_X, S_X, S_X  },
 /*          */
 /* 4 ACTIVE */  { S_X, S_X, S_X, S_0, S_0, S_X, S_X, S_5, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X  },
 /*          */
 /* 5 PASSIV */  { S_X, S_X, S_X, S_0, S_0, S_X, S_X, S_X, S_4, S_X, S_X, S_X, S_X, S_X, S_X, S_X  },
 /*          */
 /* 6 AWAIT  */  { S_X, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_8, S_X  },
 /*          */
 /* 7 TEST   */  { S_X, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_0, S_X, S_X  },
 /*          */
 /* 8 DOWNL  */  { S_X, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_0  },
 /*          */
 /* X ERROR  */  { S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N  }
 /*          */
};

/**
 * Events bit pattern definitions
 */
/**
 *                                                                               
 +------+                            +------+                                 
 |  0   | INIT_OK                    |  16  | TRAFFIC_DETECT_DURING_STARTUP_TO
 +------+                            +------+                                 
 |  1   | CORRECT_FRAME_RECEIVED     |  17  | MODE_VIOLATION_ERR              
 +------+                            +------+                                 
 |  2   | LISTEN_TIMEOUT_EXPIRED     |  18  | ACK_FAILED                      
 +------+                            +------+                                 
 |  3   | COLD_START_ALLOWED         |  19  | NODE_SLOT_ACQUIRED              
 +------+                            +------+                                 
 |  4   | HOST_LIFE_UPDATED          |  20  | STATUS_FIELD_RESET              
 +------+                            +------+                                 
 |  5   | CLIQUE_ERR                 |  21  | CONTROLLER_ID_UPDATED           
 +------+                            +------+                                 
 |  6   | COMMUNICATION_BLACKOUT     |  22  | BIST_SET                        
 +------+                            +------+                                 
 |  7   | SYNC_ERR                   |  23  | CA_SET                          
 +------+                            +------+                                 
 |  8   | ACK_ERR                    |  24  | TEST_OK                         
 +------+                            +------+                                 
 |  9   | PERIODIC_MEDL_CRC_FAILED   |  25  | FRAME_INDICATING_DOWNL_MODE     
 +------+                            +------+                                 
 |  10  | CO_ON                      |  26  | DOWNL_OK                        
 +------+                            +------+                                 
 |  11  | INIT_ERR                   |  27  | CO_OFF                                
 +------+                            +------+                                 
 |  12  | MAJORITY_CLIQUE            |  28  | HOST_LIFE_NOT_UPDATED                                
 +------+                            +------+                                 
 |  13  | LEAST_2_CONTROLLER_ALIVE   |  29  | CA_CLR                                
 +------+                            +------+                                 
 |  14  | MINORITY_CLIQUE            |  30  | BIST_CLR                                
 +------+                            +------+                                 
 |  15  | MAX_COLD_START_ENTRIES_EX  |  31  | event type                                
 +------+                            +------+                                 

 */

/**
 * This function calculates the transition number according to the event bit
 * pattern.
 * @return  the transition number.
 * This parameter can be one of the following values:
 * 		@arg TRANSITION_NUM_1 
 * 		...
 * 		@arg TRANSITION_NUM_16
 * 		@arg -1   no transition number has been activated.
 */
static int __calc_transition_num(void)
{
	int transition_num = -1;

	/**
	 * claculation process here. If no corresbonding transition number is activated,
	 * the variable transition_num will set to -1. And if valid transition number
	 * is activated, the corresbonding bits of the transition number occupying in
	 * the events bits pattern shall be cleared.
	 */
	
	to be done.
	return transition_num;
}

/**
 * This function is used to run the state machine of the TTPC protocol. Attention that
 * the toState and the doState functions of a state shall be instantiated once for one
 * run-time of a state. Normally, the run-time protocol is a dead loop. When the the
 * program returns from the doState function of current state, the protocol will execute
 * state transition immediately, meaning that state functions may be executed duplicately
 * if no transition number is activated in this state of the state cycle. Be sure that your
 * doState function ensures that some transition number must be activated,  otherwise,
 * just sleep. When you choose the sleep mode, be sure that some appropriate and correct 
 * activation mode shall be set, making the state machine drop out from the doState.
 */
static void __protocol(void)
{

	int _x; 	/**< state number */
	int _y; 	/**< stransition number */

	/**
	 * record the state number of the current state for programing conciseness. It's
	 * not neccesary to record the variable because of the memory wasting.
	 */
	_x = __G_cur_state->state_num;

	while(__G_cur_state!=S_ERROR)
	{
		__G_cur_state->toState();
		__G_cur_state->doState();

		_y = __calc_transition_num();
		
		if(_y != -1)
		{
			__G_cur_state = __G_transition_table[_x][_y];

			if(__Gf_hook != NULL)
			{
				__Gf_hook(__G_cur_state->state_num);
			}
		}
	}
} 

__INLINE void __machine_reset()
{
	//__Gf_hook 		= NULL;
	__G_start_flag  = FSM_STOPPED;  
	__G_cur_state   = S_ERROR;

	__G_permanent_events  = 0;
	__G_event_bit_pattern = 0;
}

/**
 * This function sends events to the state machine. Events are specifies
 * by bits pattern, including their disjunction and conjunction operations.
 * @param Event the events to be send
 * This parameter can be one of the following values:
 * 		@arg INIT_OK
 * 		...
 * 		@see line 133 to ling 165 at this page.
 * 		...
 * 		@arg BIST_CLR
 * 	Attention that the most significant bit of the Event is the event type bit, 
 * 	bit 1 standing for the permanent event and bit 0 standing for the transient
 * 	event.
 */
void FSM_sendEvent(uint32_t Event)
{
	static const uint32_t type_mask = (uint32_t)0x80000000;

	if((Event & type_mask) == type_mask)
	{
		// the event happened is the permanent event
		__G_permanent_events |= (Event & (~type_mask));
	}
	__G_event_bit_pattern |= (Event|permanent_events);
}

/**
 * start the state machine. This function shall never be returned normally.
 */

void FSM_start()
{
	/** init the current state to be FREEZE */
	//__machine_reset();

	if(__G_start_flag == FSM_STOPPED)
	{
		__G_cur_state  = S_FREEZE;
		__G_start_flag = FSM_RUNNING;

		__protocol();
	}
}
/**
 * reset the state machine, mainly the pointers of the hook function and the 
 * current state variable.
 */
void FSM_reset()
{
	__machine_reset();
}

/**
 * set user hook function. Attention that the address alignment will not be 
 * checked. You shall ensure the correctness of the parameter when calling
 * the function.
 */
void FSM_setHook(void (*hook)(uint32_t ps)) 
{
	__Gf_hook = hook;
}

/**
 * return the state number of the current state
 * @return the current state number.
 */
uint32_t FSM_getCurState()
{
	return __G_cur_state->state_num;
}