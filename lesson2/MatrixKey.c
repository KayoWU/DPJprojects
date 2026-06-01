#include "MatrixKey.h"

uchar matrixKeyPress[] =   //矩阵键盘的标志位，初始值均为0
{
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0	
};     

#define FOSC  11059200L  //单片机主频
#define COUNT 1          // xx ms
#define TIME  (65536 - COUNT*FOSC/12/1000)   //16位定时器初值


//========================================================================
// 函数：void MatrixKeyInit(void)
// 描述：矩阵键盘的硬件初始化配置
// 参数：
// 返回：
// 版本：
//========================================================================
void MatrixKeyInit(void)
{
	TMOD = 0x01;  //定时器0在模式1下，即16位模式
	TL0 = TIME;   //定时1ms中断一次
	TH0 = TIME >> 8; 
	TR0 = 1;
	ET0 = 1; 	
	EA = 1;      //总中断	
}

//========================================================================
// 函数：void time0(void) interrupt 1
// 描述：矩阵键盘扫描函数，使用定时器0中断函数实现
// 参数：
// 返回：
// 版本：
//========================================================================
void time0(void) interrupt 1
{
	static uchar keyStateCurrent[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                 keyStateLast[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};	
	uchar num;      //循环扫描4行按键
	uchar temp;     //0001 0000 ; 0010 0000; 0100 0000; 1000 0000
	uint count0;    //累计中断次数
    uchar code keyIndex[] = {0xfe,0xfd,0xfb,0xf7};  //矩阵键盘逐行扫描	
	TL0 = TIME;   //定时50ms中断一次
	TH0 = TIME >> 8;	
	count0++;  // count = count + 1;
	if(count0 >= 5)  //5ms时间到
	{
		count0 = 0;
		for(num = 0; num < 4; num++)  //for循环，矩阵键盘逐行扫描，一共4次
		{
			P3 = keyIndex[num];  //1111 1110; 0xfe,0xfd,0xfb,0xf7
			temp = P3;           //1111 1110
			temp = temp & 0xf0;  // 1111 1110 & 1111 0000 = 1111 0000			
			switch(temp)
			{
				case 0xf0:
					keyStateCurrent[0 + 4*num] = 1;
					keyStateCurrent[1 + 4*num] = 1;
					keyStateCurrent[2 + 4*num] = 1;
					keyStateCurrent[3 + 4*num] = 1;					
					break;
				case 0xe0:
					keyStateCurrent[0 + 4*num] = 0;			
					break;
				case 0xd0:
					keyStateCurrent[1 + 4*num] = 0;				
					break;
				case 0xb0:
					keyStateCurrent[2 + 4*num] = 0;				
					break;
				case 0x70:
					keyStateCurrent[3 + 4*num] = 0;				
					break;
				default:
					break;
			}
			if(keyStateCurrent[0 + 4*num] != keyStateLast[0 + 4*num])  //判断 一列 按键按下并输出按键标志位
			{
				if(keyStateLast[0 + 4*num] == 1)
				{
					matrixKeyPress[0 + 4*num] = 1;
				}
				keyStateLast[0 + 4*num] = keyStateCurrent[0 + 4*num];
			}	
			if(keyStateCurrent[1 + 4*num] != keyStateLast[1 + 4*num])  //判断 二列 按键按下并输出按键标志位
			{
				if(keyStateLast[1 + 4*num] == 1)
				{
					matrixKeyPress[1 + 4*num] = 1;
				}
				keyStateLast[1 + 4*num] = keyStateCurrent[1 + 4*num];
			}	
			if(keyStateCurrent[2 + 4*num] != keyStateLast[2 + 4*num])  //判断 三列 按键按下并输出按键标志位
			{
				if(keyStateLast[2 + 4*num] == 1)
				{
					matrixKeyPress[2 + 4*num] = 1;
				}
				keyStateLast[2 + 4*num] = keyStateCurrent[2 + 4*num];
			}	
			if(keyStateCurrent[3 + 4*num] != keyStateLast[3 + 4*num])  //判断 四列 按键按下并输出按键标志位
			{
				if(keyStateLast[3 + 4*num] == 1)
				{
					matrixKeyPress[3 + 4*num] = 1;
				}
				keyStateLast[3 + 4*num] = keyStateCurrent[3 + 4*num];
			}			
		}				
	}
}