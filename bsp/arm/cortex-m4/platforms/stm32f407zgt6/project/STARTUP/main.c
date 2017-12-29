#include "led.h"
#include "lsens.h"
#include "clock.h"
#include "ttpdebug.h"
#include "dbgio.h"
#include <misc.h>
#include "crc.h"
#include "xfer.h"
#include "protocol.h"

//max speed is 849 microticks for the transission of the min ethernat packet

static inline void __board_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    IO_DepInit();
    
    LED_DepInit();
    Lsens_DepInit();
	CLOCK_DepInit();   
    CRC_DepInit();
    DRV_DepInit();
}    

int main(void)
{ 
    __board_init();
    PRINT("protocol start");
    
    FSM_reset();
    FSM_start();
    
    //never come here
    PRINT("protocol error");
}

