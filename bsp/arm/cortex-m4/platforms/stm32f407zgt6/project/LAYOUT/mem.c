/**
 ******************************************************************************
 * @file    	mem.c
 * @author  	Beyer
 * @email   	sinfare@foxmail.com
 * @version 	v1.0.0
 * @date    	2018.11
 * @brief   	static memory allocation
 ******************************************************************************
 * @attention
 *   
 * @log
 *   
 *
 ******************************************************************************
 */
#include "ttpdef.h"
#include "protocol_data.h"

/** CNI memory region allocation */
/*****************************************************************************/
uint32_t __ttpc_status_regs_group[16] __SECTION("CNI_STATUS_SECTION") = {0};
uint32_t __ttpc_control_notifier __SECTION("CNI_NOTIFY_SECTION") = {0};
uint32_t __ttpc_control_info[4] __SECTION("CNI_INFO_SECTION") = {0};
uint8_t msg_pool[MSG_POOL_LENGTH] __SECTION("CNI_MSG_SECTION") = {0};

/** Protocol data region  */
/*****************************************************************************/
uint8_t PV_data[20] __SECTION("PV_SECTION") = {0};
