#include "led.h" 
#include "stm32f4xx_gpio.h"

#define LED_SYNC_PIN    GPIO_Pin_10
#define LED_ERR_PIN     GPIO_Pin_9

void LED_DepInit(void)
{    	 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
	
    //GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10|GPIO_Pin_8);
    GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);
}

void LED_On(int ledKind)
{
    if(ledKind==LED_SYNC)
    {
        GPIO_ResetBits(GPIOF,LED_SYNC_PIN);
    }
    else if(ledKind==LED_ERR)
    {
       GPIO_ResetBits(GPIOF,LED_ERR_PIN); 
    }
    else if(ledKind==LED_TX)
    {
        GPIO_ResetBits(GPIOF,LED_SYNC_PIN);;//todo
    }
}

void LED_Off(int ledKind)
{
    if(ledKind==LED_SYNC)
    {
        GPIO_SetBits(GPIOF,LED_SYNC_PIN);
    }
    else if(ledKind==LED_ERR)
    {
       GPIO_SetBits(GPIOF,LED_ERR_PIN); 
    }
    else if(ledKind==LED_TX)
    {
        GPIO_SetBits(GPIOF,LED_SYNC_PIN);;//todo
    }
}

void LED_Toggle(int ledKind)
{
    if(ledKind==LED_SYNC)
    {
        GPIO_ToggleBits(GPIOF,LED_SYNC_PIN);
    }
    else if(ledKind==LED_ERR)
    {
       GPIO_ToggleBits(GPIOF,LED_ERR_PIN); 
    }
    else if(ledKind==LED_TX)
    {
        GPIO_ToggleBits(GPIOF,LED_SYNC_PIN);;//todo
    }
}

void LED_Twinkle(int ledKind)
{
    //todo
}







