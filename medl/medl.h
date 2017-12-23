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
     
/**
 * The header is part of the medl, meaning that the real addr offset of the medl
 * IS the MEDL_BASE_ADDR + MEDL_HEADER_SIZE. Every address offset of the region of
 * the medl is relative to the MEDL_BASE_ADDR, inlcuding the medl header. 
 */
#define MEDL_HEADER_SIZE    28    /**< 28 bytes */
#define SCHED_REGION_SIZE   28
#define ROLE_REGION_SIZE    8
#define ID_REGION_SIZE      8  
#define MODE_DSCR_SIZE      8
#define SLOT_SIZE           20    

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
    uint32_t    total_size;
    /** the schedule addr offset relative to the medl base */
	uint32_t	sched_region_addr;

    /** the role addr offset relative to the medl base */
	uint32_t	role_region_addr;

    /** the id addr offset relative to the medl base */
	uint32_t	id_region_addr;

    /** the slot addr offset relative to the medl base */
    uint32_t    mode_region_addr;
    uint32_t    mode_region_num;

    /** the crc32 position of the medl region, 32 bits of course. */
    uint32_t    crc32_region_addr;

}medl_header_t; 

typedef struct mode_discriptor
{
    uint32_t mode_addr;
    uint32_t round_slots;
    uint32_t tdma_slots;

}mode_discriptor_t;

/** region type definition */
#define SCHEDULE_REGION       (uint32_t)0x00000001
#define ROLE_REGION           (uint32_t)0x00000002
#define ID_REGION             (uint32_t)0x00000003
#define MODE_REGION           (uint32_t)0x00000004

/**
 * This function will be called when power on
 */
uint32_t  MEDL_Init(void);

uint32_t  MEDL_GetSchedID(void);
uint32_t  MEDL_GetAppID(void);

uint32_t  MEDL_GetRoundCycleLength(uint32_t ModeNum);
uint32_t  MEDL_GetTDMACycleLength(uint32_t ModeNum);

void* MEDL_GetRoleAddr(void);
void* MEDL_GetScheduleAddr(void);
void* MEDL_GetRoundSlotAddr(uint32_t ModeNum, uint32_t RoundSlot);

#endif 