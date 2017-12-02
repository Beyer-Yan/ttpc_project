/**
  ******************************************************************************
  * @file    	crc.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2017.12
  * @brief   	This file provides the CRC unit related operation interfaces
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
#ifndef __CRC_H__
#define __CRC_H__
#include "ttpdef.h"
/**
 * @defgroup EXTERNAL_CRC24_Unit
 */
/**@{*/

#warning "Incompatible CRC bits with AS6003 "

/////////////////////////////////////////////////////////////////////////////
// CRC24 interfaces definitions                                            //
/////////////////////////////////////////////////////////////////////////////

void     CRC_DepInit(void);
void     CRC_ResetData(void);
void     CRC_PushData(uint8_t *pBuf, uint32_t BufLength);

uint32_t CRC_GetResult(void);

/**@}*/// end of group EXTERNAL_CRC32	


#endif // !__CRC_H__
