/**
 ******************************************************************************
 * @file    	test.c
 * @author  	Beyer
 * @email   	sinfare@foxmail.com
 * @version 	v1.0.0
 * @date    	20170331
 * @brief   	the file implements the virtual host interface
 ******************************************************************************
 * @attention
 *   
 * @log
 *   
 *
 ******************************************************************************
 */
#include "host.h"
#include "ttpmac.h"
#include "lsens.h"
#include "msg.h"
#include "ttpdebug.h"

#include "ttpconstants.h"

static inline void _byte_copy(volatile void* dst, const void* src,int size)
{
	volatile uint8_t* _d = (volatile uint8_t*)dst;
	const uint8_t*    _s = (const uint8_t*)src;

	while(size-->0)
	{
		*_d++ = *_s++;
	}
}

void HOST_Init(void)
{
    TTP_HLSR = 1;
    TTP_CR0 &= CR_CO;
    
    //for a cold start time.
    #ifdef TTP_NODE0
        TTP_CR4 |= (300<<16);
    #endif
    
    HOST_ModeChange(DMC_MODE_1);
}

void HOST_Alive(void)
{
    TTP_HLSR = 1;
}

void HOST_PrepareData(void)
{
    RoundSlotProperty_t *pRS = MAC_GetRoundSlotProperties();
    uint16_t val = LSENSE_GetVal();
    
    uint8_t* addr = MSG_GetMsgAddr(pRS->CNIAddressOffset);
    
    (addr-1)[0] = STATUS_BIT_PATTERN;

    _byte_copy(addr,&val,2);
    
    INFO("val:%d",val);
}

void HOST_ModeChange(uint32_t mode)
{
    TTP_CR0 &=~ CR_MCR; 
    TTP_CR0 |=  MCR_MODE_1;
}
