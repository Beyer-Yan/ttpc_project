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
#include "ttpmac.h"
#include "ttpdebug.h"
#include "ttpservice.h"
#include "protocol_data.h"
#include "clock.h"

static void _cni_init(void)
{
    int _i = 0;

    for (; _i < sizeof(__ttpc_status_regs_group)/4; _i++) {
        __ttpc_status_regs_group[_i] = 0;
    }

    _i = 0;

    for (; _i < sizeof(msg_pool); _i++) {
        msg_pool[_i] = 0;
    }
}

static void _pv_init(void)
{
    int i = 0;
    for (; i < sizeof(PV_data); i++) {
        PV_data[i] = 0;
    }
    PV_EnableBigBang();
    PV_EnableFreeShot();
}

static void _clock_init(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    uint32_t frequency = CLOCK_GetLocalFrequency();
    uint32_t macrotick = pSP->MacrotickParameter;
    uint32_t freq_div = (frequency)*(macrotick)/1000;

    CLOCK_SetStateCorrectionTerm(0);
    CLOCK_SetCurMacrotick(0);
    CLOCK_SetCurMicrotick(0);
    CLOCK_SetFrequencyDiv(freq_div); //840， normally
}

static void _id_init(void)
{
    //HW_GetPlatformID((uint8_t*)TTP_ID_BASE, sizeof(ttp_id));
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
        FSM_TransitIntoState(FSM_FREEZE);

        return;
    }

    _id_init();
    _clock_init();
    _pv_init();

    SVC_RaiseAsynchronousInterrupt();
    FSM_TransitIntoState(FSM_LISTEN);
}
