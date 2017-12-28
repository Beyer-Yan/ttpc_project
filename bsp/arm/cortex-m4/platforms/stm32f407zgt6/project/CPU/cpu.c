#include <misc.h>
#include "cpu.h"
#include "core_cmFunc.h"

void CPU_InterruptEnable(void)
{
    __enable_irq(); 
}
void CPU_InterruptDisable(void)
{
    __disable_irq();
}

void CPU_SoftwareInterrupt(void)
{
    //todo;
}

void CPU_Sleep(void)
{
    __WFI();
}








