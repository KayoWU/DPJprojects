#include "INTx.h"

#define FOSC  11059200L  //单片机主频
#define COUNT 50         // xx ms
#define TIME  (65536 - COUNT*FOSC/12/1000)   //16位定时器初值

bit TimeFlag = 0;        //时间标志位

//========================================================================
// 函数：void InterruptInit(void)
// 描述：中断初始化配置
// 参数：
// 返回：
// 版本：
//========================================================================
void InterruptInit(void)
{	
    RCAP2L = TL2 = TIME;          
    RCAP2H = TH2 = TIME >> 8;       
    TR2 = 1;                        
    ET2 = 1;  	
	EA = 1;        //总中断
}

//========================================================================
// 函数：void InterruptInit(void)
// 描述：中断初始化配置
// 参数：
// 返回：
// 版本：
//========================================================================
void time2(void) interrupt 5
{
	static uchar count = 0;
	TF2 = 0;
	count++;  // count = count + 1;
	if(count >= 20)  // 1000ms时间到
	{
		count = 0;
		TimeFlag = 1;
	}
}

