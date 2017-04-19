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
#include "virhw.h"
#include "ttpc_mac.h"
#include "ttpdebug.h"

#error "to be tested"

static volatile medl_header_t       __G_medl_header;

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
 * read the specified number bytes from the specific MEDL memory region into the buf.
 * @param  buf    the buffer region
 * @param  size   the size to read
 * @param  offset the address offset relative  to the base address of the medl region
 * @return      @arg 0  reading failed due to hardware fault
 *              @arg 1  reading success
 * @attention   little endian memory mode is required.
 */
extern int medl_real_read(char *buf, int size, int offset);
//extern void* get_medl_base_addr(void);

static void __medl_header_extract(void)
{
	int res = 0;

	/** read header from the 0 address relative the base addr */
	res = medl_real_read(&__G_medl_header, MEDL_HEADER_SIZE, 0);

	TTP_ASSERT(res!=0);
}

static void __medl_sched_extract(void)
{
	uint32_t buf[10] = {0};
	uint32_t res     = 0;

	res = medl_real_read((char*)buf,
						 __G_medl_header.sched_region_size,
						 __G_medl_header.sched_region_addr);

	TTP_ASSERT(res!=0);

	__G_sched.ColdStartAllow               = (buf[0]&(1<<0))>>0;
	__G_sched.ColdStartIntegrationAllow    = (buf[0]&(1<<1))>>1;
	__G_sched.ExternalRateCorrectionAllow  = (buf[0]&(1<<2))>>2;
	__G_sched.MinimumIntegrationCount      = (buf[0]&(0xff<<8))>>8;
	__G_sched.MaximumColdStartEntry        = (buf[0]&(0xff<<16))>>16;
	__G_sched.MximumMembershipFailureCount = (buf[0]&(0xff<<24))>>24;
	__G_sched.MacrotickParameter           = buf[1];
	__G_sched.Precision                    = buf[2];
	__G_sched.CommunicationRate            = buf[3];
	
	__G_sched.StartupTimeout               = buf[4];
	__G_sched.ListenTimeout                = buf[5];
	__G_sched.ColdStartTimeout             = buf[6];
}

static void __medl_role_extract(void)
{
	uint32_t buf[10] = {0};
	uint32_t res     = 0;

	res = medl_real_read((char*)buf,
						 __G_medl_header.role_region_size,
						 __G_medl_header.role_region_addr);

	TTP_ASSERT(res!=0);
	__G_role.LogicalNameSlotPosition       = (buf[0]&(0xffff<<16))>>16;
	__G_role.LogicalNameMulplexedID    	   = (buf[0]&0xffff);
	__G_role.PassiveFlag               	   = (buf[1]&(1<<0))>>0;				
	__G_role.MultiplexedMembershipFlag 	   = (buf[1]&(1<<1))>>1;
	__G_role.FlagPosition              	   = (buf[1]&(0xff<<8))>>8;				
	__G_role.SendDelay                 	   = buf[2];	
}

static void __medl_id_extract(void)
{
	uint32_t buf[10] = {0};
	uint32_t res     = 0;

	res = medl_real_read((char*)buf,
						 __G_medl_header.id_region_size,
						 __G_medl_header.id_region_addr);

	TTP_ASSERT(res!=0);
	__G_sched_id = buf[0];
	__G_app_id   = buf[1];
}

static void __medl_slot_extract(uint32_t mode, uint32_t round_slot)
{
	uint32_t buf[10] = {0};
	uint32_t res = 0;
	uint32_t offset = __G_medl_header.slot_mode_addr[mode] + 
	                  __G_medl_header.slot_size * round_slot;
	/**
	 * @attention upper program should gurantee the legality of the parameters,
	 * mode and round_slot.
	 */
	res = medl_real_read((char*)buf,
						 __G_medl_header.slot_size,
						 offset);
						 
	TTP_ASSERT(res!=0);
	__G_slot.LogicalSenderSlot        = (buf[0]&(0xffff<<16))>>16;
	__G_slot.LogicalSenderMultiplexID = (buf[0]&0xffff);
	__G_slot.SlotDuration             = buf[1];	
	__G_slot.TransmissionDuration     = buf[2];
	__G_slot.DelayCorrectionTerms     = buf[3];	
	__G_slot.CNIAddressOffset         = buf[4];	
	__G_slot.AppDataLength            = (buf[5]&0xff);
	__G_slot.FlagPosition             = (buf[5]&(0xff<<8))>>8;	
	__G_slot.FrameType                = (buf[5]&(1<<16))>>16;
	__G_slot.ModeChangePermission     = (buf[5]&(1<<17))>>17;
	__G_slot.ReintegrationAllow       = (buf[5]&(1<<18))>>18;
	__G_slot.ClockSynchronization      = (buf[5]&(1<<19))>>19;
	__G_slot.SynchronizationFrame     = (buf[5]&(1<<20))>>20;
	__G_slot.AtTime                   = buf[6];
}

static uint32_t ___medl_crc32_check(void)
{
	uint32_t res    = 0;
	uint32_t length = 0;
	uint32_t remain = 0;

	uint32_t crc32_tmp;

	uint32_t data;

	int i = 0;

	/** remove the last 4 bytes crc32 code */
	remain = (MEDL_HEADER_SIZE-4)%4;

	if(remain!=0)
	{
		/** medl region shall be 4 bytes aligned */
		return 0;
	}

	length = (MEDL_HEADER_SIZE-4)/4;
	for(;i<length;i++)
	{
		res = medl_real_read((char*)&data,sizeof(uint32_t),i*sizeof(uint32_t));
		TTP_ASSERT(res!=0);

		crc32_tmp = CRC_CalcBlock(&data,1);
	}

	/** read the crc32 region of the medl */
	res = medl_real_read((char*)&data,
						 sizeof(uint32_t),
						 __G_medl_header.crc_32_region_addr);

	TTP_ASSERT(res!=0);

	return (res=(data==crc32_tmp));
}

/**
 * Init the header area of the medl and chedk the crc32
 * @return  @arg 0 init error due to the hardware error or crc32 error
 *          @arg 1 init success
 */
uint32_t MEDL_Init(void)
{
	uint32_t res = 0;

	__medl_header_extract();

	res = __medl_crc32_check();

	if(res==0)
	{
		return res;
	}

	/**
	 * buffer the frequently-used parameters.
	 */
	
	/** The schedule parameter is unchangable when power on */
	__medl_sched_extract();

	/** The role parameter of a node is still unchangable when power on */
	__medl_role_extract();

	__medl_id_extract();
 	
 	return res;
}

/**
 * This function will return the corresbonding region by returning its address.
 * So, correct pointer casting should be performed when this function is called.
 * @param RegionType the region type
 * @return the address of the corresbonding region
 * @attention DO NOT GET THE ID REGION USING THIS FUNCTION BECAUSE YOU HAVE BETTER
 *            APPROACH TO DO THE SAME THING, THAT IS, TO CALL MEDL_GetSchedID AND
 *            MEDL_GetAppID.
 */
uint32_t MEDL_GetRegionAddr(uint32_t RegionType)
{
	uint32_t addr;

	switch(RegionType)
	{
		case SCHEDULE_REGION : addr = (uint32_t)&__G_sched; 	break;
		case ROLE_REGION     : addr = (uint32_t)&__G_role;  	break;
		case ID_REGION       : addr = (uint32_t)&__G_sched_id;  break;
		case default         : addr = 0;      
	}
	return (addr);
}

void* MEDL_GetRoundSlotAddr(uint32_t ModeNum, uint32_t TDMARound, uint32_t Slot)
{
	static uint32_t _ModeNum   = -1;
	static uint32_t _TDMARound = -1;
	static uint32_t _Slot      = -1;
	uint32_t _round_slot;
	/**
	 * Only in the first time are the slot properties extracted.
	 */
	if((_ModeNum!=ModeNum)||(_TDMARound!=TDMARound)||(_Slot!=Slot))
	{
		_ModeNum    = ModeNum;
		_TDMARound  = TDMARound;
		_Slot       = Slot;
		_round_slot = _TDMARound * __G_medl_header.tdma_slots + _Slot;

		__medl_slot_extract(_Mode,_round_slot);
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
	return __G_medl_header.slot_mode_size[ModeNum];
}

uint32_t MEDL_GetTDMACycleLength(uint32_t ModeNum)
{
	/**
	 * the parameter ModeNum is not used here temporarily. We assume that 
	 * the TDMA slots of all modes are the same.
	 */
	return __G_medl_header.tdma_slots;
}