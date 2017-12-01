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

/** CNI memory region allocation */
/*****************************************************************************/
uint32_t __ttpc_status_regs_group[16] __SECTION("CNI_STATUS_SECTION") = {0};

uint32_t __ttpc_control_notifier __SECTION("CNI_NOTIFY_SECTION") = {0};

uint32_t __ttpc_control_info[4] __SECTION("CNI_INFO_SECTION") = {0};

uint8_t msg_pool[MSG_POOL_LENGTH] __SECTION("CNI_MSG_SECTION") = {0};


/** Debug info memory region allocation */
/*****************************************************************************/
//char __Info_buf[INFO_SIZE] __SECTION("DBG_SECTION") = {0};
//char __Error_buf[ERROR_SIZE] __SECTION("DBG_SECTION") = {0};

/** Protocol data region  */
/*****************************************************************************/
uint8_t PV_data[20] __SECTION("PV_SECTION") = {0};

/** MEDL example region */
/*****************************************************************************/
static const uint8_t _medl_example_data[1024*4] __SECTION("MEDL_SECTION") =
{0}; 

//attention that the parameters legality shall be verified, but to be done.
int medl_real_read(char *buf, int size, int offset)
{
    char *tmp = (char*)(_medl_example_data+offset);
    while(size--)
        *buf++ = *tmp++;

    return 1;
}

/** other section  */




