#include "crc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_crc.h"

static int _G_Idx = 0;
static uint32_t _G_BufferedData = 0xFFFFFFFF;


void CRC_DepInit(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_CRC,ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_CRC,DISABLE);
}
void CRC_ResetData(void)
{
    CRC_ResetDR();
    _G_Idx = 0;
    _G_BufferedData = 0xFFFFFFFF;
}
void CRC_PushData(uint8_t *pBuf, uint32_t BufLength)
{
    int i = 0;
    while(BufLength--)
    {
        ((uint8_t*)&_G_BufferedData)[3-_G_Idx] = pBuf[i];
        _G_Idx++;
        i++;
        
        if(_G_Idx==4)
        {
            CRC->DR = _G_BufferedData;
            _G_Idx = 0;
        }
    }
}

uint32_t CRC_GetResult(void)
{
    static uint32_t _mask[4] = {0,0x00ffffff,0x0000ffff,0x000000ff};
    if(_G_Idx!=0)
    {
        _G_BufferedData |= _mask[_G_Idx];
        CRC->DR = _G_BufferedData;
    } 
    return CRC->DR;
}
