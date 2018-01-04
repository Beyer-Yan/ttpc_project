/**
  ******************************************************************************
  * @file    	medl.c
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.11.6
  * @brief   	This file implemets the basic interface of the MEDL
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */		
#include "medl.h"
#include "ttpmac.h"
#include "ttpdebug.h"
#include "crc.h"

/* 
 * @see MEDL specification document
 */
static volatile medl_header_t       __G_medl_header     __SECTION("PV_SECTION");
static volatile mode_discriptor_t   __G_mode_discriptor __SECTION("PV_SECTION");

static const uint8_t*               __G_medl_base_addr  __SECTION("PV_SECTION");

/**
 * variables below are used to buffer the frequently-used parameters.
 */
static volatile ScheduleParameter_t __G_sched    __SECTION("PV_SECTION");
static volatile NodeProperty_t      __G_role     __SECTION("PV_SECTION");

static volatile RoundSlotProperty_t __G_slot     __SECTION("PV_SECTION");

static volatile uint32_t            __G_sched_id __SECTION("PV_SECTION");
static volatile uint32_t            __G_app_id   __SECTION("PV_SECTION");

/**
 * this function is declared in the low-level hardware-relative file and is used to 
 * get the base address of the specific MEDL memory region.
 * @return      non
 * @attention   little endian memory mode is required.
 */
extern const uint8_t* medl_get_base_addr(void);

static inline void _byte_copy(volatile void* dst, const void* src,int size)
{
	volatile uint8_t* _d = (volatile uint8_t*)dst;
	const uint8_t*    _s = (const uint8_t*)src;

	while(size-->0)
	{
		*_d++ = *_s++;
	}
}

static inline void __medl_header_extract(void)
{
	/** read header from the 0 address relative the base addr */
	_byte_copy(&__G_medl_header, __G_medl_base_addr, MEDL_HEADER_SIZE);
}
static void __medl_sched_extract(void)
{
	const uint8_t *buf = __G_medl_base_addr + SCHED_REGION_OFFSET; //28 bytes
    
	_byte_copy(&__G_sched, buf,SCHED_REGION_SIZE);
}

static void __medl_role_extract(void)
{
	const uint8_t* buf = __G_medl_base_addr + ROLE_REGION_OFFSET;

	_byte_copy(&__G_role, buf,ROLE_REGION_SIZE);
}

static void __medl_id_extract(void)
{
	const uint8_t *buf = __G_medl_base_addr + ID_REGION_OFFSET;

	_byte_copy(&__G_sched_id, buf, 4);
	_byte_copy(&__G_app_id, buf+4, 4);
}

static void __medl_mode_extract(uint32_t mode_num)
{
    uint32_t _mode_discritor_addr;
	const uint8_t* buf = __G_medl_base_addr + MODE_ENTRY_OFFSET + MODE_ENTRY_SIZE*mode_num;
    
    _byte_copy(&_mode_discritor_addr,buf,MODE_ENTRY_SIZE);

    //get the base address of the mode_discriptor
    buf = __G_medl_base_addr + _mode_discritor_addr;
    
    //at least 12 bytes, including al least two tdma_offsets
    _byte_copy(&__G_mode_discriptor,buf,12);
}

static inline void _slot_property_extract(const uint8_t* buf)
{
	/**
	 * @attention upper program should guarantee the legality of the parameters,
	 * mode and round_slot.
	 */
	_byte_copy(&__G_slot, buf,SLOT_SIZE);
}

/** be sure that the current mode is updated */
static inline const uint8_t* __medl_get_slot_entry(uint32_t mode, uint32_t round_slot)
{
	return __G_medl_base_addr + __G_mode_discriptor.slot_addr + SLOT_SIZE*round_slot;
}

static uint32_t __medl_crc32_check(void)
{
	uint32_t crc32_tmp;
	uint32_t crc_origion;

	_byte_copy(&crc_origion,__G_medl_base_addr+__G_medl_header.total_size-4,4);
	CRC_ResetData();
	CRC_PushData(__G_medl_base_addr,__G_medl_header.total_size-4);
	crc32_tmp = CRC_GetResult();

	return crc_origion==crc32_tmp;
}

/**
 * Init the header area of the medl and chedk the crc32
 * @return  @arg 0 init error due to the hardware error or crc32 error
 *          @arg 1 init success
 */
uint32_t MEDL_Init(void)
{
	uint32_t res = 1;

	__G_medl_base_addr = medl_get_base_addr();
	TTP_ASSERT(__G_medl_base_addr!=NULL);

	__medl_header_extract();
	//res = __medl_crc32_check();
	//if(res==0){
	//	return res;
	//}

	/** buffer the frequently-used parameters. */
	/** The schedule parameter will be permanant when power on */
	__medl_sched_extract();
	/** The role parameter of a node will be permanant when power on */
	__medl_role_extract();
	__medl_id_extract();

	/** pre-extract the first mode, cold start mode*/
	__medl_mode_extract(MODE_CS_ID);

	//pre-extract the slot property of the node own.
	uint32_t _slot = __G_role.LogicalNameSlotPosition;
	const uint8_t* buf = __medl_get_slot_entry(MODE_CS_ID,_slot);
	_slot_property_extract(buf);

 	return res;
}

void* MEDL_GetRoleAddr(void)
{
	return (void*)&__G_role;
}

void* MEDL_GetScheduleAddr(void)
{
	return (void*)&__G_sched;
}

void* MEDL_GetRoundSlotAddr(uint32_t ModeNum, uint32_t RoundSlot)
{
	if(ModeNum>=__G_medl_header.mode_region_num)
		return NULL;
	
	__medl_mode_extract(ModeNum);
	
	if(RoundSlot>=__G_mode_discriptor.round_slots)
		return NULL;
	
	_slot_property_extract(__medl_get_slot_entry(ModeNum,RoundSlot));

	return ((void*)&__G_slot);
}

uint32_t MEDL_GetSchedID(void)
{
	return (__G_sched_id);
}

uint32_t MEDL_GetAppID(void)
{
	return (__G_app_id); 
}
#warning "legality of parameters passed in shall be checked outside the function"
uint32_t MEDL_GetRoundCycleLength(uint32_t ModeNum)
{
	uint32_t _mode_discritor_addr;
    uint16_t length; 
	const uint8_t* buf;

    buf = __G_medl_base_addr + MODE_ENTRY_OFFSET + MODE_ENTRY_SIZE*ModeNum;
    _byte_copy(&_mode_discritor_addr,buf,MODE_ENTRY_SIZE);

    buf = __G_medl_base_addr + _mode_discritor_addr;
    
	_byte_copy(&length,buf,2);
	return length;
}
#warning "legality of parameters passed in shall be checked outside the function"
uint32_t MEDL_GetTDMACycleLength(uint32_t ModeNum)
{
	uint32_t _mode_discritor_addr;
    uint16_t length; 
	const uint8_t* buf;

    buf = __G_medl_base_addr + MODE_ENTRY_OFFSET + MODE_ENTRY_SIZE*ModeNum;
    _byte_copy(&_mode_discritor_addr,buf,MODE_ENTRY_SIZE);

    buf = __G_medl_base_addr + _mode_discritor_addr;
    
	_byte_copy(&length,buf+2,2);
	return length;
}

#warning "legality of parameters passed in shall be checked outside the function"
uint32_t MEDL_GetTDMAOffsetValue(uint32_t ModeNum, uint32_t TDMARound)
{
    uint32_t _mode_discritor_addr;
    uint16_t offset; 
	const uint8_t* buf;

    buf = __G_medl_base_addr + MODE_ENTRY_OFFSET + MODE_ENTRY_SIZE*ModeNum;
    _byte_copy(&_mode_discritor_addr,buf,MODE_ENTRY_SIZE);

    //get the tdma_offset address
    buf = __G_medl_base_addr + _mode_discritor_addr + 8 + TDMARound*2;
    _byte_copy(&offset,buf,2);
    return offset;
}
