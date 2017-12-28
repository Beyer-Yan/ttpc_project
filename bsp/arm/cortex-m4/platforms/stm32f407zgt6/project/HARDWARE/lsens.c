#include "lsens.h"
#include <stm32f4xx.h>

void Lsens_DepInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��

    //�ȳ�ʼ��ADC3ͨ��7IO��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//PA7 ͨ��7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
    GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��  
    
    
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef       ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);  //ʹ��ADC3ʱ��


    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);	  //ADC3��λ
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);  //��λ����	 


    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
    ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC_Init(ADC3, &ADC_InitStructure);//ADC��ʼ��
	
 
	ADC_Cmd(ADC3, ENABLE);//����ADת����
}
//��ȡLight Sens��ֵ
//0~100:0,�;100,���� 
uint8_t Lsens_GetVal(void)
{
	uint32_t temp_val=0;

    ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_480Cycles );	//ADC3,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    

    ADC_SoftwareStartConv(ADC3);		//ʹ��ָ����ADC3�����ת����������	

    while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));//�ȴ�ת������

    temp_val = ADC_GetConversionValue(ADC3);	//�������һ��ADC3�������ת�����

	if(temp_val>4000)
        temp_val=4000;
    
	return (uint8_t)(100-(temp_val/40));
}












