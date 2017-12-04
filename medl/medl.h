/**
  ******************************************************************************
  * @file    	medl.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.09
  * @brief   	The interfaces of the MEDL
  ******************************************************************************
  * @attention
  *
  * @desc
  *
  * 
  ******************************************************************************
  */
 #ifndef __MEDL_H__
 #define __MEDL_H__

#include "ttpdef.h"
     
#define MODE_NUM            3

/**
 * The header is part of the medl, meaning that the real addr offset of the medl
 * IS the MEDL_BASE_ADDR + MEDL_HEADER_SIZE. Every address offset of the region of
 * the medl is relative to the MEDL_BASE_ADDR, inlcuding the medl header. 
 */
#define MEDL_HEADER_SIZE    64    /**< 64 bytes */  

/**
 *                                                                                 
                            sched_region_size                                   
                  /--------------------------------------\                      
 sched_region_addr+--------------------------------------+                      
                  |          SCHEDULE REGION             |                      
                  |                                      |                      
                  +--------------------------------------+                      
                                                                                
                            role_region_size                                    
                  /--------------------------------------\                      
 role_region_addr +--------------------------------------+                      
                  |          ROLE REGION                 |                      
                  |                                      |                      
                  +--------------------------------------+                      
                                                                                
                            id_region_size                                      
                  /--------------------------------------\                      
 id_region_addr   +--------------------------------------+                      
                  |          ID REGION                   |                      
                  |                                      |                      
                  +--------------------------------------+                      
                                                                                
                            slot_region_size                                    
                  /-----------------------------------------------------------\ 
 slot_region_addr +-----------------------------------------------------------+ 
                  |                                                           | 
                  |        slot_size      slot_mode_size                      | 
                  |         /---\           /                                 | 
                  |         +---+---+---+---|        Round Slot Region        | 
                  |  MODE 1 |   |   |   |   |               /                 | 
                  |         +---+---+---+---|               |                 | 
                  |                         |               |                 | 
                  |         +---+---+---+---|---+---+---+---|---+---+---+---+ | 
                  |  MODE 2 |   |   |   |   |   |   |   |   |   |   |   |   | | 
                  |         +---+---+---+---|---+---+---+---|---+---+---+---+ | 
                  |                         |               |                 | 
                  |         +---+---+---+---|---+---+---+---|                 | 
                  |  MODE X |   |   |   |   |   |   |   |   |                 | 
                  |         +---+---+---+---|---+---+---+---| TDMA Boundary   | 
                  |                         \               \                 | 
                  +-----------------------------------------------------------+ 
 */


/**
 * Attention that the header is part of the medl region. 
 */
typedef struct medl_header 
{
  /** the schedule addr offset relative to the medl base */
	uint32_t		sched_region_addr;
	uint32_t 		sched_region_size;

  /** the role addr offset relative to the medl base */
	uint32_t		role_region_addr;
	uint32_t 		role_region_size;

  /** the id addr offset relative to the medl base */
	uint32_t		id_region_addr;
	uint32_t 		id_region_size;

  /** the slot addr offset relative to the medl base */
  uint32_t    slot_region_addr;
  uint32_t    slot_region_size;

  /**
   * the memory address of the slot-mode-data structure. 
   * Each mode has its own address of the corresponding slot-
   * mode-data structure.
   */
  uint32_t    slot_mode_addr[MODE_NUM];
  /**
   * The numbers of slots for the corresponding mode. The size may
   * differ for different modes.
   */ 
  uint32_t    slot_mode_size[MODE_NUM];

  /**
   * The slot number of every TDMA round
   */
  uint32_t    tdma_slots;

  /**
   * The size of every slot  configuration data;
   */
  uint32_t    slot_size;

  /** the crc32 position of the medl region, 32 bits of course. */
  uint32_t    crc32_region_addr;

}medl_header_t; 


/** region type definition */
#define SCHEDULE_REGION       (uint32_t)0x00000001
#define ROLE_REGION           (uint32_t)0x00000002
#define ID_REGION             (uint32_t)0x00000003

/**
 * This function will be called when power on
 */
uint32_t  MEDL_Init(void);

/**
 * Get the corresbonding region address
 * @param  RegionType the region
 *           @arg SCHEDULE_REGION
 *           @arg ROLE_REGION
 * @return the region address
 */
void*     MEDL_GetRegionAddr(uint32_t RegionType);
uint32_t  MEDL_GetSchedID(void);
uint32_t  MEDL_GetAppID(void);

uint32_t  MEDL_GetRoundCycleLength(uint32_t ModeNum);
uint32_t  MEDL_GetTDMACycleLength(uint32_t ModeNum);

void* 	  MEDL_GetRoundSlotAddr(uint32_t ModeNum, uint32_t TDMARound, uint32_t Slot);

#endif 