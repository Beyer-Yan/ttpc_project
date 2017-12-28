#include "led.h"
#include "lsens.h"
#include "clock.h"
#include "ttpdebug.h"
#include "dbgio.h"
#include <misc.h>
#include <stdarg.h>
#include "crc.h"
#include "xfer.h"
#include "ttpmac.h"

//max speed is 849 microticks for the transission of the min ethernat packet

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	LED_DepInit();
	CLOCK_DepInit();
	IO_DepInit();   
    Lsens_DepInit();
    DRV_DepInit();
    
    CLOCK_Start();
    
    uint32_t i = 0;
    
    while(1)
    {
        uint32_t mi = TIM2->CNT;
        uint16_t ma = TIM4->CNT;

        DRV_PushData((uint8_t*)&i,4);
        DRV_PushData((uint8_t*)&mi,4);
        DRV_PushData((uint8_t*)&ma,2);
        DRV_PrepareToTransmit();
            
        //time preparing to transmit
        uint32_t mi_s = TIM2->CNT;
        uint32_t mi_t = TIM2->CNT;
        
        INFO("tx timing %d",mi_t-mi_s);
        
        mi_s = TIM2->CNT;
        //ETH->DMATPDR = 0; 
        MAC_StartTransmit();
        
        CLOCK_WaitAlarm(2,0);
        mi_t = CLOCK_GetCaptureRX0();
        
        INFO("tx timing %d",mi_t-mi_s);
        DBG_Flush();
        
        //while(1);

        CLOCK_WaitAlarm(1000,0);
        
        //CLOCK_WaitAlarm(2,0); //3us
        
        //DRV_CheckTransmitted();
        
        i++;
    }
}

