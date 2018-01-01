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

//#error "to be tested"

static volatile medl_header_t       __G_medl_header;
static volatile mode_discriptor_t   __G_mode_discriptor;

static const uint8_t*               __G_medl_base_addr;

/**
 * variables below are used to buffer the frequently-used parameters.
 */
static volatile ScheduleParameter_t __G_sched;
static volatile NodeProperty_t      __G_role;

static volatile RoundSlotProperty_t __G_slot;

static volatile uint32_t            __G_sched_id;
static volatile uint32_t            __G_app_id;

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

	__G_sched.ColdStartAllow               =  buf[0]&0x01;
	__G_sched.ColdStartIntegrationAllow    = (buf[0]&0x02)>>1;
	__G_sched.ExternalRateCorrectionAllow  = (buf[0]&0x04)>>2;
	__G_sched.MinimumIntegrationCount      = buf[1];
	__G_sched.MaximumColdStartEntry        = buf[2];
	__G_sched.MaximumMembershipFailureCount= buf[3];

	_byte_copy(&__G_sched.MacrotickParameter, buf + 4, 4);
	_byte_copy(&__G_sched.Precision, buf + 8, 4);
	_byte_copy(&__G_sched.ArrivalTimingWindow, buf + 12,4);
	_byte_copy(&__G_sched.StartupTimeout, buf + 16,4);
	_byte_copy(&__G_sched.ListenTimeout, buf + 20,4);
	_byte_copy(&__G_sched.ColdStartTimeout, buf + 24,4);
}

static void __medl_role_extract(void)
{
	const uint8_t* buf = __G_medl_base_addr + ROLE_REGION_OFFSET;

	_byte_copy(&__G_role.LogicalNameMultiplexedID, buf,2);
	__G_role.LogicalNameSlotPosition &= 0x0000ffff;

	_byte_copy(&__G_role.LogicalNameSlotPosition, buf+2,2);
	__G_role.LogicalNameMultiplexedID &= 0x0000ffff;

	__G_role.PassiveFlag               	   = buf[4]&0x01;				
	__G_role.MultiplexedMembershipFlag 	   = (buf[4]&0x02)>>1;
	__G_role.FlagPosition              	   = buf[5];		

	_byte_copy(&__G_role.SendDelay, buf+6,2);
}

static void __medl_id_extract(void)
{
	const uint8_t *buf = __G_medl_base_addr + ID_REGION_OFFSET;

	_byte_copy(&__G_sched_id, buf, 4);
	_byte_copy(&__G_app_id, buf+4, 4);
}

static void __medl_mode_extract(uint32_t mode)
{
	const uint8_t* buf = __G_medl_base_addr + MODE_DSCR_OFFSET + MODE_DSCR_SIZE*mode;
	_byte_copy(&__G_mode_discriptor,buf,MODE_DSCR_SIZE);
}

static inline void _slot_property_extract(const uint8_t* buf)
{
	/**
	 * @attention upper program should guarantee the legality of the parameters,
	 * mode and round_slot.
	 */
	_byte_copy(&__G_slot.LogicalSenderMultiplexID, buf,2);
	__G_slot.LogicalSenderSlot &= 0x0000ffff;

	_byte_copy(&__G_slot.LogicalSenderSlot, buf+2,2);
	__G_slot.LogicalSenderMultiplexID &= 0x0000ffff;

	_byte_copy(&__G_slot.SlotDuration, buf+4,2);
	__G_slot.SlotDuration &= 0x0000ffff;

	__G_slot.PSPDuration              = buf[6];
	__G_slot.TransmissionDuration     = buf[7];

	_byte_copy(&__G_slot.DelayCorrectionTerms, buf+8,2);
	__G_slot.DelayCorrectionTerms &= 0x0000ffff;

	__G_slot.AppDataLength            = buf[10];
	_byte_copy(&__G_slot.CNIAddressOffset, buf+12,2);
	
	__G_slot.FlagPosition             = buf[16];	
	__G_slot.FrameType                = buf[17]&0x01;
	__G_slot.ModeChangePermission     = (buf[17]&0x02)>>1;
	__G_slot.ReintegrationAllow       = (buf[17]&0x04)>>2;
	__G_slot.ClockSynchronization     = (buf[17]&0x08)>>3;
	__G_slot.SynchronizationFrame     = (buf[17]&0x10)>>4;
	
	_byte_copy(&__G_slot.AtTime, buf+18,2);
	__G_slot.AtTime  &= 0x0000ffff;
}

/** be sure that the current mode is updated */
static inline const uint8_t* __medl_get_slot_entry(uint32_t mode, uint32_t round_slot)
{
	return __G_medl_base_addr + __G_mode_discriptor.mode_addr + SLOT_SIZE*round_slot;
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
	static uint32_t _modeNum   = 0xffffffff;
	static uint32_t _slot      = 0xffffffff;
	/**
	 * Only in the first time are the slot properties extracted.
	 */
	if(_modeNum!=ModeNum){
        
        if(ModeNum>=__G_medl_header.mode_region_num)
            return NULL;
        
		_modeNum = ModeNum;
		__medl_mode_extract(_modeNum);
        
        if(RoundSlot>=__G_mode_discriptor.round_slots)
			return NULL;
        
        _slot = RoundSlot;
		_slot_property_extract(__medl_get_slot_entry(_modeNum,_slot));
        
	}else if(_slot!=RoundSlot){
		
		if(RoundSlot>=__G_mode_discriptor.round_slots)
			return NULL;

		_slot = RoundSlot;
		const uint8_t* buf = __medl_get_slot_entry(_modeNum,_slot);
		_slot_property_extract(buf);
	}

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

uint32_t MEDL_GetRoundCycleLength(uint32_t ModeNum)
{
	uint16_t len;
	const uint8_t* buf = __G_medl_base_addr + MODE_DSCR_OFFSET + MODE_DSCR_SIZE*ModeNum;
	_byte_copy(&len,buf+4,2);
	return len;
}

uint32_t MEDL_GetTDMACycleLength(uint32_t ModeNum)
{
	uint16_t len;
	const uint8_t* buf = __G_medl_base_addr + MODE_DSCR_OFFSET + MODE_DSCR_SIZE*ModeNum;
	_byte_copy(&len,buf+6,2);
	return len;
}
