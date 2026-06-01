#include "PWM.h"

#define FOSC  11059200L   //单片机主频
#define COUNT 1           // xx ms
#define TIME  (65536 - COUNT*FOSC/12/1000)  //16位定时器初值

sbit PWM_OUT = P1^0; //PWM信号输出端
uchar dutyCycle;     //占空比，具体数字就代表 x/255
//========================================================================
// 函数：void PwmInit(void)
// 描述：利用定时器来实现PWM输出，硬件的初始化配置
// 参数：
// 返回：
// 版本：
//========================================================================
void PwmInit(void)
{
	TMOD = 0x10;  //定时器1工作在模式1下，即16位模式 
	TL1 = TIME;   //定时1ms中断一次
	TH1 = TIME >> 8; 	
	TR1 = 1;
	ET1 = 1; 
	EA = 1;      //总中断
}

//========================================================================
// 函数：void time1(void) interrupt 3
// 描述：利用定时器来实现PWM输出
// 参数：
// 返回：
// 版本：
//========================================================================
void time1(void) interrupt 3
{
	static uchar pwmTime = 0;
	TL1 = TIME;   
	TH1 = TIME >> 8;	
	pwmTime++;
	if(pwmTime > 255)
	{
		pwmTime = 0;
	}	
	if(pwmTime <= dutyCycle)
	{
		PWM_OUT = 1;
	}
	else
	{
		PWM_OUT = 0;
	}		
}

