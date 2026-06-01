#include "DS18B20.h"
#include "Delay.h"

sbit DQ = P2^2; 	  //数据传输线接单片机的相应的引脚 
bit Tem_bit=1;        //温度正负标志


void Delay15us(void)	//@11.0592MHz
{
	unsigned char data i;

	i = 4;
	while (--i);
}

void Delay600us(void)	//@11.0592MHz
{
	unsigned char data i, j;

	_nop_();
	i = 2;
	j = 15;
	do
	{
		while (--j);
	} while (--i);
}

void Delay45us(void)	//@11.0592MHz
{
	unsigned char data i;

	i = 18;
	while (--i);
}


//========================================================================
// 函数：void Ds18b20Init(void) 
// 描述：DS18B20硬件初始化设置
// 参数：
// 返回：
// 版本：
//========================================================================
void Ds18b20Init(void) 
{
	uchar x = 0;
	DQ = 1; 			//DQ先置高 
	_nop_(); 			//稍延时
	DQ = 0; 			//发送复位脉冲 
	Delay600us(); 		//延时（>480us)   //（重要参数）
	DQ = 1; 			//拉高数据线 
	Delay45us(); 	    //等待（15~60us)  //  60us（卡在60us,重要参数）
	x = DQ; 		    //用X的值来判断初始化有没有成功，18B20存在的话X=0，否则X=1 
	Delay600us();       //（重要参数）
}

//========================================================================
// 函数：uchar ReadOneChar(void)  
// 描述：读一个字节，主机数据线先从高拉至低电平1us以上，再使数据线升为高电平，从而产生读信号
// 参数：
// 返回：
// 版本：
//========================================================================
uchar ReadOneChar(void)  	
{
	uchar i = 0; 		        //每个读周期最短的持续时间为60us，各个读周期之间必须有1us以上的高电平恢复期
	uchar dat = 0; 
	for(i = 0; i < 8; i++) 		//一个字节有8位 
	{
		EA = 0;         // 非常重要的 指令，本程序里的中断会打断 DS18B20的 读写时序时间，此段需要先关掉中断，如果不关闭中断读温度将不可控
		DQ = 0;
		_nop_();        // (重要参数)
		DQ = 1;
		dat >>= 1; 
		DQ = 1; 
		_nop_();        // (重要参数)	
		if(DQ) 
		{
			dat|=0x80; 			
		}
		Delay45us();    // (重要参数)
		EA=1;           // 非常重要的 指令，读完后打开中断
	} 
	return(dat);
}

//========================================================================
// 函数：void WriteOneChar(uchar dat)   
// 描述：写一个字节
// 参数：
// 返回：
// 版本：
//========================================================================
void WriteOneChar(uchar dat)  
{ 
	uchar i = 0; 		     //数据线从高电平拉至低电平，产生写起始信号。15us之内将所需写的位送到数据线上，
	for(i = 0; i < 8; i++) 	 //在15~60us之间对数据线进行采样，如果是高电平就写1，低写0发生。 
	{ 
 	    EA = 0;              // 非常重要的 指令，本程序里的中断会打断 DS18B20的 读写时序时间，此段需要先关掉中断，如果不关闭中断读温度将不可控
		DQ = 0; 				
        Delay15us(); 		
		DQ = dat & 0x01; 
		Delay45us(); 
		DQ = 1; 
		dat >>= 1;
		EA = 1;              // 非常重要的 指令，写完后打开中断
	} 
}

//========================================================================
// 函数：uint ReadTemperature(void)   
// 描述：读取温度
// 参数：
// 返回：
// 版本：
//========================================================================
uint ReadTemperature(void)        
{ 
	uchar tempL=0; 		  //设全局变量,低4位数据
	uchar tempH=0;        //高8位数据
	uchar sdata;		  //测量到的温度的整数部分
	uchar xiaoshu1;		  //小数第一位
	uchar xiaoshu2;		  //小数第二位	
	uint temData;      
	Ds18b20Init(); 					    //初始化
	WriteOneChar(0xcc); 				//跳过读序列号的操作
	WriteOneChar(0x44); 				//启动温度转换
	delay(800); 						//转换需要一点时间，延时 
	Ds18b20Init(); 					    //初始化
	WriteOneChar(0xcc); 				//跳过读序列号的操作  
	WriteOneChar(0xbe); 				//读温度寄存器（头两个值分别为温度的低位和高位） 
	tempL = ReadOneChar(); 				//读出温度的低位LSB
	tempH = ReadOneChar(); 				//读出温度的高位MSB	
	if(tempH > 0x7f)      				//最高位为1时温度是负
	{
		tempL = ~tempL;					//补码转换，取反加一
		tempH = ~tempH+1;       
		Tem_bit = 0;      				//读取温度为负时Tem_bit=0
	}
	sdata = tempL/16 + tempH*16;      	//整数部分
	xiaoshu1 = (tempL&0x0f)*10/16; 		//小数第一位
	xiaoshu2 = (tempL&0x0f)*100/16%10;	//小数第二位
	temData = sdata * 100 + xiaoshu1 * 10 + xiaoshu2;   //整合小数和整数部分，合并成一个整数并返回
	return temData;
}


