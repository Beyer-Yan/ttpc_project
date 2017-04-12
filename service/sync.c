/**
  ******************************************************************************
  * @file    	sync.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.03
  * @brief   	the implementation syncronization shchema of TTPC protocol 
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */	
#include "ttpservice.h"
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "prococol_data.h"
#include "virhw.h"

#warning "FrMTs feature is not supported"
#define  FRMTS      1
#undef   FRMTS

#define  FIX_STEP   100;

/** @see "Time Triggered Protocol Spec", page 56*/
static volatile int32_t _G_pushdown_stack[4] = {0};

/** the estimate arival time interval of the frame aligned to local microtick */
static volatile int32_t _G_estimate_time_interval = 0;

static __INLINE void _stack_push(int32_t offset)
{
	static int idx = 0;

	_G_pushdown_stack[idx] = offset;
	idx = (idx+1)%((int)sizeof(_G_pushdown_stack));

}

static int32_t _average()
{
	int32_t tmp[4];

	tmp[0] =  _G_pushdown_stack[0];
	tmp[1] =  _G_pushdown_stack[1];
	tmp[2] =  _G_pushdown_stack[2];
	tmp[3] =  _G_pushdown_stack[3];

	/** simple bubble sort */

	tmp[0] > tmp[1] ? SWAP(tmp[0], tmp[1]) : (void)0;
	tmp[1] > tmp[2] ? SWAP(tmp[1], tmp[2]) : (void)0;
	tmp[2] > tmp[3] ? SWAP(tmp[2], tmp[3]) : (void)0;
	tmp[0] > tmp[1] ? SWAP(tmp[0], tmp[1]) : (void)0;
	tmp[1] > tmp[2] ? SWAP(tmp[1], tmp[2]) : (void)0;
	tmp[0] > tmp[1] ? SWAP(tmp[0], tmp[1]) : (void)0;

	return (tmp[1] + tmp[2])/2;
}

static __INLINE uint32_t _alignment_err(uint32_t value)
{
	static uint32_t mai = MAC_GetMacrotickParameter(); /**< ma = macrotick interval */
	static uint32_t ratio = MAC_GetRatio();

	static uint32_t lmi = mai/ratio;    /**< lmi = local microtick interval */

	return (value/lmi);
}

/** This function calculates the aligned result of the given value */
static int32_t _alignment_err_accumulated(int32_t value)
{
	/** alignment error */
	static int32_t err = 0;

	/** error correction term */
	static int32_t theta = 0;

	#warning "bugs may happen here"

	/** gurantee that the values below do not exceed the MAX(int32_t) */

	static int32_t mai = (int32_t)MAC_GetMacrotickParameter(); /**< ma = macrotick interval */
	static int32_t ratio = (int32_t)MAC_GetRatio();

	static int32_t lmi = mai/ratio;    /**< lmi = local microtick interval */

	TTP_ASSERT(!mai%ratio);

	err   = (value%lmi + err)%lmi;
	theta = (value%lmi + err)/lmi;

	return	value/lmi + theta;

}

void SVC_SetEstimateArivalTime(uint32_t EstimateTimeInterval)
{
	/** value too large */
	TTP_ASSERT(!(EstimateTimeInterval&0x80000000));

	int32_t _eti = (int32_t)EstimateTimeInterval;
	_G_estimate_time_interval = _alignment_err_accumulated(_eti);

}

void SVC_GetEstimateArivalTime()
{
	return _G_estimate_time_interval;
}

void SVC_SyncCalcOffset(uint32_t PSPTsmp, uint32_t FrameTsmp)
{
	uint32_t estimate_frame_tsmp = PSPTsmp + (uint32_t)_G_estimate_time_interval;
	int32_t  offset = (int)(FrameTsmp - estimate_frame_tsmp);

	_stack_push(offset);
}

uint32_t SVC_ExecSyncSchema(uint32_t Steps)
{
	int32_t avg  = _average();
	int16_t erc  = (int16_t)CNI_GetERC();
	int32_t csct = avg + _alignment_err_accumulated(erc);

	uint32_t precision = MAC_GetPrecision();

	int32_t aligned_pi = (int32_t)_alignment_err(precision); 

	if(csct > aligned_pi/2)
	{
		CNI_SetSRBit(SR_SE);
		FSM_sendEvent(FSM_EVENT_SYNC_ERR);
		return 0;
	}

	MAC_AdjTime(CLK_FREQ_ADJ,csct,FIX_STEP);

	return 1;
}
