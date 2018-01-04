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
#include "ttpmac.h"
#include "ttpdebug.h"
#include "ttpservice.h"

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

    if (pNP->NodeFlags & NodeFlags_PermanentPassive)
        goto _end;
    if (!(pRS->SlotFlags & SlotFlags_ReintegrationAllowed))
        goto _end;

    if (SVC_CheckHostLifeSign() || (FREE_SHOT_ENABLE == PV_GetFreeShotFlag() && mode == MODE_CS_ID)) {
        if (int_counter == pSP->MinimumIntegrationCount) {
            
            //@see Time-Triggered Protocol Spec, page 53.
            if(pNP->NodeFlags & NodeFlags_MultiplexedMembership){
                res = 1;
            }else if (!CS_GetMemberBit(mmp)) {
                res = 1;
            }
        }
    }
    PV_DisableFreeShot();
_end:
    return res;
}
