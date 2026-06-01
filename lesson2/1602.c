#include "1602.h"
#include "Delay.h"

#define LCD_DATA P0   //数据输入端口
sbit LCD_RS = P3^5;   //读、写控制端口
sbit LCD_EN = P3^4;   //使能1602液晶端口


//========================================================================
// 函数：void LcdWriteCom(uchar com)
// 描述：1602液晶写 指令 函数
// 参数：
// 返回：
// 版本：
//========================================================================
void LcdWriteCom(uchar com)
{
	LCD_EN = 0;
	LCD_RS = 0;
	LCD_DATA = com;
	LCD_EN = 1;
    delay(1);
	LCD_EN = 0;
}

//========================================================================
// 函数：void LcdWriteData(uchar dat)
// 描述：1602液晶写 数据 函数
// 参数：
// 返回：
// 版本：
//========================================================================
void LcdWriteData(uchar dat)
{
	LCD_EN = 0;
	LCD_RS = 1;
	LCD_DATA = dat;
	LCD_EN = 1;
	delay(1);
	LCD_EN = 0;
}

//========================================================================
// 函数：void Lcd1602Init(void)
// 描述：1602液晶初始化配置
// 参数：
// 返回：
// 版本：
//========================================================================
void Lcd1602Init(void)
{
	LcdWriteCom(0x38);   //功能设置：8位总线、2行显示、5*8字体；0011 1000
	delay(40);
	LcdWriteCom(0x0c);   //显示开、光标关、光标闪烁关； 0000 1100
	delay(40);
	LcdWriteCom(0x06);   //输入模式：自动右移、无位移； 0000 0110
	delay(40);
	LcdWriteCom(0x01);   //清屏
	delay(40);
}

//========================================================================
// 函数：void ShowStringAny(uchar index, uchar line, uchar a[])
// 描述：1602液晶任意位置显示字符串
// 参数：
// 返回：
// 版本：
//========================================================================
void ShowStringAny(uchar index, uchar line, uchar a[])
{
	uchar i;
	if((index != 1 && index != 2) || line < 1 || line > 40)
	{
		return;
	}	
	if(index == 1)
	{
		LcdWriteCom(0x80 + line - 1);	
	}
	if(index == 2)
	{
		LcdWriteCom(0x80 + 0x40 + line - 1);		
	}
	for(i = 0; a[i]!='\0'; i++)  // '\0' 当前的字符串结束了；"I love MCU !" '\0'
	{
		LcdWriteData(a[i]);
	}
}


//========================================================================
// 函数：void ShowContentAny(uchar index, uchar line, uchar date)
// 描述：1602液晶在任意位置显示任意字符
// 参数：index:1,2   line: 1-16  
// 返回：
// 版本：
//========================================================================
void ShowContentAny(uchar index, uchar line, uchar date)
{
	if((index != 1 && index != 2) || line < 1 || line > 40)
	{
		return;
	}
	if(index == 1)
	{
		LcdWriteCom(0x80 + line - 1);	
	}
	if(index == 2)
	{
		LcdWriteCom(0x80 + 0x40 + line - 1);		
	}
	LcdWriteData(date);
}


