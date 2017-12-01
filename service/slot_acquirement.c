/**
  ******************************************************************************
  * @file    	slot_acquirement.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	20170329
  * @brief   	the slot acquirement algorithm implementation
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */

#include "protocol_data.h"
#include "ttpc_mac.h"
#include "ttpdebug.h"
#include "ttpservice.h"
#include "virhw.h"

uint32_t SVC_SlotAcquirement(void)
{
    NodeProperty_t* pNP;
    RoundSlotProperty_t* pRS;
    ScheduleParameter_t* pSP;

    uint32_t mode;
    uint32_t res = 0;
    uint32_t int_counter;
    uint32_t mmp;

    pNP = MAC_GetNodeProperties();
    pRS = MAC_GetRoundSlotProperties();
    pSP = MAC_GetScheduleParameter();

    mmp = pNP->FlagPosition;

    mode = CS_GetCurMode();
    int_counter = PV_GetCounter(INTEGRATION_COUNTER);

    if (pNP->PassiveFlag == PERMANENT_PASSIVE)
        goto _end;
    if (pRS->ReintegrationAllow == REINTEGRATION_NOT_ALLOWED)
        goto _end;

    if (CNI_CheckHLFS() || (FREE_SHOT_ENABLE == PV_GetFreeShotFlag() && mode == MODE_CS_ID)) {
        if (int_counter == pSP->MinimumIntegrationCount) {
            if (!CS_GetMemberBit(mmp)) {
                res = 1;
            }
        }
    }
    PV_DisableFreeShot();
_end:
    return res;
}