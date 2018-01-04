/**
  ******************************************************************************
  * @file    	xfer.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2017.09
  * @brief   	This file defines the interfaces of the tranducer of ttpc
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
  * The XFER  module is  the bridge of the  physical  driver interface and the mac. 
  ******************************************************************************
  */
#ifndef __XFER_H__
#define __XFER_H__

#include <stdint.h>

/**
 * @defgroup Physical_transfer
 */
/**@{*/

/////////////////////////////////////////////////////////////////////////////
// transmission driver layer interfaces definitions                        //
/////////////////////////////////////////////////////////////////////////////


typedef struct
{
    int    status;
    int    length;
    uint8_t*     BufferAddr;
}ChannelDataTypeDef;

typedef struct
{
    ChannelDataTypeDef* ch0;
    ChannelDataTypeDef* ch1;
}DataPacketTypeDef;

/**
 * /@todo The other errors to be expanded
 */

/**
 * functions definitions
 */

void DRV_DepInit(void);
void DRV_Reset(void);

void DRV_StartTransmission(void);
void DRV_StopTransmission(void);
void DRV_StartReception(void);
void DRV_StopReception(void);

/**
 * This function clears the status of the receiver  
 */
void DRV_RxClear(void);

uint32_t DRV_PushData(const uint8_t* DataAddr,int size);
void DRV_PrepareToTransmit(void);

//void DRV_PrepareToTransmitOfCH0(uint8_t* BufferAddr, int length);
//void DRV_PrepareToTransmitOfCH1(uint8_t* BufferAddr, int length);

/**
 * This function checks a frame transmission according to
 * the status of the controller's transfer hardware
 * @return DRV_OK:transmission success, DRV_ERR:transmission failed.
 */
uint32_t DRV_CheckTransmitted(void);

/**
 * This function polls for a frame reception
 * @return 1:received farmes, 0:received nothing.
 */
uint32_t DRV_CheckReceived(int channel); 

/**
 * This function polls for a channel activity
 * @return !0: channel is active(a frame is receiving on any one of the two channels).
 *          0: channel is silent.
 */
uint32_t DRV_IsChannelActive(void);

DataPacketTypeDef* DRV_PullData(void);

// void DRV_SetCommRate(uint16_t Rate);

/**@}*/// end of group Physical_transfer

#endif
