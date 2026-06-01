#include "Adc.h"

sbit ADC_CS = P0^7;
sbit ADC_WR = P3^6;
sbit ADC_RD = P3^7;

sbit duan = P2^6;  //控制数码管 段 的芯片锁存器端
sbit wei = P2^7;   //控制数码管 位 的芯片锁存器端

//========================================================================
// 函数：void AdcInit(void)
// 描述：ADC芯片初始化配置
// 参数：
// 返回：
// 版本：
//========================================================================
void AdcInit(void)
{
	wei = 1;
	ADC_CS = 0;
	wei = 0;
}

//========================================================================
// 函数：uchar AdcDataGet(void)
// 描述：获取ADC数值
// 参数：
// 返回：
// 版本：
//========================================================================
uchar AdcDataGet(void)
{
	uchar adcData;
	ADC_WR = 1;
	_nop_();
	ADC_WR = 0;
	_nop_();
	ADC_WR = 1;
	_nop_();
	ADC_RD = 1;
	_nop_();
	ADC_RD = 0;
	_nop_();
	adcData = P1;	
	ADC_RD = 1;	
	return adcData;
}
