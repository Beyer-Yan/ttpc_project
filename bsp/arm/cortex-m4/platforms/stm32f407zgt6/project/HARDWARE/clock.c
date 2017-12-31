/**
  ******************************************************************************
  * @file    	virhw.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.09
  * @brief   	This file connects the low-level hardware and the high-level protocol
  ******************************************************************************
  * @attention
  *
  * @desc
  * For the platform portability,  it is necessary to  separate the low-level hardwares and
  * the high-level  protocol  services. The virtual  hardware level is set to transport the 
  * action coming from the upper level to the real hardware operations. Different platforms
  * have different hardware implementations, so the program generosity should be guaranteed
  * within  shielding the real hardware differences.
  * 
  * The clock is a very important notion of TTA, which determines the timing correctness of 
  * the protocol  services. The local  clock and  the the global clock are specified in the 
  * clock module. The DMA module is inessential, but it will reduce the memory access delay 
  * jitter and the utilization  of the CPU. The TRIGGER   module   provides  the   concrete
  * trigger-control unit in TDMA timing of MAC services, which contains the capture  of the
  * timestamps of the sending or the receiving messages,and the triggering point of the PSP,
  * AT  and PRP actions. 
  ******************************************************************************
  */
	
#include "clock.h"
#include "stm32f4xx.h"

//#include "led.h"

/************** Macro Deffinitions ********************************************************/

#define CLOCK_LOCAL_FREQ       (84) //MHz
#define DEFAULT_DIV            (839)
#define CLOCK_ADJUSTING_STEPS  (10)

/************** Global Variables Deffinitions *********************************************/

static uint32_t _G_GlobalTimerOverflowCounter = 0;

//static volatile uint8_t _G_TriggerATFlag  = 0;
//static volatile uint8_t _G_TriggerPRPFlag = 0;
//static volatile uint8_t _G_TriggerEndFlag = 0;

static volatile uint16_t _G_TriggerArr[3] = {0};

static uint16_t _G_TuningArray[CLOCK_ADJUSTING_STEPS] = {0};

/************** Functions Deffinitions ***************************************************/

void CLOCK_DepInit(void)
{
    //enable the TIM2 and the TIM3 for local clock.
    //the TIM3 functions as a divider for TIM4 and TIM5.
    //the TIM4 as a 16 bits system global clock.
    //the TIM5 as a 32 bits user macrotick clock. 
    /*********************************************************************/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //local clock 32bit
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //clock divider 16bit
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //system global clock 16bit
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); //user global clock 32bit

    //input capture channel pins
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    
    //for speeding up the dynamic clock adjusting
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    
    //configuration parameters for local clock TIM2, 84MHz
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period        = 0xFFFFFFFF;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
    
    //configuration parameters for local clock divider TIM3, 84MHz
    //works in master mode
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period        = (uint16_t)DEFAULT_DIV; //839+1=840,10us Period 
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
    TIM_ARRPreloadConfig(TIM3,ENABLE);
    TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);
    
    //configuration parameters for system global clock TIM4, 
    //works in slave mode
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period        = 0xFFFF;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
    TIM_SelectSlaveMode(TIM4,TIM_SlaveMode_External1);
    TIM_SelectInputTrigger(TIM4,TIM_TS_ITR2);
    
    //configuration parameters for user global clock TIM5, 
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period        = 0xFFFFFFFF;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);
    TIM_SelectSlaveMode(TIM5,TIM_SlaveMode_External1);
    TIM_SelectInputTrigger(TIM5,TIM_TS_ITR1);
    
    //capture configuration for TIM2, the 32 bits local clock 
    /******************************************************************/
    //  PA5  -> TIM2_CH1 -> RX0
    //  PB3  -> TIM2_CH2 -> RX1
     
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_TIM2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2);    
    
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter    = 0;
    TIM_ICInit(TIM2,&TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_2;
    TIM_ICInit(TIM2,&TIM_ICInitStructure);
 
    //Compare trigger parameters for TIM3, the global clock
    /******************************************************************/
    /*
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    
    //for TIM3 OC1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle; //be not influenced
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OC1Init(TIM4,&TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Disable);
    
    //for TIM3 OC2, the same
    TIM_OC2Init(TIM4,&TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Disable);
    
    //for TIM3 OC3, the same
    TIM_OC3Init(TIM4,&TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Disable);
    */
    //DMA1 configuration for the dynamic frequency adjusting of TIM3
    /*******************************************************************/
    DMA_InitTypeDef  DMA_InitStructure;	
    DMA_DeInit(DMA1_Stream2);

    while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);
    
    int i = 0;
    for(;i<CLOCK_ADJUSTING_STEPS;i++)
        _G_TuningArray[i] = DEFAULT_DIV;

    /* DMA Stream configuration*/
    DMA_InitStructure.DMA_Channel            = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM3->DMAR;       
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)_G_TuningArray;       
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = CLOCK_ADJUSTING_STEPS;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream2, &DMA_InitStructure);
    
    //DMA_ITConfig(DMA1_Stream2,DMA_IT_TC,ENABLE);
    
    TIM_DMAConfig(TIM3,TIM_DMABase_ARR,TIM_DMABurstLength_1Transfer);
    TIM_DMACmd(TIM3,TIM_DMA_Update,ENABLE);
    
    //interruption configuration 
    /*******************************************************************/
    NVIC_InitTypeDef NVIC_InitStructure;
    /*
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
    
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
    
    /* for system time measuring */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period        = 0xffff;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStructure);
    TIM14->CR1 |= TIM_CR1_CEN; 
}
/*
void DMA1_Stream2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2)==SET)
    {
        DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);
        LED_On(LED_SYNC);
    } 
}
*/
void TIM5_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET)
    {
        _G_GlobalTimerOverflowCounter++;
        TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
    }
}

/* Base operations of TTPC timer *******************************************/
void CLOCK_Start(void)
{
    TIM4->CR1 |= TIM_CR1_CEN;
    TIM5->CR1 |= TIM_CR1_CEN;
    //close interruption,todo
    
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    
    //open interruption,todo
}
void CLOCK_Stop(void)
{
    TIM2->CR1 &= (uint16_t)~TIM_CR1_CEN;
    TIM3->CR1 &= (uint16_t)~TIM_CR1_CEN;
    TIM4->CR1 &= (uint16_t)~TIM_CR1_CEN;
    TIM5->CR1 &= (uint16_t)~TIM_CR1_CEN;
}
void CLOCK_Reset(void)
{
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4,ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5,ENABLE);
    
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2,DISABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3,DISABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4,DISABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5,DISABLE);
    
    CLOCK_DepInit();
}
void CLOCK_Clear(void)
{
    CLOCK_Stop();
    TIM2->CNT = 0;
    TIM3->CNT = 0;
    TIM4->CNT = 0;
    TIM5->CNT = 0;
}

void CLOCK_EnableTrigger(void)
{
    //TIM_ITConfig(TIM4,TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3,ENABLE);
}
void CLOCK_DisableTrigger(void)
{
    //TIM_ITConfig(TIM4,TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3,DISABLE);
}

/* Macro/Micro-ticks getting operations ***********************************/
uint32_t CLOCK_GetFrequencyDiv(void)
{
    return TIM3->ARR + 1;
}
uint32_t CLOCK_GetLocalFrequency(void)
{
    return CLOCK_LOCAL_FREQ;
}

uint32_t CLOCK_GetCaptureRX0(void)
{
    //clear the flag;
    TIM_GetFlagStatus(TIM2,TIM_FLAG_CC1);
    TIM_GetFlagStatus(TIM2,TIM_FLAG_CC1OF);
    return TIM2->CCR1;
}
uint32_t CLOCK_GetCaptureRX1(void)
{
    //clear the flag
    TIM_GetFlagStatus(TIM2,TIM_FLAG_CC2);
    TIM_GetFlagStatus(TIM2,TIM_FLAG_CC2OF);
    return TIM2->CCR2;
}

uint32_t CLOCK_GetCurMacrotick(void)
{
    return TIM4->CNT;
}
uint32_t CLOCK_GetCurMicrotick(void)
{
    return TIM2->CNT;
}


void CLOCK_ClearCaptureAll(void)
{
    //todo
}

/* Trigger settings *******************************************************/

static inline void _start_adjust(void)
{
    //start adjusting
    DMA_Cmd(DMA1_Stream2, DISABLE);
    DMA1->LISR = 0;
    DMA1_Stream2->NDTR = (uint16_t)CLOCK_ADJUSTING_STEPS;
    DMA_Cmd(DMA1_Stream2, ENABLE);
}

void CLOCK_SetStateCorrectionTerm(int16_t value)
{
    /*
    uint16_t abs = 0; 
    uint16_t arr = TIM3->ARR;
    
    abs = value > 0 ? value : -value;
    
    uint32_t tmp1;
    uint32_t tmp2;
    uint32_t tmp3;
    
    if(value==0)
        return;
    
    if(value<0)
        TIM3->CNT = TIM3->CNT - abs + 7;
    else
    {
        tmp1 = TIM3->CNT;
        tmp2 = tmp1 + abs;
        tmp3 = arr - 6; //experiences number
        if(tmp2<tmp3) 
        {
           TIM3->CNT = TIM3->CNT + abs + 9;
        }
        else
        {
            tmp1 = arr - TIM3->CNT;
            TIM3->CNT = arr;
            TIM3->CNT = TIM3->CNT + abs - tmp1 + 15;
        }
    }
    */
    //configuration with DMA
    int16_t abs = value>0 ? value : -value;
    int16_t quotient = -value/(CLOCK_ADJUSTING_STEPS-1);
    int16_t remain   = -value%(CLOCK_ADJUSTING_STEPS-1);
    int i = 0;
    
    for(;i<CLOCK_ADJUSTING_STEPS-1;i++)
        _G_TuningArray[i] = (uint16_t)(DEFAULT_DIV + quotient);
        
    _G_TuningArray[0] = (uint16_t)((int16_t)_G_TuningArray[0] + remain);
    
    //not nessasary
    if(value > 0)
    {
        TIM2->CNT = TIM2->CNT + abs + 7;
    }
    else
    {
        TIM2->CNT = TIM2->CNT - abs + 7; 
    }
    
    _start_adjust();
}
void CLOCK_SetFrequencyDiv(uint16_t div)
{
    TIM3->ARR = div - 1;
}
void CLOCK_SetCurMacrotick(uint32_t MacrotickValue)
{
    TIM4->CNT = MacrotickValue;
    TIM5->CNT = MacrotickValue & 0x0000FFFF;
}
void CLOCK_SetCurMicrotick(uint32_t MicrotickValue)
{
    TIM2->CNT = MicrotickValue;
    TIM3->CNT = 0;
}

//in uint of macrotick
void CLOCK_SetTriggerAT(uint32_t at)
{
    //TIM_SetCompare1(TIM4,at);
    _G_TriggerArr[CLOCK_TRIGGER_AT] = at&0xffff;
}
void CLOCK_SetTriggerPRP(uint32_t prp)
{
    //TIM_SetCompare2(TIM4,prp);
    _G_TriggerArr[CLOCK_TRIGGER_PRP] = prp&0xffff;
}
void CLOCK_SetTriggerSlotEnd(uint32_t end)
{
    //TIM_SetCompare3(TIM4,end);
    _G_TriggerArr[CLOCK_TRIGGER_END] = end&0xffff;
}

void CLOCK_WaitTrigger(uint32_t _ClockTrigger)
{
    while(TIM4->CNT!=_G_TriggerArr[_ClockTrigger]);
}
/*
void CLOCK_WaitTrigger(uint32_t ClockTrigger)
{
    
    if(ClockTrigger == CLOCK_TRIGGER_AT)
    {
        assert_param(_G_TriggerATFlag!=1);
        while(!_G_TriggerATFlag);
        _G_TriggerATFlag = 0;
    }
    else if(ClockTrigger == CLOCK_TRIGGER_PRP)
    {
        assert_param(_G_TriggerPRPFlag!=1);
        while(!_G_TriggerPRPFlag);
        _G_TriggerPRPFlag = 1;
    }
    else if(ClockTrigger == CLOCK_TRIGGER_END)
    {
        assert_param(_G_TriggerEndFlag!=1);
        while(!_G_TriggerEndFlag);
        _G_TriggerEndFlag = 0;
    }
    else
    {
        //parameter ilegal
        assert_param(1);
    }
    
    _CLOCK_WaitTriggerForTest(ClockTrigger);
}
*/

uint32_t CLOCK_WaitAlarm(uint32_t  Macroticks, uint32_t (*fDisturb)(void))
{
    uint32_t curTime = TIM5->CNT;
    while(TIM5->CNT - curTime < Macroticks)
    {
        if(fDisturb != 0)
            if(fDisturb())
                return 1;
    }
    return 0;
}
uint32_t CLOCK_WaitMicroticks(uint32_t MicrotickValue, uint32_t (fDisturb)(void))
{
    uint32_t curTime = TIM2->CNT;
    while(TIM2->CNT - curTime < MicrotickValue)
    {
        if(fDisturb != 0)
            if(fDisturb())
                return 1;
    }
    return 0;
}

/* user settings **********************************************************/
void CLOCK_SetTriggerUser(uint16_t UserMacrotickValue)
{
    //todo
}

void CLOCK_EnableUserIT(void)
{
    //todo
}
void CLOCK_DisenableUserIT(void)
{
    //todo
}
void CLOCK_SetAlarmOnce(uint16_t UserMacroValue)
{
    //todo
}
void CLOCK_SetAlarmPeriod(uint16_t UserMacroValue)
{
    //todo
}
