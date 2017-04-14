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
#include "protocol.h"
#include "ttpdebug.h"

#define MAX_TRANSITION_NUM 		20
#define MAX_STATE_NUM 			11

/**
 * This variable is used to record the events happened, mapping out the transition number
 * to be activated.
 */
static volatile uint32_t __G_event_bit_pattern = 0;

/**
 * This variable is used to record the permanent events, such as POWER_ON, AWAIT_CLR
 * TEST_CLR and so on. The permanent events happened will not be cleared if the 
 * corresponding transition number is activated according to some events snapshot.  
 */
static volatile uint32_t __G_permanent_events = 0; 

/**
 * This variable is used to record the current state where the controller situates at
 * present.
 */
static const  FSM_State* volatile __G_cur_state = NULL;

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

/** extern definition for three phases cirlation of the corresponding states */
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
		.doState          = FSM_doSubColdStart,
		.doStateProcessor = 
			{
				.doState  	   = FSM_doSubColdStart,
				.pSubRoutine   = NULL,
			},
	}

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

/**
 * The macro definitions below are used to simplify the name spellings of the
 * state pointers and the state names.
 */
/** state pointers macro definitions*/
#define S_N 				NULL 				/**< for NULL pointer */
#define S_X 				&__G_state[10] 		/**< pointer to ERROR state */
#define S_0 				&__G_state[0] 		/**< pointer to FREEZE state */
#define S_1 				&__G_state[1] 		/**< pointer to INIT state */
#define S_2 				&__G_state[2] 		/**< pointer to LISTEN state */
#define S_3 				&__G_state[3] 		/**< pointer to COLD_START state */
#define S_4 				&__G_state[4] 		/**< pointer to the subColdStart state */
#define S_5 				&__G_state[5] 		/**< pointer to ACTIVE state */
#define S_6 				&__G_state[6] 		/**< pointer to PASSIVE state */
#define S_7 				&__G_state[7] 		/**< pointer to AWAIT state */
#define S_8 				&__G_state[8] 		/**< pointer to TEST state */
#define S_9 				&__G_state[9] 		/**< pointer to DOWNLOAD state */

/** state names macro definitions*/
#define S_FREEZE 			S_0
#define S_INIT 				S_1
#define S_LISTEN 			S_2
#define S_COLD_START 		S_3
#define S_SUB_CS 			S_4
#define S_ACTIVE 			S_5
#define S_PASSIVE 			S_6
#define S_AWAIT 			S_7
#define S_TEST  			S_8
#define S_DOWNLOAD 			S_9
#define S_ERROR 			S_X

static const FSM_State* const __G_transition_table[MAX_STATE_NUM][MAX_TRANSITION_NUM] = 
{
 /*                 0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19 */
/*                TS1  TS2  TS3  TS4  TS5  TS6  TS7  TS8  TS9  TS10 TS11 TS12 TS13 TS14 TS15 TS16 TS17 TS18 TS19 TS20 */
 /*          */ 
 /* 0 FREEZE */  { S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_8, S_X, S_7, S_1, S_X, S_X, S_X, S_X, S_X, S_X, S_X },
 /*          */ 
 /* 1 INIT   */  { S_0, S_2, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X },
 /*          */
 /* 2 LISTEN */  { S_0, S_X, S_6, S_3, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_9, S_X, S_X, S_X, S_X, S_X },
 /*          */
 /* 3 COL_ST */  { S_0, S_X, S_X, S_X, S_X, S_5, S_2, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_4, S_X, S_X, S_6 },
 /*          */
 /* 4 COL_ST */  { S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_2, S_3, S_X },
 /*          */
 /* 5 ACTIVE */  { S_0, S_X, S_X, S_X, S_0, S_X, S_X, S_6, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X },
 /*          */
 /* 6 PASSIV */  { S_0, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_5, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X },
 /*          */
 /* 7 AWAIT  */  { S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_9, S_X, S_X, S_X, S_X, S_X },
 /*          */
 /* 8 TEST   */  { S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_X, S_X, S_X },
 /*          */
 /* 9 DOWNL  */  { S_0, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_X, S_0, S_X, S_X, S_X, S_X },
 /*          */
 /* X ERROR  */  { S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N, S_N }
 /*          */
};

/**
 * This function calculates the transition number according to the event bit
 * pattern.
 * @return  the transition number.
 * @attention If no transition happended, the function will return -1.
 */
static int __calc_transition_num(void)
{
	int tn = -1;
	uint16_t tbp = 0;       /**< transition bit pattern */

	uint32_t sn = __G_cur_state->state_num;

	static uint32_t bit_mask[32] = {

		1<< 0,1<< 1,1<< 2,1<< 3,1<< 4,1<< 5,1<< 6,1<< 7,1<< 8,1<< 9,
		1<<10,1<<11,1<<12,1<<13,1<<14,1<<15,1<<16,1<<17,1<<18,1<<19,
		1<<20,1<<21,1<<22,1<<23,1<<24,1<<25,1<<26,1<<27,1<<28,1<<29,
		1<<30,1<<31
	};


	// static uint32_t event_clr_mask_old[20] = {
		
	// 	0xfffffbfe,0xfffffbfd,0xfffffbe3,0xfffff81f,
	// 	0xf7fff3ff,0xffffcbef,0xeffe3bff,0xeff9fbff,
	// 	0xfff7fbef,0x9fcffbff,0xeffdebff,0xffbffbff,
	// 	0xffffdbff,0xfefffbff,0xfdfffbff,0xfbfffbff,
	// };
	static uint32_t event_clr_mask[20] = 
	{
		0xfffffbff,0xfffffffe,0xfffffffd,0xffffffe3,
		0xfffffc1f,0xffffa7ff,0xffffffdf,0xffff1fff,
		0xfffeffff,0xfff9ffff,0xfff7ffff,0xffeffdff,
		0xff9bffff,0xff7fffff,0xfeffffff,0xfdffffff,
		0xffffffbf,0xf3ffdfff,0xffffffe7,0xffff97ff,
	};

	// static uint16_t trans_mask_old[9] = {

	// 	0x1a00,0x0011,0x4016,
	// 	0x0470,0x0098,0x0118,
	// 	0x4010,0x2010,0x8010,

	// };

	static uint32_t trans_mask[10] = 
	{
		0x00001a00,0x00000403,0x0000400d,0x00090061,0x00060001,
		0x00000091,0x00000111,0x00004001,0x00002001,0x00008001,
	};

	static uint8_t tpos[16] = {

		0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0,
	};
	static uint8_t wpos[4] = {
		0,4,8,12
	};

	/** get the bit i of event bit pattern, return 0 or 1. i from 0 to 31 */
	#define E(i) 		((__G_event_bit_pattern&bit_mask[i])>>i)
	#define G(e,l) 		((e)<<(l))

	/**
	 * calculation process here. If no corresponding transition number is activated,
	 * the variable transition_num will set to -1. And if valid transition number
	 * is activated, the corresponding bits of the transition number occupying in
	 * the events bits pattern shall be cleared.
	 */
	/* 01 */ tbp |= G(E(11),0);
	/* 02 */ tbp |= G(E(1),1);
	/* 03 */ tbp |= G(E(2),2);
	/* 04 */ tbp |= G(E(3)&E(4)&E(5),3);
	/* 05 */ tbp |= G(E(6)|E(7)|E(8)|E(9)|E(10),4);
	/* 06 */ tbp |= G(E(12)&E(13)&E(E5),5);
	/* 07 */ tbp |= G(E(6),6);
	/* 08 */ tbp |= G(E(14)|E(15)|E(16),7);
	/* 09 */ tbp |= G(E(17),8);
	/* 10 */ tbp |= G(E(18)&E(19),9);
	/* 11 */ tbp |= G(E(20),10);
	/* 12 */ tbp |= G(E(21)&E(19),11);
	/* 13 */ tbp |= G(E(22)&E(23)&E(19),12);
	/* 14 */ tbp |= G(E(24),13);
	/* 15 */ tbp |= G(E(25),14);
	/* 16 */ tbp |= G(E(26),15);
	/* 17 */ tbp |= G(E(7),16);
	/* 18 */ tbp |= G(E(27)&E(28)&E(14),17);
	/* 19 */ tbp |= G(E(4)&E(5),18);
	/* 20 */ tbp |= G(E(12)&(E(14)|E(15)),19);

	#undef E
	#undef G

	tbp &= trans_mask[sn];
	tbp = tbp^(tbp-1)&tbp;

	while(tbp) { tbp>>=1; tn++ }
	tn!=-1 ? __G_event_bit_pattern &= event_clr_mask[tn] : (void)0;
	
	return tn;
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

	int _x; 	/**< state number */
	int _y; 	/**< transition number */
	static int state_changed = 1;
	/**
	 * record the state number of the current state for programing conciseness. It's
	 * not necessary to record the variable because of the memory wasting.
	 */
	_x = __G_cur_state->state_num;

	while(__G_cur_state!=S_ERROR)
	{
		if(state_changed==1)
		{
			if(__G_cur_state->toState!=NULL)
			{
				__G_cur_state->toState();
			}

			state_changed = 0;
		}
		
		if((__G_cur_state->doStateProcessor).doState != NULL)
		{
			(__G_cur_state->doStateProcessor).doState();
		}
		else if((__G_cur_state->doStateProcessor).pSubRoutine != NULL)
		{
			struct SubSeqRoutine* _p = (__G_cur_state->doStateProcessor).pSubRoutine;
			volatile uint32_t*    _i = _p->pIndicator;

			_p->func[*_i]();
			*_i = (*_i+1)%(sizeof(_p->func)/sizeof(_p->func[0]));

		}

		_y = __calc_transition_num();
		
		if(_y != -1)
		{
			state_changed = 1;
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
	__G_event_bit_pattern |= (Event|__G_permanent_events);
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
	return __G_cur_state->state_num;
}