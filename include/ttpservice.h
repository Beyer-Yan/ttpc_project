/**
  ******************************************************************************
  * @file    	ttpservice.h
  * @author  	Beyer
  * @email   	sinfare@foxmail.com
  * @version 	v1.0.0
  * @date    	2016.8
  * @brief   	brief description about this file
  ******************************************************************************
  * @attention
  *
  * @desc
  * This file aims to provide the basic operation interface for byte processing
  * and the ttpc protocol services.
  * The TTPC protocol provides three groups services to the higher layers,
  * COMMUNICATION SERVICES, SAFETY SERVICES, HIGHER LEVEL SERVICES.
  *
  * COMMUNICATION SERVICES are responsible for data exchange, cluster startup and 
  * integration, noise tolerance, acknowledgment scheme and the fault-tolerent clock
  * synchronization.
  *
  * SAFETY SERVICES establish the node membership, the clique avoidance algorithmm, 
  * the independent bus guardian and the host/controller lifesign algorithm.
  *
  * HIGHER LEVEL SERVICES are requsted by the host and are used for switching 
  * between transmission schedules at run time, syncronizing the cluster with 
  * an external clock or other TTPC cluster or for a role-change of a node.
  *
  * Both the communication and the safety services provide a failed-operational 
  * behavior of a running TTPC cluster.
  *   
  * 
  ******************************************************************************
  */
#ifndef __TTPSERVICE_H__
#define __TTPSERVICE_H__
#include "ttpdef.h"

/** memory align */

/**
 * @def TTP_ALIGN(size,align)
 * Return the most contiguous size aligned at specified width. For example, TTP_ALIGN(11,3)
 * will return 12.
 */
#define TTP_ALIGN(size,align)				(((size)+(align)-1)&~((align)-1))

/**
 * @def TTP_ALIGN_DOWN(size,align)
 * Return the down number of the specified aligned width. For example, TTP_ALIGN(11,3) will
 * return 9.
 */
#define TTP_ALIGN_DOWN(size,align)			((size)&~((align)-1))

/** MAX and MIN macro without jumps definition */
#define MIN(a,b) 							((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))
#define MAX(a,b) 							((a) ^ (((a)^(b)) & (((a) > (b)) - 1)))

/** ABS macro definition, remember that int32_t type is needed */
#define ABS(n)								((n) ^ ((n) >> 31)) - ((n) >> 31)

/**
 * If the root interrupt is closed, all the interruptions will be 
 * closed.
 */
void SVC_CloseRootInt(void);
void SVC_OpenRootInt(void);

/**
 * enforce the sleep of the ttp controller. When the ttp controller comes into 
 * the sleep mode, only the interrruption can activate the controller.
 */
void SVC_Sleep(void);

/**
 * freeze the ttp controller, set the corresbonding error bits
 */
void SVC_Freeze(void);

/**
 * @todo more utillties should be added here.
 */

/**
 * @defgroup Service_Group
 */
/**@{*/

/**
 * @defgroup Comm_Services
 */
/**@{*/


uint32_t SVC_ClusterStartup(void*param);

uint32_t SVC_Integration(void*param);

uint32_t SVC_SyncSchema(void*param);

uint32_t SVC_NoiseTolerence(void*param);

uint32_t SVC_Acknowledgment(void*param);



uint32_t SVC_SlotAcquirement(void);


/**@}*/// end of group Comm_Services

/**
 * 
 * @defgroup Safety_Services
 */
/**@{*/

#define CLIQUE_MAJORITY              1
#define CLIQUE_MINORITY              2
#define CLIQUE_NO_ACTIVITY           3
/**
 * This function process the clique detecting. Each TTP controller shall evaluate
 * once in a TDMA round whether it is in the majority clique by checking if it has
 * perceived more agreed slots than failed slots. If this condition is not fulfilled, 
 * meaning that the node detects that it is not consistent with the majority of 
 * the nodes, the TTP controller shall report a clique error to the host and shall 
 * stop operation. 
 *
 * If the sum of the agreed slot and the failed slot is less or equal than one, which
 * indicates that no correct transmission activity, except possibly for the node's own,
 * has been detected during the last slot of a TDMA round, then the TTP/C controller
 * will raises the COMMUNICATION_BLACKOUT error and freeze. This condition is performed
 * once per TDMA round, in the PSP phase prior the the controller's AT time of the node's
 * sending slot of the successor TDMA round.
 * slot.
 * @return  the result of clique detecting.
 *              @arg CLIQUE_MAJORITY
 *              @arg CLIQUE_MINORITY
 *              @arg CLIQUE_NO_ACTIVITY
 */
uint32_t SVC_CliqueDetect(void);

/**
 * check whether the HLFS(Host Life Sign) is valid or not. If the HLFS is not
 * valid, it will return 0. The checking operation shall be performed during
 * PSP phase for once, because the controller will clear it after checking. 
 * @return  check result.
 *       @arg 0  host not valid
 *       @arg !0 host ok
 */
static __INLINE uint32_t SVC_CheckHostLifeSign(void)
{
    return CNI_CheckHLFS();
}

/**
 * update the controller life sign.
 * @return  the value of the controller life sign
 */
static __INLINE uint32_t SVC_UpdateControllerLifeSign(void)
{
    return CNI_UpdateCLFS();
}

/**@}*/// end of group Safety_Services


/**
 * @defgroup HighLevel_Services
 */
/**@{*/

//code here

/**@}*/// end of group HighLevel_Services



/**@}*/// end of group Service_Group

#endif
