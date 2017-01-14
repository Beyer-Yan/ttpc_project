/**
  ******************************************************************************
  * @file    	ack.c
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.11
  * @brief   	implementation of acknowledgment algorithm of TTPC
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file provides an implementation of the acknowledgment algorithm of ttpc
  * controller. The function "uint32_t MAC_AckFrame", seeing that from file 
  * /mac/ttpc_mac.h, line 369, is the interface of the acknowledgment algorithm,
  * which shall be called at the start of PRP phase for frame acknowledgment.
  * @see AS6003, page 18/56.
  * 
  ******************************************************************************
  */
 
#include "ttpc_mac.h"
#include "ttpcdebug.h"
#include "virhw.h"
 
