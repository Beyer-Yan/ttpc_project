#include "led.h"
#include "lsens.h"
#include "clock.h"
#include "ttpdebug.h"
#include "dbgio.h"
#include <misc.h>
#include "crc.h"
#include "xfer.h"
#include "protocol.h"

#include "host.h"

//max speed is 849 microticks for the transission of the min ethernat packet

static char _ps[10][10] = {"FREEZE","INIT","LISTEN","COLDSTART","SUB_CS","ACTIVE","PASSIVE","AWAIT","TEST","DOWNLOAD"};

static inline void __board_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    IO_DepInit();
    
    LED_DepInit();
    LSENSE_DepInit();
	CLOCK_DepInit();   
    CRC_DepInit();
    DRV_DepInit();

    HOST_Init();
}    

void _hook_debug(uint32_t ps_num)
{
    //INFO("transite into state %s",_ps[ps_num]);
}

int main(void)
{ 
    int i = 10000;
    __board_init();
    
    //PRINT("protocol start");
    while(i--);
    
    FSM_Reset();
    
    FSM_SetHook(_hook_debug);
    
    FSM_Start();
    
    //never come here
    //PRINT("protocol error");
}

