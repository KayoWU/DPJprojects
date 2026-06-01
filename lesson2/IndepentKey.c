#include "IndepentKey.h"

uchar keyPress[] = {0, 0, 0, 0};  //四个独立按键的标志位，初始值均为0

#define FOSC  11059200L  //单片机主频
#define COUNT 1          // xx ms
#define TIME  (65536 - COUNT*FOSC/12/1000)   //16位定时器初值


//========================================================================
// 函数：void IndepentKeyInit(void)
// 描述：独立键盘的硬件初始化配置
// 参数：
// 返回：
// 版本：
//========================================================================
void IndepentKeyInit(void)
{
	TMOD = 0x01;  //定时器0在模式1下，即16位模式
	TL0 = TIME;   //定时50ms中断一次
	TH0 = TIME >> 8; 
	TR0 = 1;
	ET0 = 1; 	
	EA = 1;      //总中断	
}

//========================================================================
// 函数：independentKeyScan(void)
// 描述：独立键盘扫描函数，使用定时器0中断函数实现
// 参数：
// 返回：
// 版本：
//========================================================================
void time0(void) interrupt 1
{
	static uchar keyStateCurrent[] = {1, 1, 1, 1}, keyStateLast[] = {1, 1, 1, 1};	
	uchar index;
	uchar temp;     //0001 0000 ; 0010 0000; 0100 0000; 1000 0000
	uint count0;    //累计中断次数
	uchar keyTemp[] = {0x10, 0x20, 0x40, 0x80};	
	TL0 = TIME;   //定时50ms中断一次
	TH0 = TIME >> 8;	
	count0++;  // count = count + 1;
	if(count0 >= 5)  // 5ms时间到
	{
		count0 = 0;
		for(index = 0; index < 4; index++)  
		{
			temp = P3;
			temp = temp & keyTemp[index];
			if(temp != keyTemp[index])
			{
				keyStateCurrent[index] = 0;
			}
			if(temp == keyTemp[index])
			{
				keyStateCurrent[index] = 1;
			}	
			if(keyStateCurrent[index] != keyStateLast[index])
			{
				if(keyStateLast[index] == 1)
				{
					keyPress[index] = 1;  		
				}
				keyStateLast[index] = keyStateCurrent[index];			
			}	
		}			
	}
}