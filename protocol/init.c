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
#include "medl.h"
#include "protocol.h"
#include "protocol_data.h"
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "virhw.h"

#include "ttpservice.h"

static inline void _cni_init(void)
{
    int _i = 0;

    for (; _i < sizeof(__ttpc_status_regs_group); _i++) {
        __ttpc_status_regs_group[_i] = 0;
    }

    _i = 0;

    for (; _i < sizeof(msg_pool); _i++) {
        msg_pool[_i] = 0;
    }
}

static inline void _pv_init(void)
{
    int i = 0;
    for (; i < sizeof(PV_data); i++) {
        PV_data[i] = 0;
    }
    PV_EnableBigBang();
    PV_EnableFreeShot();
}

static inline void _timer_init(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();
    TIM_DepInit();

    uint32_t frequency = TIM_GetLocalFrequency();
    uint32_t macrotick = pSP->MacrotickParameter;

    uint32_t freq_div = frequency/macrotick;

    TIM_SetStateCorrectionTerm(0);
    TIM_SetCurMacrotick(0);
    TIM_SetCurMicrotick(0);
    TIM_SetFrequencyDiv(freq_div);
}

static inline void _id_init(void)
{
    HW_GetPlatformID((uint8_t*)TTP_ID_BASE, sizeof(ttp_id));
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
    if (!MEDL_Init()) {
        CNI_SetSRBit(SR_MC);
        CNI_SetISRBit(ISR_PE);
        FSM_sendEvent(FSM_EVENT_INIT_ERR);

        return;
    }

    _id_init();

    _timer_init();

    _pv_init();

    //platform init
    DMA_DepInit();
    WDG_DepInit();
    CRC_DepInit();
    DRV_DepInit();

    SVC_RaiseSynchronousInterrupt();
    FSM_sendEvent(FSM_EVENT_INIT_OK);
}