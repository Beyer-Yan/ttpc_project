/**
  ******************************************************************************
  * @file    	param.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.12
  * @brief   	This file provides the profile of o node
  ******************************************************************************
  * @attention
  *
  * @desc
  *  The file provides the same profiles as the file MEDL.c, but this file provides 
  *  the extracted version.
  * 
  *
  ******************************************************************************
  */

#include "ttpmac.h"
#include "clock.h"
#include "medl.h"
#include "xfer.h"
#include "ttpdebug.h"

/*********************** slot parameters configuration *************************/

RoundSlotProperty_t* MAC_GetRoundSlotProperties(void)
{
    uint32_t mode = CS_GetCurMode();
    uint32_t tdma = MAC_GetTDMARound();
    uint32_t slot = MAC_GetNodeSlot();

    uint32_t mode_num = CALC_MODE_NUM(mode);
    return MEDL_GetRoundSlotAddr(mode_num, tdma, slot);
}

RoundSlotProperty_t* MAC_LoadSlotProperties(uint32_t mode, uint32_t tdma, uint32_t slot)
{
    uint32_t mode_num;

    mode_num = CALC_MODE_NUM(mode);

    return MEDL_GetRoundSlotAddr(mode_num, tdma, slot);
}


/*********************** ID parameters access  *************************/

uint32_t  MAC_GetClusterScheduleID(void)
{
    return MEDL_GetSchedID();
}
uint32_t  MAC_GetAppID(void)
{
    return MEDL_GetAppID();
}

/*********************** node parameters access  *************************/

NodeProperty_t* MAC_GetNodeProperties(void)
{
    return MEDL_GetRegionAddr(ROLE_REGION);
}

uint32_t MAC_IsPassiveNode(void)
{
    NodeProperty_t* pNS = MAC_GetNodeProperties();

    if(pNS->PassiveFlag == PERMANENT_PASSIVE)
        return 1;
    return 0;
}
uint32_t MAC_IsMultiplexedMembershipNode(void)
{
    NodeProperty_t* pNS = MAC_GetNodeProperties();

    if(pNS->MultiplexedMembershipFlag == MULTIPLEXED_MEMBERSHIP)
        return 1;
    return 0;
}

/*********************** scheduler parameters access  *************************/
ScheduleParameter_t* MAC_GetScheduleParameter(void)
{
    return MEDL_GetRegionAddr(SCHEDULE_REGION);
}
uint32_t MAC_GetMinimumIntegrationCount(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    return pSP->MinimumIntegrationCount;
}
uint32_t MAC_GetMaximumColdStartEntry(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    return pSP->MaximumColdStartEntry;
}
uint32_t MAC_GetMaximumMembershipFailureCount(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();
    return pSP->MaximumMembershipFailureCount;
}
uint32_t MAC_GetMacrotickParameter(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    return pSP->MacrotickParameter;
}
uint32_t MAC_GetPrecision(void)
{
    ScheduleParameter_t* pSP = MAC_GetScheduleParameter();

    return pSP->Precision;
}

