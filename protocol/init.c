/**
  ******************************************************************************
  * @file    	init.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	20170331
  * @brief   	brief description about this file
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
 #include "ttpc_mac.h"
 #include "ttpdebug.h"
 #include "protocol_data.h"
 #include "protocol.h"
 #include "virhw.h"

 #include "medl.h"


static __INLINE _cni_init(void)
{
	int _i = 0;

	for(;_i<sizeof(__ttpc_status_regs_group);_i++)
	{
		__ttpc_status_regs_group[_i] = 0;
	}

	_i = 0;

	for(;_i<sizeof(msg_pool); _i++)
	{
		msg_pool[_i] = 0;
	}

}

static __INLINE _timer_init(void)
{
	ScheduleParameter_t* pSP = MAC_GetScheduleParameter();
	TIM_DepInit();
	TIM_SetCompensateMode(COMPENSATE_MODE);
	TIM_ChannelCMD(TIM_CH_1,ENABLE);
	TIM_ChannelCMD(TIM_CH_2,ENABLE);
	TIM_ChannelCMD(TIM_CH_3,ENABLE);

	//for channel mode, opened defaultly
	TIM_SetChannelMicrotick(TIM_CH_1,MIC_10_NS);
	TIM_SetChannelMicrotick(TIM_CH_1,MIC_1_US);
	TIM_SetChannelMicrotick(TIM_CH_3,MIC_100_US);

	//for compemsate mode
	TIM_SetLocalMicrotick(MIC_50_NS);

	TIM_SetMacrotick(pSP->MacrotickParameter);
}

static __INLINE _id_init(void)
{
	HW_GetPlatformID(TTP_ID_BASE,sizeof(ttp_id));
	TTP_IDCR = MEDL_GetSchedID();
	TTP_IDAR = MEDL_GetAppID();
}

void FSM_toInit(void)
{
	//do nothing
}

void FSM_doInit(void)
{
	//clear the status field of cni
	_cni_init();

	//medl init and preload
	if(!MEDL_Init())
	{
		CNI_SetISRBit(ISR_MC);
        FSM_sendEvent(FSM_EVENT_INIT_ERR);

        return ;
	}

	_id_init();

	_timer_init();



	FSM_sendEvent(FSM_EVENT_INIT_OK);




	
	


}