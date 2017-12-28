#include "led.h"
#include "lsens.h"
#include "clock.h"
#include "ttpdebug.h"
#include "dbgio.h"
#include <misc.h>
#include <stdarg.h>
#include "crc.h"
#include "xfer.h"

#define PRINTF(fmt,...) _print_now(fmt,##__VA_ARGS__)

char data[46] = {1,2,3,4,5,6,7,8,9,10,'h','e','l','l','w','o','r','l','d'};

void _print_now(const char* format,...)
{
    va_list ap;
    va_start(ap,format);
    
    if(!format)
        return;
    INFO(format,ap);
    DBG_Flush();
    va_end(ap);
}

void MAC_StartTransmit(void)
{
    DRV_StartTransmission();
}

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
    
    //PRINTF("test start");
    //CLOCK_WaitAlarm(100,0); 
    
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

