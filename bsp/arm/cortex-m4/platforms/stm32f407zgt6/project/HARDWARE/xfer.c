/**
  ******************************************************************************
  * @file    	xfer.c
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2017.10
  * @brief   	the hardware level of the tranduscer of ttpc
  ******************************************************************************
  * @attention
  *  Only RMII mode is enable in this implementation.
  * @desc
  * 
  * 
  ******************************************************************************
  */

#include "xfer.h"
#include "lan8720.h"
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_conf.h"
#include "ttpdebug.h"

#include "ttpconstants.h"

#if defined (TTP_NODE0) || defined (TTP_NODE1)
    #undef LAN8720_PHY_ADDRESS
    #define LAN8720_PHY_ADDRESS 0x00
#endif


//#include "crc.h"

//DMA descriptor and buffer variables for channel 0
__align(4)
    static ETH_DMADESCTypeDef DMARxDscrTab0;
__align(4)
    static ETH_DMADESCTypeDef DMATxDscrTab0;
__align(4)
    static uint8_t Rx_Buff0[ETH_RX_BUF_SIZE];
__align(4)
    static uint8_t Tx_Buff0[ETH_TX_BUF_SIZE];

//DMA descriptor and buffer variables for channel 1
__align(4)
    static ETH_DMADESCTypeDef DMARxDscrTab1;
__align(4)
    static ETH_DMADESCTypeDef DMATxDscrTab1;
__align(4)
    static uint8_t Rx_Buff1[ETH_RX_BUF_SIZE];
__align(4)
    static uint8_t Tx_Buff1[ETH_TX_BUF_SIZE];

//buffer pointer
static uint16_t Tx_CurPointer = ETH_HEADER;

//buffer descriptor
static ChannelDataTypeDef Rx_CH0;
static ChannelDataTypeDef Rx_CH1;

static DataPacketTypeDef Rx_DataPacket;

static inline void _reset_phy(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    int i = 840000;
    while (i--);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

static inline void _mac_address_config(void)
{
    int i = 0;
    uint32_t sn = *((uint32_t*)(0x1fff7a10));
    uint8_t mac[6];

    mac[0] = 2;
    mac[1] = 0;
    mac[2] = 0;
    mac[3] = ((uint8_t*)&sn)[2];
    mac[4] = ((uint8_t*)&sn)[1];
    mac[5] = ((uint8_t*)&sn)[0];

    //eth dst
    for (; i < 6; i++) {
        Tx_Buff0[i] = 0xff;
        Tx_Buff1[i] = 0xff;
    }

    //eth src
    for (i = 0; i < 6; i++) {
        Tx_Buff0[i + 6] = mac[i];
        Tx_Buff1[i + 6] = mac[i];
    }
    ETH_MACAddressConfig(ETH_MAC_Address0, mac);
}

static inline void _buffer_config(void)
{
    Rx_CH0.BufferAddr = 0;
    Rx_CH0.length     = 0;
    Rx_CH0.status     = DRV_INV;
    
    Rx_CH1 = Rx_CH0;
    
    Rx_DataPacket.ch0 = &Rx_CH0;
    Rx_DataPacket.ch1 = &Rx_CH1;
    
    //ETH buffer configuration for channel 0
    ETH_DMATxDescChainInit(&DMATxDscrTab0, Tx_Buff0, 1);
    ETH_DMARxDescChainInit(&DMARxDscrTab0, Rx_Buff0, 1);
    
    DMATxDscrTab0.Status |=ETH_DMATxDesc_FS|ETH_DMATxDesc_LS;
    DMATxDscrTab1.Status |=ETH_DMATxDesc_FS|ETH_DMATxDesc_LS;
    DMATxDscrTab0.Status &=~ ETH_DMATxDesc_OWN;
    DMATxDscrTab1.Status &=~ ETH_DMATxDesc_OWN;
    
    DMARxDscrTab0.Status &=~ETH_DMARxDesc_OWN;
    DMARxDscrTab0.Status &=~ETH_DMARxDesc_OWN;
    
    DMATxDscrTab0.ControlBufferSize = 0;
    DMATxDscrTab1.ControlBufferSize = 0;

    //ETH buffer configuration for channel 1
    //todo
}

static inline void _gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);

    /* Pin configuration of RMII mode
    ETH_MDIO -------------------------> PA2
    ETH_MDC --------------------------> PC1
    ETH_RMII_REF_CLK------------------> PA1
    ETH_RMII_CRS_DV ------------------> PA7
    ETH_RMII_RXD0 --------------------> PC4
    ETH_RMII_RXD1 --------------------> PC5
    ETH_RMII_TX_EN -------------------> PB11
    ETH_RMII_TXD0 --------------------> PB12
    ETH_RMII_TXD1 --------------------> PB13
    ETH_RESET-------------------------> PB14*/

    //config PA1 PA2 PA7
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

    //config PB11, PB12, PB13 and PB14
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);
    //PB14 is used for hardware reset

    //config PC1,PC4 and PC5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

    //traffic signal PA6
    //for ethernet, the signal is provided by RX_DV or CRS_DV
    //which indicates that a frame is receiving
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //hardware reset the ETH PHY
    //_reset_phy();
}

static inline void _mac_config(void)
{
    ETH_InitTypeDef ETH_InitStructure;
    int rval = 0;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

    ETH_DeInit();
    ETH_SoftwareReset();
    while (ETH_GetSoftwareResetStatus() == SET)
        ;
    ETH_StructInit(&ETH_InitStructure);

    ///parameters configuration of ETH MAC
    ETH_InitStructure.ETH_AutoNegotiation             = ETH_AutoNegotiation_Enable; //ETH_AutoNegotiation_Disable Enable

    ETH_InitStructure.ETH_Mode                        = ETH_Mode_FullDuplex;
    ETH_InitStructure.ETH_Speed                       = ETH_Speed_100M;

    ETH_InitStructure.ETH_LoopbackMode                = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission           = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip        = ETH_AutomaticPadCRCStrip_Enable;

    ETH_InitStructure.ETH_ReceiveAll                  = ETH_ReceiveAll_Enable;
    ETH_InitStructure.ETH_BroadcastFramesReception    = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_PromiscuousMode             = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter       = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter         = ETH_UnicastFramesFilter_Perfect;

    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Disable;
    ETH_InitStructure.ETH_ReceiveStoreForward         = ETH_ReceiveStoreForward_Enable;
    ETH_InitStructure.ETH_TransmitStoreForward        = ETH_TransmitStoreForward_Disable;
    ETH_InitStructure.ETH_TransmitThresholdControl    = ETH_TransmitThresholdControl_16Bytes;  // as fast as possible

    ETH_InitStructure.ETH_ForwardErrorFrames          = ETH_ForwardErrorFrames_Disable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
    ETH_InitStructure.ETH_SecondFrameOperate          = ETH_SecondFrameOperate_Disable;
    ETH_InitStructure.ETH_AddressAlignedBeats         = ETH_AddressAlignedBeats_Enable;
    ETH_InitStructure.ETH_FixedBurst                  = ETH_FixedBurst_Enable;
    ETH_InitStructure.ETH_RxDMABurstLength            = ETH_RxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_TxDMABurstLength            = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration              = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    rval = ETH_Init(&ETH_InitStructure, LAN8720_PHY_ADDRESS);
    TTP_ASSERT(rval == ETH_SUCCESS);
    
    _mac_address_config();
    _buffer_config();
    
    uint32_t x1 = ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BSR);
    uint32_t x2 = ETH_ReadPHYRegister(LAN8720_PHY_ADDRESS, PHY_BCR);
    
    //enable transimission
    ETH_MACTransmissionCmd(ENABLE);
    ETH_FlushTransmitFIFO();
    ETH_DMATransmissionCmd(ENABLE); 
    //enable reception
    ETH_MACReceptionCmd(ENABLE);
    ETH_DMAReceptionCmd(ENABLE);
}

void DRV_DepInit(void)
{
    _gpio_config();   
    _mac_config();
}
void DRV_Reset(void)
{
    //reset the PHY
    _reset_phy();
    //reset the MAC
    ETH_SoftwareReset();
    Tx_CurPointer = ETH_HEADER;
}

void DRV_StartTransmission(void)
{
    ETH->DMATPDR = 0;   
}
void DRV_StopTransmission(void)
{
    DMARxDscrTab0.Status = 0;
    DMARxDscrTab1.Status = 0;
}

void DRV_StartReception(void)
{
    //TTP_TGUARD((DMARxDscrTab0.Status & ETH_DMARxDesc_OWN) == (uint32_t)RESET);

    DMARxDscrTab0.Status = ETH_DMARxDesc_OWN;
    DMARxDscrTab1.Status = ETH_DMARxDesc_OWN;
    
    ETH->DMARPDR = 0; 
}
void DRV_StopReception(void)
{
    //DMARxDscrTab0.Status = 0;
    //DMARxDscrTab1.Status = 0;
}

void DRV_RxClear(void)
{
    DMARxDscrTab0.Status = 0;
    DMARxDscrTab1.Status = 0;
}

int DRV_PushData(const uint8_t* DataAddr, int size)
{
    int i = Tx_CurPointer;
    while(size--)
    {
        i = Tx_CurPointer;
        Tx_Buff0[i] = *DataAddr;
        //Tx_Buff1[i] = *DataAddr; //todo
        Tx_CurPointer++;
        DataAddr++;
        if(Tx_CurPointer>ETH_TX_BUF_SIZE)
        {
            Tx_CurPointer = ETH_HEADER;
            return DRV_ERR;
        }
    }
    return DRV_OK;
}

void DRV_PrepareToTransmit(void)
{
    //TTP_TGUARD((DMATxDscrTab0.Status & ETH_DMATxDesc_OWN) == (uint32_t)RESET);    
    //time delta shall be less than 539microticks between two successor transmission
    
    DMATxDscrTab0.ControlBufferSize = Tx_CurPointer & ETH_DMATxDesc_TBS1;
    DMATxDscrTab1.ControlBufferSize = Tx_CurPointer & ETH_DMATxDesc_TBS1;    
    DMATxDscrTab0.Status |= ETH_DMATxDesc_OWN;
    DMATxDscrTab1.Status |= ETH_DMATxDesc_OWN;
    
    uint16_t length = Tx_CurPointer - ETH_HEADER;
    
    Tx_Buff0[12] = length>>8;
    Tx_Buff0[13] = length&0x00ff;
    
    Tx_CurPointer = ETH_HEADER; 
    
    //todo for Tx_Buffer1  
}

int DRV_CheckTransmitted(void)
{
    TTP_TGUARD((DMATxDscrTab0.Status & ETH_DMATxDesc_OWN) == (uint32_t)RESET);
    
    if(ETH_GetDMATxDescFlagStatus(&DMATxDscrTab0,ETH_DMATxDesc_ES)==SET)
        return DRV_ERR;
    
    return DRV_OK;
}

int DRV_CheckReceived(int channel)
{
    //todo for different channels checking
    FrameTypeDef frame;
    uint32_t res = ETH_CheckFrameReceived();
    
    if(res==1)
    {
        //a frame received
        if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab0,ETH_DMARxDesc_ES)==SET)
        {
            Rx_CH0.BufferAddr = 0;//NULL
            Rx_CH0.length     = 0;
            
            if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab0,ETH_DMARxDesc_LE)==SET)
                Rx_CH0.status = LTH_ERR;
            else if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab0,ETH_DMARxDesc_CE)==SET)
                Rx_CH0.status = CRC_ERR;
            else if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab0,ETH_DMARxDesc_LC)==SET)
                Rx_CH0.status = RXD_COL;
            else
                Rx_CH0.status = PHY_ERR;
        }
        else
        {
            frame = ETH_Get_Received_Frame();
            Rx_CH0.BufferAddr = Rx_Buff0 + ETH_HEADER; //substruct the 14 bytes of the ETH header
            Rx_CH0.length = frame.length;
            Rx_CH0.status = DRV_OK;
        }
        
        Rx_CH1 = Rx_CH0;
    }
    /* only one channel in current implementation
    if(rval_ch1)
    {
        if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab1,ETH_DMARxDesc_ES)==SET)
        {
            Rx_CH1.BufferAddr = 0;//NULL
            Rx_CH1.length     = 0;
            
            if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab1,ETH_DMARxDesc_LE)==SET)
                Rx_CH1.status = LTH_ERR;
            else if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab1,ETH_DMARxDesc_CE)==SET)
                Rx_CH1.status = CRC_ERR;
            else if(ETH_GetDMARxDescFlagStatus(&DMARxDscrTab1,ETH_DMARxDesc_LC)==SET)
                Rx_CH1.status = RXD_COL;
            else
                Rx_CH1.status = PHY_ERR;
        }
        else
        {
            frame = ETH_Get_Received_Frame();
            Rx_CH1.BufferAddr = Rx_Buff1 + ETH_HEADER; //substruct the 14 bytes of the ETH header
            Rx_CH1.length = frame.length - ETH_HEADER;
            Rx_CH1.status = DRV_OK;
        }
    }
    */

    return res;
}

int DRV_IsChannelActive()
{
    // to do for channel 1
    return (GPIOA->IDR & GPIO_Pin_6);
}

DataPacketTypeDef* DRV_PullData(void)
{ 
    return &Rx_DataPacket;
}
