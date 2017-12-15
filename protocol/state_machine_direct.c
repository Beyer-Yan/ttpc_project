/**
  ******************************************************************************
  * @file    	state_machine.c
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
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
#include "ttpdebug.h"
#include "ttpservice.h"
#include "protocol.h"
/**
 * This variable is used to record the current state where the controller situates at
 * present.
 */
static  int volatile __G_cur_state = FSM_ERROR;

/**
 * This variable is used to record the urgent state, which occurs normally in interruption.
 * ex. the controller on and the controller off signal; 
 */
static  int volatile __G_urgent_state = FSM_ERROR; 

static int  volatile __G_state_changed = 0;

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
static volatile char __G_start_flag = FSM_STOPPED;

/** extern definition for three phases circulation of the corresponding states */
extern struct SubSeqRoutine SSR_active;
extern struct SubSeqRoutine SSR_passive;
extern struct SubSeqRoutine SSR_coldstart;

/**
 * schedule table filling
 */
static const FSM_State __G_state[MAX_STATE_NUM] = 
{
	/* FREEZE  */
	{
		.state_num        = 0,
		.toState          = FSM_toFreeze,
		.doStateProcessor = 
			{
				.doState      = FSM_doFreeze,
				.pSubRoutine  = NULL, 
			},
	},

	/* INIT    */
	{
		.state_num        = 1,
		.toState          = FSM_toInit,
		.doStateProcessor = 
			{
				.doState       = FSM_doInit,
				.pSubRoutine   = NULL,
			},
	},

	/* LISTEN  */
	{
		.state_num        = 2,
		.toState          = FSM_toListen,
		.doStateProcessor = 
			{
				.doState       = FSM_doListen,
				.pSubRoutine   = NULL,
			},
	},

	/* COL_ST  */
	{
		.state_num        = 3,
		.toState          = FSM_toColdStart,
		.doStateProcessor = 
			{
				.doState       = NULL,
				.pSubRoutine   = &SSR_coldstart,
			},
	},

	/* SUB_CS */
	{
		.state_num        = 4,
		.toState   	      = FSM_toSubColdStart,
		.doStateProcessor = 
			{
				.doState  	   = FSM_doSubColdStart,
				.pSubRoutine   = NULL,
			},
	},

	/* ACTIVE  */
	{
		.state_num        = 5,
		.toState          = FSM_toActive,
		.doStateProcessor = 
			{
				.doState       = NULL,
				.pSubRoutine   = &SSR_active,
			},
	},

	/* PASSIVE */
	{
		.state_num        = 6,
		.toState          = FSM_toPassive,
		.doStateProcessor =  
			{
				.doState       = NULL,
				.pSubRoutine   = &SSR_passive,
			},
	},

	/* AWAIT   */
	{
		.state_num        = 7,
		.toState          = FSM_toAwait,
		.doStateProcessor =  
			{
				.doState       = FSM_doAwait,
				.pSubRoutine   = NULL,
			},
	},

	/* TEST    */
	{
		.state_num        = 8,
		.toState          = FSM_toTest,
		.doStateProcessor =  
			{
				.doState       = FSM_doTest,
				.pSubRoutine   = NULL,
			},
	},

	/* DOWNL   */
	{
		.state_num        = 9,
		.toState          = FSM_toDownload,
		.doStateProcessor =  
			{
				.doState       = FSM_doDownload,
				.pSubRoutine   = NULL,
			},
	},

	/* ERROR   */
	{
		.state_num        = 10,
		.toState          = NULL,
		.doStateProcessor =  
			{
				.doState       = NULL,
				.pSubRoutine   = NULL,
			},
	},
};


static const int  __PermissionTable[MAX_STATE_NUM][MAX_STATE_NUM] = 
{
 /*                 0       1      2       3        4       5      6       7      8     9      x */
/*                FREEZE   INIT  LISTEN  COL_ST  SUB_CS  ACTIVE  PASSIV  AWAIT  TEST  DOWNL  ERROR  */
 /*          */ 
 /* 0 FREEZE */  {  0,      1,     0,      0,      0,       0,      0,     1,     1,    0,     0},
 /*          */ 
 /* 1 INIT   */  {  1,      0,     1,      0,      0,       0,      0,     0,     0,    0,     0},
 /*          */
 /* 2 LISTEN */  {  1,      0,     0,      1,      0,       0,      1,     0,     0,    1,     0},
 /*          */
 /* 3 COL_ST */  {  1,      0,     1,      0,      1,       1,      1,     0,     0,    0,     0},
 /*          */
 /* 4 SUB_CS */  {  1,      0,     1,      1,      0,       0,      0,     0,     0,    0,     0},
 /*          */
 /* 5 ACTIVE */  {  1,      0,     0,      0,      0,       0,      1,     0,     0,    0,     0},
 /*          */
 /* 6 PASSIV */  {  1,      0,     0,      0,      0,       1,      0,     0,     0,    0,     0},
 /*          */
 /* 7 AWAIT  */  {  1,      0,     0,      0,      0,       0,      0,     0,     0,    1,     0},
 /*          */
 /* 8 TEST   */  {  1,      0,     0,      0,      0,       0,      0,     0,     0,    0,     0},
 /*          */
 /* 9 DOWNL  */  {  1,      0,     0,      0,      0,       0,      0,     0,     0,    0,     0},
 /*          */
 /* X ERROR  */  {  0,      0,     0,      0,      0,       0,      0,     0,     0,    0,     0}
 /*          */
};

static void _set_ps(uint32_t _ps_num)
{
	static int _num_to_ps[10] =
	{
		PS_FREEZE,PS_INIT,PS_LISTEN,PS_COLDSTART,PS_COLDSTART,PS_ACTIVE,PS_PASSIVE,PS_AWAIT,PS_TEST,PS_DOWNLOAD
	};

	if(_ps_num>10)
		return;
	
	uint32_t ps = _num_to_ps[_ps_num];
	SET_PS(ps);
}

void FSM_TransitIntoState(uint32_t NextState)
{
	TTP_ASSERT(NextState<10);
	TTP_ASSERT(__G_cur_state<10);

	if(!__PermissionTable[__G_cur_state][NextState])
	{
		ERROR("state transition error!");
		ERROR("current state is %d",__G_cur_state);
		ERROR("the next state is %d",NextState);
		DBG_Flush();
		while(1);
	}

	__G_cur_state = NextState;
	__G_state_changed = 1;

	if(__Gf_hook != NULL)
	{
		__Gf_hook(__G_state[__G_cur_state].state_num);
		_set_ps(__G_state[__G_cur_state].state_num);
	}
} 
void FSM_TransitIntoStateUrgent(uint32_t NextState)
{
	TTP_ASSERT(NextState<10);
	TTP_ASSERT(__G_cur_state<10);
	if(!__PermissionTable[__G_cur_state][NextState])
	{
		ERROR("state transition error!");
		ERROR("current state is %d",__G_cur_state);
		ERROR("the next state is %d",NextState);
		DBG_Flush();
		while(1);
	}
	if(__G_cur_state!=NextState)
	{
		__G_urgent_state = NextState;
	}
		
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
 * activation mode shall be set, making the state machine droping out from the doState.
 */
static void __protocol(void)
{
	while(__G_cur_state!=FSM_ERROR)
	{
		//process the urgent state transition
		if(__G_urgent_state!=FSM_ERROR)
		{
			__G_state_changed = 1;
			__G_cur_state = __G_urgent_state;
			__G_urgent_state = FSM_ERROR;
		}

		if(__G_state_changed==1)
		{
			if(__G_state[__G_cur_state].toState!=NULL)
			{
				__G_state[__G_cur_state].toState();
			}
			__G_state_changed = 0;
		}
		
		if((__G_state[__G_cur_state].doStateProcessor).doState != NULL)
		{
			(__G_state[__G_cur_state].doStateProcessor).doState();
		}
		else if((__G_state[__G_cur_state].doStateProcessor).pSubRoutine != NULL)
		{
			struct SubSeqRoutine* _p = (__G_state[__G_cur_state].doStateProcessor).pSubRoutine;
			volatile uint32_t*    _i = _p->pIndicator;
			//waiting for trigger event
			SVC_Wait(*_i);
			_p->func[*_i]();
			*_i = (*_i+1)%(sizeof(_p->func)/sizeof(_p->func[0]));
		}
	}
} 

inline void __machine_reset()
{
	//__Gf_hook 		= NULL;
	__G_start_flag  = FSM_STOPPED;  
	__G_cur_state   = FSM_ERROR;
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
	//NULL for this version
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
		__G_cur_state  = FSM_FREEZE;
		__G_start_flag = FSM_RUNNING;
		__G_state_changed = 1;
		SET_PS(PS_FREEZE);
	}
	__protocol();
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
	return __G_state[__G_cur_state].state_num;
}