#include <stm32f4xx_usart.h>
#include <stm32f4xx_dma.h>
#include "dbgio.h"
#include "ttpconstants.h"

static char _G_IO_Buffer[IO_BUFFER_SIZE] = {0};

void IO_DepInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//USART1 port configuration
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

   //USART1 parammeters configuration
	USART_InitStructure.USART_BaudRate = 1228800;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 

    DMA_InitTypeDef  DMA_InitStructure;	
    DMA_DeInit(DMA2_Stream7);

    while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE){}

    /* DMA Stream configuration*/
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
        
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)_G_IO_Buffer;
        
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    //DMA_InitStructure.DMA_BufferSize = ndtr;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
        
    USART_Cmd(USART1, ENABLE); 
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
}

void IO_Flush(int size)
{
    if(size<=0 || size>IO_BUFFER_SIZE)
        return;   
    
    //while(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)==SET);
    
	DMA_Cmd(DMA2_Stream7, DISABLE);
    DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
	
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);
		
	DMA_SetCurrDataCounter(DMA2_Stream7,size);
	DMA_Cmd(DMA2_Stream7, ENABLE); 
}

char* IO_GetBaseAddr(void)
{
    return _G_IO_Buffer;
}




