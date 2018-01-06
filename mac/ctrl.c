/**
  ******************************************************************************
  * @file    	ctrl.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2017.12
  * @brief   	slot control
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * implementation of the mac control interface, which is responsible for the slot
  * parammeter loading, node paramenters access and the scheduling paramenters
  * accesing. Other operations such as the time parameters of the slot, clock 
  * adjusting and etc. are instantiated in this file
  *
  ******************************************************************************
  */
#include "ttpmac.h"
#include "clock.h"
#include "medl.h"
#include "xfer.h"
#include "ttpdebug.h"

void MAC_StartTransmit(void)
{
    DRV_StartTransmission();
}
void MAC_StopTransmit(void)
{
    DRV_StopTransmission();
}

void MAC_StartReceive(void)
{
    DRV_StartReception();
}
void MAC_StopReceive(void)
{
    DRV_StopReception();
}


void MAC_StartPhaseCirculation()
{ 
     CLOCK_EnableTrigger(); 
}

void MAC_StopPhaseCirculation()
{
    CLOCK_DisableTrigger();
}

void MAC_AdjTime(uint16_t AdjMode, int16_t Offset)
{
    // AdjMode is set CLK_FREQ_ADJ forcedly

    // TTP_ASSERT(Steps < MAX_STEPS);

    // int32_t cor_val[MAX_STEPS] = { 0 };

    // int32_t ratio = (int32_t)TIM_GetFrequencyDiv();

    // int i = 0;
    // int j = 0;

    // int _step = MAX_STEPS / (FrMT + 1);

    // int32_t quotient = Offset / _step;
    // int32_t remain = Offset % _step;

    // for (j = 0, i = 0; i < _step; i++, j += FrMT + 1) {
    //     cor_val[j] = quotient;
    // }

    // int32_t tmp = ABS(remain);
    // for (i = 0, j = 0; i < tmp; i++, j += FrMT + 1) {
    //     cor_val[j] += remain > 0 ? 1 : -1;
    // }

    // for (i = 0; i < MAX_STEPS; i++) {
    //     cor_val[i] += ratio;
    // }

    if(AdjMode == CLK_FREQ_ADJ)
    {
        CLOCK_SetStateCorrectionTerm(Offset);
    }
    else
    {
        #warning "phase adjusting is removed"
    }
}
