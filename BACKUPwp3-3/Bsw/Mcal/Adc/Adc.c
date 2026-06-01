#include "Adc.h"

sbit Adc_Cs = P0^7;
sbit Adc_Wr = P3^6;
sbit Adc_Rd = P3^7;
sbit Adc_Wei = P2^7;

/* ADC初始化：选通ADC相关锁存/片选信号，让后续可以启动转换。 */
void Adc_Init(void)
{
    Adc_Wei = 1;
    Adc_Cs = 0;
    Adc_Wei = 0;
}

/* 读取ADC原始值：用WR启动转换，用RD读回P1上的8位数据。 */
uint8 Adc_ReadValue(void)
{
    uint8 adcData;

    Adc_Wr = 1;
    _nop_();
    Adc_Wr = 0;
    _nop_();
    Adc_Wr = 1;
    _nop_();
    Adc_Rd = 1;
    _nop_();
    Adc_Rd = 0;
    _nop_();
    adcData = P1;
    Adc_Rd = 1;

    return adcData;
}

/* 把ADC原始值换算成毫伏：0对应0mV，255对应配置的参考电压。 */
uint16 Adc_ConvertToMv(uint8 adcValue)
{
    return (uint16)(((uint32)ADC_REFERENCE_MV * adcValue) / 255u);
}
