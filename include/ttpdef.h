/**
  ******************************************************************************
  * @file    	ttpdef.h
  * @author  	Beyer
  * @email   	sinfare@hotmail.com
  * @version 	v1.0.0
  * @date    	2016.7
  * @brief   	brief description about this file
  ******************************************************************************
  * @attention
  *
  * @description
  *
  * 
  ******************************************************************************
  */

#ifndef __TTP_DEF_H__
#define __TTP_DEF_H__

/** basic types definition */

/** The most platforms will support stdint.h. Maybe,the definition below is not needed. */
#define USE_STDINC

typedef enum {FALSE=0,TRUE} 		bool_t;

#ifndef USE_STDINC
	typedef char 					int8_t;
	typedef unsigned char 			uint8_t;
	typedef short 					int16_t;
	typedef unsigned short 			uint16_t;
	typedef int 					int32_t;
	typedef unsigned int 			uint32_t_t;

	/** half of a byte(4 bits type definition)*/
	typedef unsigned char 			Nibble;

	/** like standard c stddef, definition below may be not be unused */

	typedef int 					ptrdiff_t;
	typedef unsigned int 			size_t; 
	typedef unsigned short 			wchar_t;
	
	#define NULL 					(void*)0
	#define offsetoff(T, member)	((size_t)&((T*)0)->member)

	/** @bref implementation specific of uint48 and uint64 */

	typedef struct{
		uint32_t_t lsb;
		uint16_t msb;
	}uint48_t;

	typedef struct{
		uint32_t_t lsb;
		uint32_t_t msb;
	}uint64_t;

	/** max value of basic types */

	#define UINT8_MAX 				0xff
	#define UINT16_MAX 				0xffff
	#define UINT32_t_MAX			0xffffffff

#else
	#include <stdint.h>
	#include <stddef.h>
#endif
/** compiler related definitions*/
#ifdef __CC_ARM
	#include <stdarg.h>
	#define __SECTION(x)		__attribute__((section(x)))
	#define __UNUSED			__attribute__((unused))
	#define __USED 				__attribute__((used))
	#define __ALIGN(n)			__attribute__((aligned(n)))
	#define __AT(addr)			__attribute__((at(addr)))
	

	/** In this case, the alignment for ARM is 8 bytes */ 
	#define __ALIGNED			__attribute__((aligned))
	#define __PACK				__attribute__((packed))
	#define __WEAK				__weak
	#define __INLINE			__inline
	#define __ASM				__asm
#else
	#error "not supported tool chain"
#endif
/** compiler related definitions end, but need to be completed later for GCC,IARCC,TICC and so on  */ 

/** definitions for  */

#if 1
// /**
//  * @defgroup TTP_REGS_Definition
//  */
// /**@{*/

// /**
//  * @defgroup TTP_REGS_Type
//  */
// /**@{*/

// * 
//  * @warning Attributes of Reg definitions below. Attention that the processor must be in Little-Endian
//  * mode.
 
// typedef volatile union
// {
// 	uint32_t_t dword;				/**< dword part of the register. Normally, one-32-bits part */ 
// 	uint16_t word[2];				/**< word part of the register.  Normally, two-16-bits part */
// 	uint8_t  byte[4];				/**< byte part of the register.  Normally, four-8-bits part */
// }REG32;

// /** definition used to identify the read-only registers */
// #define	__RO 

// /** definition used to identify the read-write registers*/								
// #define __RW

// /** 
//  * definition used to identify the case that the registers are used. 
//  * Registers identified with __INNER are used only for TTP/C controller, while registers identified with 
//  * __OUTER are used for user and controller.
//  */
// #define	__INNER
// #define __OUTER
// #define __HARZARD

// /**
//  * REGs definitions below, which refers to AS6003 TTP/C specification. 
//  * Please pay attention that __RW and __RO are only the dummy identifications, which means that you can 
//  * write registers identified with __RO. Remember that do not write __RO registers on the user level.  
//  * Registers with __RO identification  shall be modified only in kernel level. In hardware implementation, 
//  * __RO registers can be specified in status registers that can be read-only. But in software implementation, 
//  * especially in C language, variables can not be instantiated in "private-like" mode.                               
//  */

// extern   REG32 __REG_FILE[50];

// #define  REG_BASE_PTR			(&__REG_FILE[0])

// /* end TTP_REGS_Type */
// /**@}*/


// /**
//  * @defgroup TTP_REGS_Spec
//  */
// /**@{*/

// /**
//  * The TTPC registers specified in a special way.
//  *  The registers prefixed with a single underscore "_" mean the "word" part of the register union. Those 
//  *  registers prefixed with double underscores "__" mean the byte part of the register union. And those 
//  *  registers prefixed with no underscore mean the "dword" part of the  register. For example, there is 
//  *  such a register named TTP_X, then TTP_X indicates the (TTP_X.dword) and the _TTP_X indicates the 
//  *  (TTP_X.word) and the __TTP_X indicates the (TTP_X.byte). Remember that "word" and the "byte" access 
//  *  mode is not recommended unless you know what you are doing.
//  */

// /**
//  * @defgroup Schedule_Section
//  */
// /**@{*/

// /** 
//  * TTP schedule parameters description register definition. 
//  * This register describes the basic communication behaviors, which are necessary in startup and integration 
//  * procedure of TTPC controller. This register is specified with tag __INNER.
//  */

// #define      TTP_SPR			(__REG_FILE[0].dword)
// #define	    _TTP_SPR			(__REG_FILE[0].word)
// #define    __TTP_SPR			(__REG_FILE[0].byte)

// /**
//  * TTP Macrotick Generation Parameters Description Register.
//  * This register is used to describe the macrotick generation parameter. 
//  * This register is specified with tag __INNER
//  */

// #define      TTP_MGPR			(__REG_FILE[1].dword)
// #define	    _TTP_MGPR			(__REG_FILE[1].word)
// #define    __TTP_MGPR			(__REG_FILE[1].byte)


// /**
//  * TTP precision parameter description register definition.
//  * Clock precision parameter is described by microtick, which is used to indicate the constraint of precision
//  * of macrotick generation. This value determines the error detection of the clock synchronization of local-
//  * area and the cluster-area directly and must be set according to the physical properties of the real clock.  
//  * if this value to be set is less than the actual value of the whole cluster, the TTPC controller will not be
//  * happy. This register is specified with tag __INNER.
//  */


// #define      TTP_PIR			(__REG_FILE[2].dword)
// #define	    _TTP_PIR			(__REG_FILE[2].word)
// #define    __TTP_PIR			(__REG_FILE[2].byte)

// /**
//  * TTP communication rate bit description register definition.
//  * This register is used to define concrete communication bit rate. This register is specified with tag either 
//  * __INNER or __OUTER.
//  * @todo special bits segment definition is necessary for this register.
//  */

// #define      TTP_CBRR			(__REG_FILE[3].dword)
// #define	    _TTP_CBRR			(__REG_FILE[3].word)
// #define    __TTP_CBRR			(__REG_FILE[3].byte)

// /**
//  * TTP Startup Timeout Register definition.
//  * This register is used to set the startup timeout, which is unique in a given cluster design. The startup
//  * timeout of node \f$ j \f$ is specified in formula \f$ \sum_{i=0}^{j-1}slot\_duration_j \f$. This register
//  * is specified with tag __INNER.
//  */

// #define      TTP_STR			(__REG_FILE[4].dword)
// #define     _TTP_STR			(__REG_FILE[4].word)
// #define    __TTP_STR			(__REG_FILE[4].byte)

// /**
//  * TTP Listening Timeout Register definition.
//  * The value of this register is used for the Big-Bang algorithm. The listening timeout of node \f$ j \f$ is 
//  * specified in formula \f$ TTP_STR + 2*TDMA_Round \f$. This register is specified with tag __INNER.
//  */

// #define      TTP_LTR			(__REG_FILE[5].dword)
// #define     _TTP_LTR			(__REG_FILE[5].word)
// #define    __TTP_LTR			(__REG_FILE[5].byte)

// /**
//  * TTP Cold Start Timeout Register Definition.
//  * The value of this register is used for the Big-Bang algorithm. The cold start timeout of node \f$ j \f$ is
//  * specified in formula \f$ TTP_STR + 1*TDMA_Round \f$. This register is specified with tag __INNER.
//  */

// #define      TTP_CTR			(__REG_FILE[6].dword)
// #define     _TTP_CTR			(__REG_FILE[6].word)
// #define    __TTP_CTR			(__REG_FILE[6].byte)

// /* @} */

// /**
//  * @defgroup Role_Section
//  */
// /* @{ */

// /**
//  * TTP Role Section Register definition.
//  * This register is used to describe the sending slot of the node-self. This register is specified with tag 
//  * __INNER.
//  *
//  */

// #define      TTP_RSR			(__REG_FILE[7].dword)
// #define     _TTP_RSR			(__REG_FILE[7].word)
// #define    __TTP_RSR			(__REG_FILE[7].byte)

// /**
//  * TTP Logical Name Register definition.
//  * This register is used to indicate the particular slot for frame sending and clique detecting. This register
//  * is specified with tag either __INNER or __OUTER.
//  *
//  */

// #define      TTP_LNR			(__REG_FILE[8].dword)
// #define     _TTP_LNR			(__REG_FILE[8].word)
// #define    __TTP_LNR			(__REG_FILE[8].byte)

// /**
//  * TTP Send Delay Register definition.
//  * This register specifies the compulsory sending delay when the node is sending node. This register is specified
//  * with tag __INNER.
//  *
//  */

// #define      TTP_SNDR			(__REG_FILE[9].dword)
// #define     _TTP_SNDR			(__REG_FILE[9].word)
// #define    __TTP_SNDR			(__REG_FILE[9].byte)

// /**@}*/


// /**
//  * @defgroup Identification_Section
//  */
// /**@{*/

// /**
//  * TTP Cluster Schedule Identification Register definition.
//  * This register specifies a n identifier of the abstract MEDL, i.e.the cluster design. The cluster designer has to 
//  * ensure that the schedule IDs differ for different cluster designs. The contents are used for the frame CRC 
//  * calculation to guarantee that only node based on the same cluster design can participate the same cluster. This 
//  * register is specified with tag __INNER.
//  * 
//  */

// #define      TTP_IDSCR			(__REG_FILE[10].dword)
// #define     _TTP_IDSCR			(__REG_FILE[10].word)
// #define    __TTP_IDSCR			(__REG_FILE[10].byte)

// /**
//  * TTP Application Identification Register definition.
//  * Unique ID used by the host application to verify whether the actual MEDL is compatible to it or not. Although the
//  * Application ID is not used by the TTP/C controller, it ensure a correct cooperation between the controller and 
//  * the host. The application ID may consist of several subparts that are generated by the cluster schedule and the 
//  * node design tools. This register specified with tag __OUTER. 
//  * @todo Bits segment should be 
//  *
//  */

// #define      TTP_IDSAR			(__REG_FILE[11].dword)
// #define     _TTP_IDSAR			(__REG_FILE[11].word)
// #define    __TTP_IDSAR			(__REG_FILE[11].byte)

// /**@}*/


// /**
//  * @defgroup Round_Slot_Section
//  */
// /**@{*/

// /**
//  * TTP Round Slot Section Register definition.
//  * A cluster cycle is a sequence of round slots. Each round slot is assigned to a node slot, but parameters of round 
//  * slots can vary in different TDMA rounds. For each cluster mode a cluster cycle description (list of successive
//  * round slots) must exist. Round slot section,in fact, consists of a group of registers and TTP_RSSR is just the
//  * one of them. This register is specified with tag either __INNER or __OUTER.
//  *
//  */

// #define      TTP_RSSR			(__REG_FILE[12].dword)
// #define     _TTP_RSSR			(__REG_FILE[12].word)
// #define    __TTP_RSSR			(__REG_FILE[12].byte)

// /**
//  * TTP CNI Register definition.
//  * This register is used to define the Source/Destination address and the length of the application data transmitted 
//  * in the slot. May also be used to mark data as "ignore", if the received data is not be used. This register is specified
//  * with tag __INNER and __OUTER.
//  * 
//  */

// #define      TTP_CNIR			(__REG_FILE[13].dword)
// #define     _TTP_CNIR			(__REG_FILE[13].word)
// #define    __TTP_CNIR			(__REG_FILE[13].byte)

// /**
//  * TTP Slot Duration Register definition.
//  * This register specifies the duration of the current node slot in units of macrotick. Slot durations are chosen to fit the
//  * timing requirements of the application. At least transmission phase,the pre-send phase and the post-received phase must
//  * be processed within the defined slot duration. This register is specified with tag __INNER.
//  *
//  */

// #define      TTP_SDR			(__REG_FILE[14].dword)
// #define     _TTP_SDR			(__REG_FILE[14].word)
// #define    __TTP_SDR			(__REG_FILE[14].byte)

// /**
//  * TTP Logical Sender register definition.
//  * This register specifies the slot position part and the multiplex ID part of the logical sender name in this slot. If the node
//  * slot position of the current logical name of the controller matches the value contained in this entry, then a non-multiplexed
//  * controller is the sender in the current slot; a controller sharing this slot with other node must additionally have a multiplex
//  * ID equal to the multiplex ID stored in the current round slot entry. If the controller is not the sender in the slot, it is a
//  * receiver. The value of this field represents the number of the slot in the TDMA round zero based and can be calculated as the
//  * round slot modulo the number of slots in the TDMA round. This register is specified with tag __INNER.
//  *
//  */

// #define      TTP_LSR			(__REG_FILE[15].dword)
// #define     _TTP_LSR			(__REG_FILE[15].word)
// #define    __TTP_LSR			(__REG_FILE[15].byte)

// /**
//  * TTP Transmission Phase Duration Register definition.
//  * This register specifies the duration of the transmission phase in the node slot. The length of the transmission phase includes
//  * the maximum transmission duration of the two channels with jitter and delay. This register is specified with tag __INNER.
//  *
//  */

// #define      TTP_TPDR			(__REG_FILE[16].dword)
// #define     _TTP_TPDR			(__REG_FILE[16].word)
// #define    __TTP_TPDR			(__REG_FILE[16].byte)

// /**
//  * Propagation delays of the frame transmission between this node and the sending node on the respective channels of in units of
//  * microticks. If the node is the sending node, the values are ignored and the send delays @see TTP_SNDR from the protocol parameters
//  * are used to delay the transmissions on the two channels compulsorily. The combination of the delays (send delay and the delay
//  * correction term) of the receiving  and transmitting nodes will amount to the delay correction of the virtual point-to-point
//  * connection. These two registers are specified with tag __INNER.
//  *
//  */

// /** [TTP_DCTR0			 propagation delay of channel 0 ] */
// #define      TTP_DCTR0			(__REG_FILE[17].dword)
// #define     _TTP_DCTR0			(__REG_FILE[17].word)
// #define    __TTP_DCTR0			(__REG_FILE[17].byte)

// /** [TTP_DCTR1			 propagation delay of channel 1] */
// #define      TTP_DCTR1			(__REG_FILE[18].dword)
// #define     _TTP_DCTR1			(__REG_FILE[18].word)
// #define    __TTP_DCTR1			(__REG_FILE[18].byte)

// /**@}*/


// /**
//  * @defgroup C-State
//  */
// /**@{*/

// /**
//  * TTP Global Time Field Register definition.
//  * This register specifies the macrotick value of the start time of the transmission phase (the AT time) in the current slot.
//  * The global time shall be updated during the pre-send phase with respect to the current value of the global time and with
//  * respect to the current round slot position. This register is specified with tag __INNER.
//  * 
//  */

// #define      TTP_GTFR			(__REG_FILE[19].dword)
// #define     _TTP_GTFR			(__REG_FILE[19].word)
// #define    __TTP_GTFR			(__REG_FILE[19].byte)

// /**
//  * TTP Cluster Position Field Register definition.
//  * This register specifies the information about the current round slot position, the current cluster mode and the pending
//  * deferred mode change(DMC).
//  * - The DMC field contains the value of the requested successor cluster mode
//  * 	- the cluster mode field contains the value of the current active cluster mode the TTPC controller is operating in. The
//  *  	TTPC controller updates the cluster mode field accordingly.
//  * 	- The round slot position field contains the current cluster position information. The round slot position is zero based
//  * 		and updated in the pre-send phase. 
//  *
//  */

// #define      TTP_CPFR			(__REG_FILE[20].dword)
// #define     _TTP_CPFR			(__REG_FILE[20].word)
// #define    __TTP_CPFR			(__REG_FILE[20].byte)

// /**
//  * TTP Membership Vector Register Group definition.
//  * These registers specify a consistent view of the operational status of all nodes in the cluster. A receiving node updates
//  * its membership vector in the post-receive-phase(PRP) based on its perceived status of the received frame. A sending node 
//  * updates its membership vector flag according to the acknowledgment algorithm. These registers are specified with tag
//  * __INNER.
//  *
//  */

// /** [TTP_MVF0			 membership vector 0 ] */
// #define      TTP_MVF0			(__REG_FILE[21].dword)
// #define     _TTP_MVF0			(__REG_FILE[21].word)
// #define    __TTP_MVF0			(__REG_FILE[21].byte)

// /** [TTP_MVF1			 membership vector 1] */
// #define      TTP_MVF1			(__REG_FILE[22].dword)
// #define     _TTP_MVF1			(__REG_FILE[22].word)
// #define    __TTP_MVF1			(__REG_FILE[22].byte)

// /** [TTP_MVF2			 membership vector 2] */
// #define      TTP_MVF2			(__REG_FILE[23].dword)
// #define     _TTP_MVF2			(__REG_FILE[23].word)
// #define    __TTP_MVF2			(__REG_FILE[23].byte)

// /** [TTP_MVF3			 membership vector 3] */
// #define      TTP_MVF3			(__REG_FILE[24].dword)
// #define     _TTP_MVF3			(__REG_FILE[24].word)
// #define    __TTP_MVF3			(__REG_FILE[24].byte)

// /**@}*/


// /**
//  * @defgroup Protocol_Status
//  */
// /**@{*/

// /**
//  * TTP Clock Synchronization State Correction Register Definition.
//  * This register specifies the current value of the clock correction term that has been computed by the clock
//  * synchronization algorithm in units of microticks. This register is specified with tag __INNER and __OUTER. 
//  *
//  */

// #define      TTP_CSCTR			(__REG_FILE[25].dword)
// #define     _TTP_CSCTR			(__REG_FILE[25].word)
// #define    __TTP_CSCTR			(__REG_FILE[25].byte)

// /**
//  * TTP Protocol State Register definition.
//  * This register specifies the state that the TTPCcontroller currently executes. This register is specified with
//  * tag __INNER as well as __OUTER.
//  *
//  */

// #define      TTP_PSR			(__REG_FILE[26].dword)
// #define     _TTP_PSR			(__REG_FILE[26].word)
// #define    __TTP_PSR			(__REG_FILE[26].byte)

// /**
//  * TTP Frame Status Register Definition.
//  * This register specifies the current frame status and the slot status of the slot. This register is specified
//  * with tag __INNER as well as __OUTER.
//  *
//  */

// #define      TTP_FSR			(__REG_FILE[27].dword)
// #define     _TTP_FSR			(__REG_FILE[27].word)
// #define    __TTP_FSR			(__REG_FILE[27].byte)

// /**
//  * TTP Error Flags Register Definition.
//  * This register specifies the internal protocol errors detected by the controller. They are updated as soon as 
//  * the appropriate error condition is detected by the controller, and at the same time an interrupt may be raised.
//  * The flags is cleared at the controller initialization; the controller immediately freezes upon the occurrence
//  * of any protocol error. This register is specified with tag __INNER and __OUTER.
//  * 
//  */

// #define      TTP_EFR			(__REG_FILE[28].dword)
// #define     _TTP_EFR			(__REG_FILE[28].word)
// #define    __TTP_EFR			(__REG_FILE[28].byte)

// /**@}*/


// /**
//  * @defgroup Controller_Info
//  */
// /**@{*/

// /**
//  * TTP Serial Number Register Definition.
//  * This register specifies the serial number of the TTPC controller and is specified with tag __OUTER.
//  *
//  */

// /** [TTP_SNLR			 the least significant 32 bits of the serial number] */
// #define      TTP_SNLR			(__REG_FILE[29].dword)
// #define     _TTP_SNLR			(__REG_FILE[29].word)
// #define    __TTP_SNLR			(__REG_FILE[29].byte)

// /** [TTP_SNHR			 the most significant 32 bits of the serial number] */
// #define      TTP_SNHR			(__REG_FILE[30].dword)
// #define     _TTP_SNHR			(__REG_FILE[30].word)
// #define    __TTP_SNHR			(__REG_FILE[30].byte)

// /**
//  * TTP Controller Life Sign  Register Definition.
//  * This registers specifies a periodic life-sign information to the host, if no other frequently updated
//  * fields are used for a life-sign mechanism. This life-sign is updated in its own sending slot. This
//  * register is specified with tag __INNER.
//  *
//  */

// #define      TTP_CLFSR			(__REG_FILE[31].dword)
// #define     _TTP_CLFSR			(__REG_FILE[31].word)
// #define    __TTP_CLFSR			(__REG_FILE[31].byte)

// /**
//  * TTP Cluster Time Field Register definition.
//  * This register specifies the globally synchronized time with a granularity of one macrotick. The register 
//  * is read only for the host. The value of this register is not defined if the C-state is not valid. This 
//  * register is specified with tag __OUTER.
//  *
//  */

// #define      TTP_CTFR			(__REG_FILE[32].dword)
// #define     _TTP_CTFR			(__REG_FILE[32].word)
// #define    __TTP_CTFR			(__REG_FILE[32].byte)

// /**@}*/ 


// /**
//  * @defgroup Control_Field
//  */
// /**@{*/

// /**
//  * TTP Host Life-Sign Register definition.
//  * This register specifies the periodically updated life-sign that the host shall provide to the TTPC controller.
//  * This register is specified with tag __OUTER and __INNER.
//  *
//  */

// #define      TTP_HLFSR			(__REG_FILE[33].dword)
// #define     _TTP_HLFSR			(__REG_FILE[33].word)
// #define    __TTP_HLFSR			(__REG_FILE[33].byte)

// /**
//  * TTP Control Register definition.
//  * This register specifies the basic operation from the host to the TTPC controller, such as the global interrupt
//  * switcher, the mode change requesting, the controller power control and so on. This register is specified with 
//  * tag __OUTER.
//  *
//  */

// #define      TTP_CR				(__REG_FILE[34].dword)
// #define     _TTP_CR				(__REG_FILE[34].word)
// #define    __TTP_CR				(__REG_FILE[34].byte)

// *
//  * TTP External Rate Correction Field Register definition.
//  * This register specifies a common mode rate correction term in units of microicks per resynchronization interval,
//  * which is added to the correction term of the TTPC controller internal clock synchronization algorithm. The TTPC
//  * controller will regard the value of the field only if the AR flag is set. The contents of the field are bounded
//  * by 1/2 PI. If the host violates the condition, a synchronization error will be raised. This register is specified
//  * with tag __INNER and__OUTER.
//  *
 

// #define      TTP_ERCFR			(__REG_FILE[35].dword)
// #define     _TTP_ERCFR			(__REG_FILE[35].word)
// #define    __TTP_ERCFR			(__REG_FILE[35].byte)

// /**
//  * TTP Timer Register definition.
//  * This register specifies a special point in time at which an interrupt should be raised. For this purpose, the host
//  * processor writes the desired value of the global time to this field. When the global time reached the value, a timer
//  * interrupt to the host processor is raised. This register is specified with tag __INNER and __OUTER.
//  *
//  */

// #define      TTP_TIMR			(__REG_FILE[36].dword)
// #define     _TTP_TIMR			(__REG_FILE[36].word)
// #define    __TTP_TIMR			(__REG_FILE[36].byte)

// /**
//  * TTP Interrupt Status Register definition.
//  * This register specifies the current status of interrupt from the TTPC controller. This register is specified with
//  * tag __INNER and __OUTER.
//  *
//  */

// #define      TTP_ISR			(__REG_FILE[37].dword)
// #define     _TTP_ISR			(__REG_FILE[37].word)
// #define    __TTP_ISR			(__REG_FILE[37].byte)

// /**
//  * TTP Interrupt Mask Register definition.
//  * This register specifies the interrupt mask corresponding to TTP_ISR register. This register is specified with tag
//  * __OUTER and __INNER.
//  *
//  */

// #define      TTP_IMR			(__REG_FILE[38].dword)
// #define     _TTP_IMR			(__REG_FILE[38].word)
// #define    __TTP_IMR			(__REG_FILE[38].byte)

// /**
//  * TTP Timer Overflow Counter Field Register definition.
//  * This register specifies a counter which is initialized with 0 upon a reset of the controller and is incremented each
//  * time an overflow if the cluster time occurs. This register can be set by the host to any value and can be used to build
//  * a larger ranged time base with macotick granularity, but only the synchronized cluster part is equal to all nodes; the 
//  * timer overflow counter can be synchronized by the host applications. This register is specified with tag __OUTER.
//  *
//  */

// #define      TTP_TOCFR			(__REG_FILE[39].dword)
// #define     _TTP_TOCFR			(__REG_FILE[39].word)
// #define    __TTP_TOCFR			(__REG_FILE[39].byte)

// /**
//  * TTP Time Startup Field Register definition.
//  * This field is used for synchronized cluster startup. The C-state time value transmitted in the first cold start frame of 
//  * the controller contains this value. The controller reads this value at a defined point in time before transmitting the 
//  * cold start frame (this point in time is available in the controller data-sheet), the host must therefore update the field 
//  * accordingly. Synchronized cluster startup is typically used in conjunction with inter-cluster synchronization.
//  * @todo I'm confused by the register setting, which refers to "Time Triggered Protocol Specification.pdf" on page 92. If you
//  * know more about it, please email to me for more discussion.
//  * This register is specified with tag __INNER and __OUTER.
//  */

// #define      TTP_TSFR			(__REG_FILE[40].dword)
// #define     _TTP_TSFR			(__REG_FILE[40].word)
// #define    __TTP_TSFR			(__REG_FILE[40].byte)

// /**@}*/


// /**
//  * @defgroup Counter_Field
//  */
// /**@{*/

// /**
//  * TTP Counter Register 0 definition.
//  * This register specifies the necessary counter used by the protocol executing, including the FAILED SLOTS COUNTER, the 
//  * AGREED SLOTS COUNTER, the INTEGRATION COUNTER and the COLD START COUNTER.
//  *
//  */

// #define      TTP_CNTR0			(__REG_FILE[41].dword)
// #define     _TTP_CNTR0			(__REG_FILE[41].word)
// #define    __TTP_CNTR0			(__REG_FILE[41].byte)

// /**
//  * TTP Counter Register 1 definition.
//  * This register specifies the necessary counter used by the protocol executing, including the FIRST SUCCESSOR MEMBERSHIP
//  * FLAG and the MEMBERSHIP FAILED COUNTER.
//  */

// #define      TTP_CNTR1			(__REG_FILE[42].dword)
// #define     _TTP_CNTR1			(__REG_FILE[42].word)
// #define    __TTP_CNTR1			(__REG_FILE[42].byte)

// /**
//  * TTP clock synchronization FIFO register group definition.
//  * These registers specify the clock synchronization stack used by the synchronization algorithm. They are 
//  * specified with tag __INNER.
//  *
//  */

// /** [TTP_CSF0			 clock synchronization FIFO 0] */
// #define      TTP_CSF0			(__REG_FILE[43].dword)
// #define     _TTP_CSF0			(__REG_FILE[43].word)
// #define    __TTP_CSF0			(__REG_FILE[43].byte)

// /** [TTP_CSF1			 clock synchronization FIFO 1] */
// #define      TTP_CSF1			(__REG_FILE[44].dword)
// #define     _TTP_CSF1			(__REG_FILE[44].word)
// #define    __TTP_CSF1			(__REG_FILE[44].byte)

// /** [TTP_CSF2			 clock synchronization FIFO 2] */
// #define      TTP_CSF2			(__REG_FILE[45].dword)
// #define     _TTP_CSF2			(__REG_FILE[45].word)
// #define    __TTP_CSF2			(__REG_FILE[45].byte)

// /** [TTP_CSF3			 clock synchronization FIFO 3] */
// #define      TTP_CSF3			(__REG_FILE[46].dword)
// #define     _TTP_CSF3			(__REG_FILE[46].word)
// #define    __TTP_CSF3			(__REG_FILE[46].byte)

// /**@}*/

// /* end TTP_REGS_Spec */
// /**@}*/

// /*end group TTP_REGS_Definition*/
// /**@}*/

// /**
//  * @defgroup Regs_Bits_Def
//  */
// /**@{*/

// /**
//  * @defgroup Schedule_Parameter
//  */
// /**@{*/
// /**
//  * @brief Bits definition of TTP_SPR register
//  */
// #define SPR_MMFC				((uint32_t)0xFF000000)	/**< Maximum membership Failure Count */
// #define SPR_MCSE				((uint32_t)0x00FF0000)	/**< Maximum cold start entry */
// #define SPR_MIC					((uint32_t)0x00003F00)	/**< Minimum Integration Count */
// #define SPR_AR					((uint32_t)0x00000004)	/**< Allow external rate correction flag bit */
// #define SPR_CIA					((uint32_t)0x00000002)	/**< Cold start integration allowed flag bit */
// #define SPR_CF 					((uint32_t)0x00000001)	/**< Cold start allowed flag bit */

// /**
//  * @brief Bits definition of TTP_MGPR register
//  */
// #define SPR_MGP					((uint32_t)0xFFFFFFFF)	/**< Macrotick generation parameter */

// /**
//  * @brief Bits definition of TTP_PIR register
//  */
// #define SPR_PI					((uint32_t)0xFFFFFFFF)	/**< Precision parameter */

// /**
//  * @brief Bits definition of TTP_CBRR register
//  */
// #define SPR_CBR					((uint32_t)0xFFFFFFFF)	/**< Communication bit rate */

// /**
//  * @brief Bits definition of TTP_STR register
//  */
// #define SPR_STO					((uint32_t)0xFFFFFFFF)	/**< Startup timeout */

// /**
//  * @brief Bits definition of TTP_LTR register
//  */
// #define SPR_LTO					((uint32_t)0xFFFFFFFF)	/**< Listen timeout */

// /**
//  * @brief Bits definition of TTP_CTR
//  */
// #define SPR_CLO					((uint32_t)0xFFFFFFFF)	/**< Cold start timeout */

// /**@}*///end of group Shedule_Parameter

// /**
//  * @defgroup Role_Section
//  */
// /**@{*/

// /**
//  * @brief Bits definition of TTP_RSR register
//  */
// #define RSR_FPMV				((uint32_t)0x0000FF00)	/**< Flag position in membership position */
// #define RSR_MM					((uint32_t)0x00000002)	/**< Multiplexed membership flag bit */
// #define RSR_PF 					((uint32_t)0x00000001)	/**< Passive flag */

// /**
//  * @brief Bits definition of TTP_LNR register
//  */
// #define RSR_LNSP				((uint32_t)0xFFFF0000)	/**< Logical name slot position */
// #define RSR_LNMID				((uint32_t)0x0000FFFF)	/**< Logical name Multiplexed identification */

// /**
//  * @brief Bits definition of TTP_SNDR register
//  */
// #define RSR_SNDLY				((uint32_t)0xFFFFFFFF)	/**< Send delay, unit:microtick */
// /**@}*/// end of group Role_Section

// /**
//  * @defgroup Identification_Section
//  */
// /**@{*/			
// /**
//  * @brief Bits definition of TTP_IDSCR register
//  */
// #define ID_CSID					((uint32_t)0xFFFFFFFF)	/**< Cluster schedule identification */

// /**
//  * @brief Bits definition of TTP_IDAR
//  */
// #define ID_APPID				((uint32_t)0xFFFFFFFF)	/**< Application identification */
// /**@}*/// end of group Identification_Section

// /**
//  * @defgroup Round_Slot_Section
//  */
// /**@{*/
// /**
//  * @brief Bits definition of TTP_RSSR register
//  */
// #define RSSR_SDFPMV				((uint32_t)0x00FF0000)	/**< Flag position in membership vector */
// #define RSSR_FRMT				((uint32_t)0x00000300)	/**< Frame type */
// 	#define FRMT_RESV			((uint32_t)0x00000000)	/**< not used */
// 	#define FRMT_N				((uint32_t)0x00000100)	/**< N-frame */
// 	#define FRMT_I 				((uint32_t)0x00000200)	/**< I frame */
// 	#define FRMT_X				((uint32_t)0x00000300)	/**< X frame */
// #define RSSR_CLKSYN				((uint32_t)0x00000008)	/**< Clock synchronization flag bit */
// #define RSSR_SYF				((uint32_t)0x00000004)	/**< Synchronization Frame Flag bit */
// #define RSSR_RA					((uint32_t)0x00000002)	/**< Reintegration allowed flag bit */
// #define RSSR_MCP				((uint32_t)0x00000001)	/**< Mode change permission bit */

// #define IS_TTP_FRMT(FRAME)		(((FRAME) == FRMT_N) || \
// 								 ((FRAME) == FRMT_I) || \
// 								 ((FRMAE) == FRMT_X))
// /**
//  * @brief Bits definition of TTP_CNIR register
//  */
// #define RSSR_OFFSET				((uint32_t)0xFFFFFF00)	/**< CNI address offset relative to the base address of CNI */
// #define RSSR_LNGTH				((uint32_t)0x000000FF)	/**< Length of the data to be transmitted */

// /**
//  * @brief Bits definition of TTP_SDR register
//  */
// #define RSSR_SLTDUT				((uint32_t)0xFFFFFFFF)	/**< Slot duration, unit:macrotick */

// /**
//  * @brief Bits definition of TTP_LSR register
//  */
// #define RSSR_LSSP 				((uint32_t)0xFFFF0000)	/**< Logical sender slot position */
// #define RSSR_LSMID				((uint32_t)0x0000FFFF)	/**< Logical sender multiplexed ID */

// /**
//  * @brief Bits definition of TTP_TPDR register
//  */
// #define RSSR_TPD				((uint32_t)0x0000FFFF)	/**< Transmission phase duration */

// /**
//  * @brief Bits definition of TTP_DCTR0 and TTP_DCTR1 register
//  */
// #define RSSR_DCT0				((uint32_t)0xFFFFFFFF)	/**< Delay correction term 0 */
// #define RSSR_DCT1				((uint32_t)0xFFFFFFFF)	/**< Delay correction term 1 */
// /**@}*/// end of group Round_Slot_Section

// /**
//  * @defgroup C-State
//  */
// /**@{*/
// /**
//  * @brief Bits definition of TTP_GTFR register
//  */
// #define CS_GTS					((uint32_t)0x0000FFFF)	/**< Global time field */

// /**
//  * @brief Bits definition of TTP_CPFR register
//  */
// #define CS_DMC					((uint32_t)0x0000C000)	/**< Deferred pending mode changes */
// 	#define DMC_NO_REQ			((uint32_t)0x00000000)	/**< No pending mode change request */
// 	#define DMC_MODE_1			((uint32_t)0x00004000)	/**< First successor mode */
// 	#define DMC_MODE_2			((uint32_t)0x00008000)	/**< Second successor mode */
// 	#define DMC_MODE_3			((uint32_t)0x0000C000)	/**< Third successor mode */
// #define CS_MODE					((uint32_t)0x00003800)	/**< Mode */
// 	#define MODE_NO				((uint32_t)0x00000000)	/**< Node mode change request */
// 	#define MODE_1 				((uint32_t)0x00000800)	/**< First  successor mode requested */
// 	#define MODE_2 				((uint32_t)0x00001000)	/**< Second successor mode requested */
// 	#define MODE_3 				((uint32_t)0x00001800)	/**< Third  successor mode requested */
// 	#define MODE_CLR			((uint32_t)0x00002000)	/**< Clear pending mode change request */
// #define CS_RS					((uint32_t)0x000007FF)	/**< Round slot */

// #define IS_TTP_DMC(DMC)			(((DMC) == DMC_NO_REQ) || \
// 								 ((DMC) == DMC_MODE_1) || \
// 								 ((DMC) == DMC_MODE_2) || \
// 								 ((DMC) == DMC_MODE_3))

// #define IS_TTP_MODE(MODE)		(((MODE) == MODE_NO) || \
// 								 ((MODE) == MODE_1)  || \
// 								 ((MODE) == MODE_2)  || \
// 								 ((MODE) == MODE_3)  || \
// 								 ((MODE) == MODE_CLR))

// /**
//  * @brief Bits definition of TTP_MVFs register
//  */
// #define __CS_MV					((uint32_t)0x0000FFFF)	/**< Membership vector bits pattern    */
// #define CS_MV0					__CS_MV					/**< Region for identifying bits 15-0  */
// #define CS_MV1					__CS_MV					/**< Region for identifying bits 31-16 */
// #define CS_MV2					__CS_MV					/**< Region for identifying bits 47-32 */
// #define CS_MV3					__CS_MV					/**< Region for identifying bits 63-48 */
// /**@}*/// end of group C-State

// /**
//  * @defgroup Status_Section
//  */
// /**@{*/
// /**
//  * @brief Bits definition of TTP_CSCTR register
//  */
// #define STATUS_CSCT				((uint32_t)0xFFFFFFFF)	/**< Clock state correction term */

// /**
//  * @brief Bits definition of TTP_PSR register
//  */
// #define STATUS_MMC				((uint32_t)0x04000000)	/**< MEMB(C) */
// #define STATUS_MMB				((uint32_t)0x02000000)	/**< MEMB(B) */
// #define STATUS_MMA				((uint32_t)0x01000000)	/**< MEMB(A) */
// #define STATUS_ACKS				((uint32_t)0x00030000)	/**< Acknowledgment state */
// 	#define ACKS_INVALID		((uint32_t)0x00000000)	/**< Invalid ACK state */
// 	#define ACKS_STATE_1		((uint32_t)0x00010000)	/**< ACK state 1, waiting for the first successor */	
// 	#define ACKS_STATE_2		((uint32_t)0x00020000)	/**< ACK state 2, waiting for the second successor */
// 	#define ACKS_FINISHED		((uint32_t)0x00030000)	/**< ACK finished */
// #define STATUS_OCF				((uint32_t)0x00000400)	/**< Observed channel flag bit, 0:channel_0 | 1:channel_1 */
// #define STATUS_FSF				((uint32_t)0x00000200)	/**< Free-shot flag bit */
// #define STATUS_BBF				((uint32_t)0x00000100)	/**< Big-Bang flag bit */
// #define STATUS_PS				((uint32_t)0x0000000F)	/**< Protocol state machine */
// 	#define PS_FREEZE			((uint32_t)0x00000000)	/**< Freeze state */
// 	#define PS_INIT				((uint32_t)0x00000001)	/**< Initialization state */
// 	#define PS_LISTEN			((uint32_t)0x00000002)	/**< Listen state */
// 	#define PS_COLDSTART		((uint32_t)0x00000003)	/**< Cold start state */
// 	#define PS_ACTIVE			((uint32_t)0x00000004)	/**< Active state */
// 	#define PS_PASSIVE			((uint32_t)0x00000005)	/**< Passive state */
// 	#define PS_AWAIT			((uint32_t)0x00000006)	/**< Await state */
// 	#define PS_TEST				((uint32_t)0x00000007)	/**< Test state */
// 	#define PS_DOWNLOAD			((uint32_t)0x00000008)	/**< Download state */

// #define IS_TTP_ACKS(ACKS)		(((ACKS) == ACKS_INVALID) || \
// 								 ((ACKS) == ACKS_STATE_1) || \
// 								 ((AKCS) == ACKS_STATE_2) || \
// 								 ((ACKS) == ACKS_FINISHED))

// #define IS_TTP_PS(PS)			(((PS) == PS_FREEZE)    || \
// 								 ((PS) == PS_INIT)      || \
// 								 ((PS) == PS_LISTEN)    || \
// 								 ((PS) == PS_COLDSTART) || \
// 								 ((PS) == PS_ACTIVE)    || \
// 								 ((PS) == PS_PASSIVE)   || \
// 								 ((PS) == PS_AWAIT)     || \
// 								 ((PS) == PS_TEST)      || \
// 								 ((PS) == PS_DOWNLOAD))

// /**
//  * @brief Bits definition of TTP_FSR register
//  */

// /******-------------------------------------------------------------------------------------
//   FSR | 16 bits resv | 4 bits SLTS | 4 bits resv | 4 bits FMS1 | 4 bits resv | 4 bits FMS0 |
//   ****-------------------------------------------------------------------------------------*/

// #define FMS_CORRECT 			((uint32_t)0x00000000)
// #define FMS_TENTATIVE			((uint32_t)0x00000001)
// #define FMS_INCORRECT			((uint32_t)0x00000002)
// #define FMS_OTHER_ERR			((uint32_t)0x00000003)
// #define FMS_NULL				((uint32_t)0x00000004)
// #define FMS_INVALID				((uint32_t)0x00000005)
// #define FMS_CS_AGREEMENT		((uint32_t)0x00000006)
// #define FMS_VALID 				((uint32_t)0x00000007)

// #define STATUS_SLTS				((uint32_t)0x000F0000)	/**< Slot status,which is the min(STATUS_FMS1,STATUS_FMS0) */
// 	#define SLTS_CORRECT 		(FMS_CORRECT<<16) 		
// 	#define SLTS_TENTATIVE		(FMS_TENTATIVE<<16)		
// 	#define SLTS_INCORRECT		(FMS_INCORRECT<<16)		
// 	#define SLTS_OTHER_ERR		(FMS_OTHER_ERR<<16)		
// 	#define SLTS_NULL			(FMS_NULL<<16)			
// 	#define SLTS_INVALID		(FMS_INVALID<<16)			
// #define STATUS_FMS1 			((uint32_t)0x00000F00)	/**< Frame status of channel 1 */
// #define STATUS_FMS0				((uint32_t)0x0000000F)	/**< Frame status of channel 0 */

// #define TTP_FMS0				((uint32_t)0x0AAAAAAA)
// #define TTP_FMS1				((uint32_t)0x1DDDDDDD)

// #define IS_SLTS_STATUS(SLTS)	(((SLTS) == SLTS_CORRECT)   || \
// 								 ((SLTS) == SLTS_TENTATIVE) || \
// 								 ((SLTS) == SLTS_INCORRECT) || \
// 								 ((SLTS) == SLTS_OTHER_ERR) || \
// 								 ((SLTS) == SLTS_NULL)      || \
// 								 ((SLTS) == SLTS_INVALID))

// /**
//  * @brief Bits definition of TTP_EFR register
//  */
// #define STATUS_MC 				((uint32_t)0x01000000)	/**< MEDL error flag bit */
// #define STATUS_CC 				((uint32_t)0x00080000)	/**< Concurrency control error flag bit */
// #define STATUS_NR				((uint32_t)0x00004000)	/**< Frame not ready error flag bit */
// #define STATUS_MV				((uint32_t)0x00002000)	/**< Mode violation error flag bit */
// #define STATUS_SO 				((uint32_t)0x00001000)	/**< Slot occupied error flag error bit */
// #define STATUS_BE				((uint32_t)0x00000010)	/**< Bus Guardian error flag bit */
// #define STATUS_CB				((uint32_t)0x00000008)	/**< Communication system blackout error flag bit */
// #define STATUS_ME				((uint32_t)0x00000004)	/**< Membership error flag bit */
// #define STATUS_SE 				((uint32_t)0x00000002)	/**< Synchronization error flag bit */
// #define STATUS_CE 				((uint32_t)0x00000001)	/**< Clique error flag bit */			
// /**@}*/// end of group Status_Section

// /**
//  * @todo TTP controller Serial_Number information may be defined here
//  */

// /**
//  * @defgroup Control_Section
//  */
// /**@{*/
// /**
//  * @brief Bits definition of TTP_HLFSR register
//  */
// #define CTS_HLFS				((uint32_t)0x0000000F)	/**< Host life-sign flag bits pattern */

// /**
//  * @brief Bits definition of TTP_CR register
//  */
// #define CTS_IE					((uint32_t)0x00000010)	/**< Interrupt enable */
// #define CTS_MCRF				((uint32_t)0x00000008)	/**< Mode change request enable flag */
// #define CTS_BIST				((uint32_t)0x00000004)	/**< Built-in test enable flag */
// #define CTS_CA					((uint32_t)0x00000002)	/**< Control await enable flag */
// #define CTS_CO					((uint32_t)0x00000001)	/**< Controller on */

// /**
//  * @brief Bits definition of TTP_ERCFR register
//  */
// #define CTS_ERCF				((uint32_t)0xFFFFFFFF)	/**< External rate correction field */

// /**
//  * @brief Bits definition of TTP_TIMR register
//  */
// #define CTS_TIM					((uint32_t)0x0000FFFF)	/**< Predefined timer value for user interruption */

// /**
//  * @brief Bits definition of TTP_ISR register
//  */
// #define CTS_ISR_TI				((uint32_t)0x00010000)	/**< Timer interrupt state */
// #define CTS_ISR_UI				((uint32_t)0x00000400)	/**< User interrupt state */
// #define CTS_ISR_ML				((uint32_t)0x00000200)	/**< membership loss state */
// #define CTS_ISR_CV				((uint32_t)0x00000100)	/**< C-state valid state */
// #define CTS_ISR_CR				((uint32_t)0x00000008)	/**< Controller ready state */
// #define CTS_ISR_HE				((uint32_t)0x00000004)	/**< Host error state */
// #define CTS_ISR_PE				((uint32_t)0x00000002)	/**< Protocol error state */
// #define CTS_ISR_BR				((uint32_t)0x00000001)	/**< BIST error state */

// /**
//  * @brief Bits definition of TTP_IMR register
//  */
// #define CTS_IMR_MTI				((uint32_t)0x00008000)	/**< The mask of TIMER_INTERRUPT */
// #define CTS_IMR_MUI				((uint32_t)0x00000800)	/**< The mask of USER_INTERRUPT  */

// /**
//  * @brief Bits definition of TTP_TOCFR register
//  */
// #define CTS_TOCF				((uint32_t)0xFFFFFFFF)	/**< Timer overflow counter field */

// /**
//  * @brief Bits definition of TTP_TSFR register
//  */
// #define CTS_TSF					((uint32_t)0x0000FFFF)	/**< Timer startup field */
// /**@}*/// end of group Control_Section

// /**
//  * @defgroup TTP_CNT
//  */
// /**@{*/
// /**
//  * @brief Bits definition of TTP_CNTR0 and TTP_CNTR1 registers
//  */
// #define CNT_FSCNT				((uint32_t)0xFF000000)	/**< Failed slots counter */
// #define CNT_ASCNT				((uint32_t)0x00FF0000)	/**< Agreed slots counter */
// #define CNT_IGCNT				((uint32_t)0x0000FF00)	/**< Integration counter */
// #define CNT_CSCNT				((uint32_t)0x000000FF)	/**< Cold start counter */
// #define CNT_MFCNT				((uint32_t)0x000000FF)	/**< Membership failed counter */

// #define CNT_FSMF				((uint32_t)0x00FF0000)	/**< The first successor membership position */

// /**@}*/// end of group TTP_CNT

// /**@}*/// end of the group Regs_Bits_Def 

#endif

/**
 * @defgroup TTPC_CNI_Def
 */
/**@{*/

#define __RO const

/**
 * The 16 address-continuous status registers for TTPC ontroller. These registers shall
 * be physically addressed at compiled time. The CNI, normally, is a dual-port sram, 
 * within the status information from the inner of TTPC controller and the control
 * information to it. Also, the message area is defined in CNI specification.
 */

/**
 * @defgroup Status_Area
 */
/**@{*/

/**
 * The ttpc_status_regs_group memory region should be read-only for the host application.
 */

extern uint32_t __ttpc_status_regs_group[16];

/*
 * TTP Global Time Field Register definition.
 * This register specifies the macrotick value of the start time of the transmission phase 
 * (the AT time) in the current slot. The global time shall be updated during the pre-send
 * phase with respect to the current value of the global time and with respect to the 
 * current round slot position.
 */
#define TTP_CSR0 				__ttpc_status_regs_group[0]
#define TTP_GTR 				TTP_CSR0		/**< The alias of TTP_CRS0 */

/**
 * TTP Cluster Position Field Register definition.
 * This register specifies the information about the current round slot position, the 
 * current cluster mode and the pending deferred mode change(DMC).
 *  - The DMC field contains the value of the requested successor cluster mode
 * 	- the cluster mode field contains the value of the current active cluster 
 * 	  mode the TTPC controller is operating in. The TTPC controller updates the
 * 	  cluster mode field accordingly.
 * 	- The round slot position field contains the current cluster position information.
 * 	  The round slot position is zero based and updated in the pre-send phase. 
 */
#define TTP_CSR1				__ttpc_status_regs_group[1]
#define TTP_CPR  				TTP_CSR1		/**< The alias of TTP_CRS1 */

/**
 * TTP Membership Vector Register Group definition.
 * These registers specify a consistent view of the operational status of all nodes in the
 * cluster. A receiving node updates its membership vector in the post-receive-phase(PRP)
 * based on its perceived status of the received frame. A sending node updates its membership
 * vector flag according to the acknowledgment algorithm. 
 */

/** [TTP_CRS2 (TTP_MV0)			 membership vector 0 ] */
#define TTP_CSR2 				__ttpc_status_regs_group[2]							
#define TTP_MV0	 				TTP_CSR2
/** [TTP_CSR3 (TTP_MV1)			 membership vector 1] */
#define TTP_CSR3 				__ttpc_status_regs_group[3]	
#define TTP_MVF1				TTP_CSR3
/** [TTP_CSR4 (TTP_MV2)			 membership vector 2] */
#define TTP_CSR4 				__ttpc_status_regs_group[4]	
#define TTP_MVF2				TTP_CSR4
/** [TTP_CSR5 (TTP_MV3)			 membership vector 3] */
#define TTP_CSR5 				__ttpc_status_regs_group[5]	
#define TTP_MVF3				TTP_CSR5

/**
 * C-state structure definition. 32-bits register is specified for every item of c-state.
 * Attention that the CNI registers are all 32-bits, But the 16-bit is needed according to
 * the standard AS6003. The high 16 bits are reserved for every register of c-state field.
 * DO NOT FILL FRAMES WITH THE C-STATE DEFINED BELOW FOR FRAMES ASSEMBLING. THEY ARE BIT- 
 * INCOMPATIBLE.
 *                                                                            
 15                                                                      0 
 +-----------------------------------------------------------------------+ 
 |                      GLOBAL TIME  BASE:16                             | 
 +-------+-----------+---------------------------------------------------+ 
 | DMC:2 |  MODE:3   |                      ROUND SLOT:11                | 
 +-------+-----------+---------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 15-0                             | 
 +-----------------------------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 31-16                            | 
 +-----------------------------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 47-48                            | 
 +-----------------------------------------------------------------------+ 
 |                      MEMBERSHIP FLAG 63-48                            | 
 +-----------------------------------------------------------------------+ 
 */

#define C_STATE_BASE 			(&__ttpc_status_regs_group[0])
#define C_STATE_GT  			TTP_GTR
#define C_STATE_CP 				TTP_CPR
#define C_STATE_MV0 			TTP_MV0 		/**< bit 15:00 */
#define C_STATE_MV1 			TTP_MV1 		/**< bit 31:16 */
#define C_STATE_MV2 			TTP_MV2 		/**< bit 47:32 */
#define C_STATE_MV3 			TTP_MV3			/**< bit 63:48 */	

/**
 * TTP Status Flags Register Definition.
 * This register specifies the internal protocol errors detected by the controller. They are
 * updated as soon as the appropriate error condition is detected by the controller, and at
 * the same time an interrupt may be raised. The flags is cleared at the controller 
 * initialization; the controller immediately freezes upon the occurrence of any protocol
 * error.
 */
/**                                                                                                                                                                 
  31-21 20    19   18   17    16   15-12   11    10    9    8   7-5   4    3-0   
 +----+----+----+-----+-----+-----+-----+-----+-----+-----+----+----+----+-----+ 
 | REV| CE | SE | ME  | CB  | BE  | REV | SO  | MV  |  NR | CC | REV| MC |  PS | 
 +----+----+----+-----+-----+-----+-----+-----+-----+-----+----+----+----+-----+ 

 */

#define TTP_SR 					__ttpc_status_regs_group[6]

/**
 * This register contains information of the TTPC controller and the protocol version used.
 * Each make of TTPC controller must have its own unique identifier and reports it to the
 * host. A 96-bits serial number of each individual controller is needed.
 */
#define TTP_IDR0 				__ttpc_status_regs_group[7]
#define TTP_IDR1 				__ttpc_status_regs_group[8]
#define TTP_IDR2				__ttpc_status_regs_group[9]

#define TTP_ID_BITS 			96
#define TTP_ID_BASE				(&__ttpc_status_regs_group[7])
typedef uint8_t 				ttp_id[12];

#define TTP_12_bytes_ID 		((uint8_t*)(TTP_ID_BASE))

/**
 * TTP Cluster Schedule Identification Register definition.
 * This register specifies a n identifier of the abstract MEDL, i.e.the cluster design. The
 * cluster designer has to ensure that the schedule IDs differ for different cluster designs.
 * The contents are used for the frame CRC calculation to guarantee that only node based on 
 * the same cluster design can participate the same cluster.
 */
#define CSID_SIZE 				4
#define TTP_IDCR 				__ttpc_status_regs_group[10]

/**
 * TTP Application Identification Register definition.
 * Unique ID used by the host application to verify whether the actual MEDL is compatible to
 * it or not. Although the Application ID is not used by the TTP/C controller, it ensure a 
 * correct cooperation between the controller and the host. The application ID may consist 
 * of several subparts that are generated by the cluster schedule and the node design tools.
 */
#define APPID_SIZE 				4
#define TTP_IDAR 				__ttpc_status_regs_group[11]

/**
 * TTP Logical Name Register definition.
 * The LNSP part of this register is used to indicate the particular slot for frame sending
 * and clique detecting. And the LNMID part of this register is used to distinguish between
 * several multiplexed nodes which share the slot determined by the slot position part.
 * Multiplexed nodes do not trainsmit every TDMA round -- the rounds in which they transmit
 * are determined by the round  slot entries and may therefore differ for different cluster
 * modes -- but they do  perform the checks determined by the  slot posotion part in  every
 * round, regardless of the cluster modes.
 * If the logical  name specifies a  non-multiplexed role, the  multiplexed ID part  of the
 * logical name is set to zero.
 */
/**                                                                                                                                 
               31-16                         15-0                
 +------------------------------+------------------------------+ 
 |              LNSP            |           LNMID              | 
 +------------------------------+------------------------------+ 

 */
#define TTP_LNR 				__ttpc_status_regs_group[12]

/**
 * TTP Controller Life Sign  Register Definition.
 * This registers specifies a periodic life-sign information to the host, if no other
 * frequently updated  fields are used for a  life-sign mechanism.  This life-sign is 
 * updated in its own sending slot. 
 */
#define TTP_CLFSR 				__ttpc_status_regs_group[13]

/**
 * TTP Cluster Time Field Register definition.
 * This register specifies the globally synchronized time with a granularity of one macrotick.
 * The register is read only for the host.  The value of this register  is not defined if the 
 * C-state is not valid.
 */
#define TTP_CTFR 				__ttpc_status_regs_group[14]

/**
 * TTP Interrupt Status Register definition.
 * This register specifies the current status of interrupt from the TTPC controller.
 */
/**
                                                                        
      31-16      15  14-11   10    9     8   7-4     3     2     1      0     
 +------------+-----+-----+-----+-----+-----+-----+-----+-----+------+------+ 
 |    REV     |  TI | REV | UI  | ML  | CV  | REV | CR  |  HE |  PE  |  BR  | 
 +------------+-----+-----+-----+-----+-----+-----+-----+-----+------+------+ 
                                                                              
 */
#define TTP_ISR 				__ttpc_status_regs_group[15]

/**@}*/// end of group Status_Area

/**
 * @defgroup Control_Area
 */
/**@{*/

extern uint32_t __ttpc_control_notifier;

/**
 * TTPC control register number 0 definition.
 * This register contains all the control bits accessing all the module of the TTPC
 * controller, inlcuding START and STOP, DOWNLOAD,  BUILD-IN-TESTING,   MODE CHANGE
 * REQUEST, INTERRUPT SWITH, INTERRUPT MASK and so on.
 */
#define TTP_CR0 				__ttpc_control_notifier

extern uint32_t __ttpc_control_info[4];

/**
 * TTPC control register number 1 (Host Life-Sign) Register definition.
 * This register specifies the periodically updated life-sign that the host shall provide
 * to the TTPC controller.
 */
#define TTP_CR1 				__ttpc_control_info[0]
#define TTP_HLSR 				TTP_CR1

/**
 * TTPC control register number 2 (Timer ) definition.
 * This register is used to configure the user timer and the system timer. The system
 * timer may be used to driver the schedule of the operation system of the host.
 */
#define TTP_CR2 				__ttpc_control_info[1]
#define TTP_TIMR 				TTP_CR2

/**
 * TTP control register number 3 (Timer Overflow Counter Register) definition.
 * This register specifies a counter which is initialized with 0 upon a reset of the
 * controller and is incremented each time an overflow if the cluster time occurs. This
 * register can be set by the host to any value and can be used to build a larger ranged
 * time base with macotick granularity, but only the synchronized cluster part is equal 
 * to all nodes; the timer overflow counter can be synchronized by the host applications.
 */
#define TTP_CR3 				__ttpc_control_info[2]
#define TTP_TOCR 				TTP_CR3

/*
 * TTPC control register number 4 (Time Startup and External Rate Correction) definition.
 * This register is used to set the global time of the startup phase and set the external
 * rate correction term.
 * The TSP part of the register is used for synchronized cluster startup. The C-state time
 * value transmitted in the first cold start frame of the controller contains this value.
 * The controller reads this value at a defined point in time before transmitting the cold
 * start frame (this point in time is available in the controller data-sheet), the host must
 * therefore update the field accordingly. Synchronized cluster startup is typically used in
 * conjunction with inter-cluster synchronization.
 * The ERC part of the register specifies a common mode rate correction term in units of 
 * nano-second per resynchronization interval, which is added to the correction term of the
 * TTPC controller internal clock synchronization algorithm. The TTPC controller will regard
 * the value of the field only if the AR flag is set. The contents of the field are bounded 
 * by 1/2 PI. If the host violates the condition, a synchronization error will be raised. 
 */
 #define TTP_CR4 				__ttpc_control_info[3]

/**@}*/// end of group Control_Area

/**
 * @defgroup Msg_Area
 */
/**@{*/

#define MSG_POOL_LENGTH 		1024
 
extern uint8_t msg_pool[MSG_POOL_LENGTH];

#define MSG_POOL_BASE			msg_pool

/**@}*/// end of group Msg_Area

/**@}*/// end of group TTPC_RegType_Def

/**
 * @defgroup TTPC_Constants
 */
/**@{*/

/** for register TTP_CSR2 (TTP_CP) */

#define CS_CP_DMC				((uint32_t)0x0000C000)	/**< Deferred pending mode changes */
	#define DMC_NO_REQ			((uint32_t)0x00000000)	/**< No pending mode change request */
	#define DMC_MODE_1			((uint32_t)0x00004000)	/**< First successor mode */
	#define DMC_MODE_2			((uint32_t)0x00008000)	/**< Second successor mode */
	#define DMC_MODE_3			((uint32_t)0x0000C000)	/**< Third successor mode */
#define CS_CP_MODE				((uint32_t)0x00003800)	/**< Mode */
	//#define MODE_NO				((uint32_t)0x00000000)	/**< Node mode change request */
	#define MODE_1 				((uint32_t)0x00000800)	/**< First  successor mode requested */
	#define MODE_2 				((uint32_t)0x00001000)	/**< Second successor mode requested */
	#define MODE_3 				((uint32_t)0x00001800)	/**< Third  successor mode requested */
	#define MODE_CS_ID          ((uint32_t)0x00003800)  /**< mode used for startup */
	//#define MODE_CLR			((uint32_t)0x00002000)	/**< Clear pending mode change request */

#define IS_TTP_DMC(DMC)			(((DMC) == DMC_NO_REQ) || \
								 ((DMC) == DMC_MODE_1) || \
								 ((DMC) == DMC_MODE_2) || \
								 ((DMC) == DMC_MODE_3))

#define IS_TTP_MODE(MODE)		(((MODE) == MODE_NO) || \
								 ((MODE) == MODE_1)  || \
								 ((MODE) == MODE_2)  || \
								 ((MODE) == MODE_3)  || \ 
								 ((MODE) == MODE_CLR))
/** for register TTP_SR */

#define SR_PS 					((uint32_t)0x0000000F)	/**< Protocol state machine */
	#define PS_FREEZE			((uint32_t)0x00000000)	/**< Freeze state */
	#define PS_INIT				((uint32_t)0x00000001)	/**< Initialization state */
	#define PS_LISTEN			((uint32_t)0x00000002)	/**< Listen state */
	#define PS_COLDSTART		((uint32_t)0x00000003)	/**< Cold start state */
	#define PS_ACTIVE			((uint32_t)0x00000004)	/**< Active state */
	#define PS_PASSIVE			((uint32_t)0x00000005)	/**< Passive state */
	#define PS_AWAIT			((uint32_t)0x00000006)	/**< Await state */
	#define PS_TEST				((uint32_t)0x00000007)	/**< Test state */
	#define PS_DOWNLOAD			((uint32_t)0x00000008)	/**< Download state */

#define IS_TTP_PS(PS)			(((PS) == PS_FREEZE)    || \
								 ((PS) == PS_INIT)      || \
								 ((PS) == PS_LISTEN)    || \
								 ((PS) == PS_COLDSTART) || \
								 ((PS) == PS_ACTIVE)    || \
								 ((PS) == PS_PASSIVE)   || \
								 ((PS) == PS_AWAIT)     || \
								 ((PS) == PS_TEST)      || \
								 ((PS) == PS_DOWNLOAD))

#define SR_MC 					((uint32_t)0x00000010)	/**< MEDL error flag bit */
#define SR_CC 					((uint32_t)0x00000100)	/**< Concurrency control error flag bit */
#define SR_NR 					((uint32_t)0x00000200)	/**< Frame not ready error flag bit */
#define SR_MV					((uint32_t)0x00000400)	/**< Mode violation error flag bit */
#define SR_SO 					((uint32_t)0x00000800)	/**< Slot occupied error flag error bit */
#define SR_BE					((uint32_t)0x00010000)	/**< Bus Guardian error flag bit */
#define SR_CB 					((uint32_t)0x00020000)	/**< Communication system blackout error flag bit */
#define SR_ME 					((uint32_t)0x00040000) 	/**< Membership error flag bit */				
#define SR_SE 					((uint32_t)0x00080000)	/**< Synchronization error flag bit */
#define SR_CE 					((uint32_t)0x00100000)	/**< Clique error flag bit */	

/** for register TTP_ISR */
#define ISR_TI					((uint32_t)0x00010000)	/**< Timer interrupt state */
#define ISR_UI					((uint32_t)0x00000400)	/**< User interrupt state */
#define ISR_ML					((uint32_t)0x00000200)	/**< membership loss state */
#define ISR_CV					((uint32_t)0x00000100)	/**< C-state valid state */
#define ISR_CR					((uint32_t)0x00000008)	/**< Controller ready state */
#define ISR_HE					((uint32_t)0x00000004)	/**< Host error state */
#define ISR_PE					((uint32_t)0x00000002)	/**< Protocol error state */
#define ISR_BR					((uint32_t)0x00000001)	/**< BIST error state */

/** for register TTP_CR0 */
#define CR_IE					((uint32_t)0x00000040)	/**< Interrupt enable */
#define CR_MCR 					((uint32_t)0x00000038)  /**< the same as CS_CP_MODE */
	#define MCR_NO_REQ			((uint32_t)0x00000000)	/**< Node mode change request */
	#define MCR_MODE_1 			((uint32_t)0x00000008)	/**< First  successor mode requested */
	#define MCR_MODE_2 			((uint32_t)0x00000010)	/**< Second successor mode requested */
	#define MCR_MODE_3 			((uint32_t)0x00000018)	/**< Third  successor mode requested */
	#define MCR_MODE_CLR		((uint32_t)0x00000020)	/**< Clear pending mode change request */
#define CR_BIST					((uint32_t)0x00000004)	/**< Built-in test enable flag */
#define CR_CA					((uint32_t)0x00000002)	/**< Control await enable flag */
#define CR_CO					((uint32_t)0x00000001)	/**< Controller on */

#define CR_MTI 					((uint32_t)0x80000000)	/**< The mask of TIMER_INTERRUPT */
#define CR_MUI					((uint32_t)0x04000000)	/**< The mask of USER_INTERRUPT  */
#define CR_MML 					((uint32_t)0x02000000)	/**< The mask of MEMBERSHIP_LOSS */
#define CR_MCV 					((uint32_t)0x01000000)	/**< The mask of C-STATE_VALID */
#define CR_MCRI 				((uint32_t)0x00080000)	/**< The mask of CONTROLLER_READY */
#define CR_MHE 					((uint32_t)0x00040000)	/**< The mask of HOST_ERROR */
#define CR_MPE					((uint32_t)0x00020000)	/**< The mask of PROTOCOL_ERROR */
#define CR_MBR 					((uint32_t)0x00010000)	/**< The mask of BIST_ERROR */

#define CR_UO 					((uint32_t)0x00008000)	/**< User timer on */
#define CR_TO 					((uint32_t)0x00004000)	/**< Tick timer on */

/** for register TTP_CR2(TTP_TIMR) */
#define TIM_MODE1 				((uint32_t)0x00000600)	/**< user timer timing mode */
	#define MODE1_ONCE 			((uint32_t)0x00000200)	/**< trigger only once */	
	#define MODE1_PERIODIC 		((uint32_t)0x00000400) 	/**< trigger periodically */
#define TIM_MODE0 				((uint32_t)0x00000003)	/**< system tick timing mode */
	#define MODE0_ONCE			((uint32_t)0x00000001)	/**< trigger only once */
	#define MODE0_PERIODIC 		((uint32_t)0x00000002)	/**< trigger periodically */

#define IS_TTP_MODE0(MODE0)		(((MODE0)==MODE0_ONCE) || \
								 ((MODE0)==MODE0_PERIODIC))

#define IS_TTP_MODE1(MODE1)		(((MODE1)==MODE1_ONCE) || \
								 ((MODE1)==MODE1_PERIODIC))
/**@}*/// end of group TTPC_Constants

/**
 * @defgroup CNI_Operations
 */
/**@{*/

////////////////////////////////////////////////////////////////////////////////
///CNI access service                                                         //
////////////////////////////////////////////////////////////////////////////////

/**
 * TCN definition 
 * TTPC controller notifier register "CR0"
 */
#define TCN_ClrCO()				(TTP_CR0&=~CR_CO)
#define TCN_ClrCA()				(TTP_CR0&=~CR_CA)
#define TCN_ClrBIST() 			(TTP_CR0&=~CR_BIST)

/**
 * Set and clear the corresponding bit of the TTP_SR register of CNI.
 * @param SR the status bit
 *        @arg SR_MC     MEDL error flag bit
          @arg SR_CC     Concurrency control error flag bit 
          @arg SR_NR     Frame not ready error flag bit 
          @arg SR_MV     Mode violation error flag bit 
          @arg SR_SO     Slot occupied error flag error bit 
          @arg SR_BE     Bus Guardian error flag bit 
          @arg SR_CB     Communication system blackout error flag bit 
          @arg SR_ME     Membership error flag bit              
          @arg SR_SE     Synchronization error flag bit 
          @arg SR_CE     Clique error flag bit 
 */
#define CNI_SetSRBit(SR) 			(TTP_SR|=(SR))
#define CNI_ClrSRBit(SR) 			(TTP_SR&=~(SR))
#define CNI_ClrSRAll() 				(TTP_SR=0)
#define CNI_GetSRBit(SR) 			(TTP_SR&(SR))

/**
 * Set and Get the corresbonding protocol state of the TTP_SR register of CNI
 * @param PS the protocol state
          @arg PS_FREEZE    
          @arg PS_INIT      
          @arg PS_LISTEN    
          @arg PS_COLDSTART
          @arg PS_ACTIVE    
          @arg PS_PASSIVE   
          @arg PS_AWAIT 
          @arg PS_TEST      
          @arg PS_DOWNLOAD  
 */
#define CNI_GetPS() 				(TTP_SR&SR_PS)
#define CNI_SetPS(ps) 				(TTP_SR=(TTP_SR&(~SR_PS))|((ps)&SR_PS))

/** id is a type of uint8_t[12] */
#define CNI_GetUID(id) 				do{\
										int i = 0;
										for(;i<sizeof(ttp_id);i++)
											id[i] = TTP_12_bytes_ID[i];
									}while(0)
/**
 * The CSID and the APPID are type of uint32_t, meaning that the direct
 * assigning is allowed in 32-bits platform.
 */
#define CNI_GetCSID() 		 		(TTP_IDCR)	
#define CNI_GetAPPID() 				(TTP_IDAR)

/** UID related */
#define CNI_GetLNSP() 				(TTP_LNR>>16)
#define CNI_SetLNSP(lnsp) 			(TTP_LNR=(TTP_LNR&(~0xffff))|((lnsp)&0xffff))
#define CNI_GetLNMID() 				
 

/** controller life sign related */
#define CNI_UpdateCLFS() 			(++TTP_CLFSR)

/** cluster time field related */
/**
 *  Notice the overflow of the CTF. The function below will not handle the overflow,
 *  and the TTPC overflow field will be updated if the overflow of TTP_CTFR happens
 *  by a specific interruption.
 *  @Attention The function shall be instantiated by hardware.
 */
#define CNI_UpdateCTF() 			(++TTP_CTFR)
#define CNI_GetCTF() 				(TTP_CTFR)

/**
 * Set the corresbonding isr bit. The controller has no qualification to
 * clear any bit. All bits will be clears when the controller powers on.
 * @param  isr the interruption service routine bit.
 *      @arg ISR_TI   Timer interrupt state 
        @arg ISR_UI   User interrupt state 
        @arg ISR_ML   membership loss state 
        @arg ISR_CV   C-state valid state 
        @arg ISR_CR   Controller ready state 
        @arg ISR_HE   Host error state 
        @arg ISR_PE   Protocol error state 
        @arg ISR_BR   BIST error state 
 */
#define CNI_SetISRBit(isr) 			(TTP_ISR|=(isr))
//void     CNI_RaiseISR(void);

/**
 * Get the current mode change requst from the cni.
 * @return  the mode change request value
        @arg MCR_NO_REQ     Node mode change request 
        @arg MCR_MODE_1     First  successor mode requested 
        @arg MCR_MODE_2     Second successor mode requested 
        @arg MCR_MODE_3     Third  successor mode requested 
        @arg MCR_MODE_CLR   Clear pending mode change request 
 */
#define CNI_GetCurMCR() 			(TTP_CR0&CR_MCR)

#define CNI_IsModeChangeRequsted()  ({uint32_t _mcr = CNI_GetCurMCR(); (_mcr == MCR_MODE_1) || \
															  		   (_mcr == MCR_MODE_2) || \
															  		   (_mcr == MCR_MODE_3);})
#define CNI_ClrMCR() 				(TTP_CR0 = (TTP_CR0&~CR_MCR)|MCR_MODE_CLR)
/**
 * check whether the HLFS(Host Life Sign) is valid or not. If the HLFS is not
 * valid, it will return 0. The checking operation shall be performed during
 * PSP phase for once, because the controller will clear it after checking. 
 * @return  check result.
 *       @arg 0  host not valid
 *       @arg !0 host ok
 */
#define CNI_CheckHLFS() 			({uint32_t _hls=TTP_HLSR; TTP_HLSR=0; _hls>0?1:0;})

/** increase the TOC(Global Time Overflow Counter) */
/**
 * The function below will not handle the overflow situation of TOC register.
 */
#define CNI_UpdateTOC() 			(TTP_TOCR++)

/** get the TSF(Time startup field) */
#define CNI_GetTSF() 				(TTP_CR4>>16)

/** get the ERC(External Rate Correction) */
#define CNI_GetERC() 				(TTP_CR4&0xffff)

/*******************************************************************************/
/**
 * c-state operations definitions below, which are the part of CNI access
 * services
 */
/** macro definition for c_state access, getters and setters */

/** getters definitions below */
#define CS_GetCurRoundSlot() 		(C_STATE_CP&0x07ff)
#define CS_GetCurMode()				(C_STATE_CP&0x3800)
#define CS_GetCurDMC()				(C_STATE_CP&0xc000)
#define CS_GetCurGTF()				(C_STATE_GT)
#define CS_GetMemberBit(pos)		(((*(C_STATE_BASE+((pos)/16)+2))>>((pos)%16))&1)

/** setters definitions below */
#define CS_SetRoundSlot(rs)			(C_STATE_CP=(C_STATE_CP&(~0x07ff))|((rs)&0x07ff))
#define CS_SetMode(md)				(C_STATE_CP=(C_STATE_CP&(~0x3800))|((md)&0x3800))
#define CS_SetDMC(mc)				(C_STATE_CP=(C_STATE_CP&(~0xc000))|((mc)&0xc000))
#define CS_SetGTF(gt)				(C_STATE_GT=(gt)&0xffff)
#define CS_SetMemberBit(pos)		((*(C_STATE_BASE+((pos)/16)+2))|=1<<((pos)%16))




/** the clearance definitions below */
#define CS_ClearMemberAll()			do{C_STATE_MV0=0;\
										   C_STATE_MV1=0;\
										   C_STATE_MV2=0;\
										   C_STATE_MV3=0;\
									}while(0)

#define CS_ClearMemberBit(pos)		((*(C_STATE_BASE+((pos)/16)+2))&=~(1<<((pos)%16)))

/*******************************************************************************/
/**
 * messages operations definitions below, which are the part of CNI access
 * services
 */

/**
 * the function MSG_CheckMsgRF is used to read Message-Ready-Flag of the corresponding
 * message, which is stored in the CNI, posited in address MSG_POOL_BASE + offset + 1.
 * When the controller sends a message, it must check the validity of this field, then 
 * clear it. When the controller receives a message, the Message-Ready-Flag is used 
 * for message status indicating.
 *
 * the status of frame-ready indicating. When the host ensures that the frame to 
 * be sent is OK, it shall write the particular bit pattern "0xEA" to the first
 * byte offset of the CNIAddressOffset of the corresponding slot configuration 
 * parameters, which means that the position of the application data starts from
 * the second byte. 
 * The host shall confirm the frame by writing the particular bit-pattern to the 
 * corresponding position before the controller sends the frame. if not, the
 * controller will raise the NR(Frame Not Ready) interruption and stop running.
 * The controller will clear the bit-pattern to zero after reading it out.
 */
#define STATUS_BIT_PATTERN     			((uint8_t)0xEA)

#define _RF_CLR(offset) 				((MSG_POOL_BASE+(offset))[0]=0,1)			
#define MSG_CheckMsgRF(offset) 			((MSG_POOL_BASE+(offset))[0]==STATUS_BIT_PATTERN?_RF_CLR(offset):0)
#undef  _RF_CLR

#define MSG_SetStatus(offset,status) 	((MSG_POOL_BASE+(offset))[0]=(status))	

//ensure that the CNI memory is a part of the memory of TTPC controller, having 
//the same address space. This macro-function returns the base address of the message
//field of the CNI. Notice the address type of uint8_t.
//attention the 1 byte status or NR.
#define MSG_GetMsgAddr(offset) 			(msg_pool+(offset)+1)		 		



/**@}*/// end of group CNI_Operations




/**@}*/// end of group TTPC_CNI_Def

#endif
