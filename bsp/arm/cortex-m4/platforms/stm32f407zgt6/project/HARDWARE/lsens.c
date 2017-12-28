#include "lsens.h"
#include <stm32f4xx.h>

void Lsens_DepInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef       ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);


    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);	 


    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//�ADCCLK=PCLK2/4=84/4=21Mhzno more than 36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC3, &ADC_InitStructure);
	
 
	ADC_Cmd(ADC3, ENABLE);
}

//0~100 : dark to light 
uint8_t Lsens_GetVal(void)
{
	uint32_t temp_val=0;

    ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_480Cycles );	    
    ADC_SoftwareStartConv(ADC3);
    while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));

    temp_val = ADC_GetConversionValue(ADC3);

	if(temp_val>4000)
        temp_val=4000;
    
	return (uint8_t)(100-(temp_val/40));
}












