#include "At24c02.h"

sbit sda = P2^0;   //数据线
sbit scl = P2^1;   //时钟线

//========================================================================
// 函数：void IIcStart(void)
// 描述：IIC 通信起始信号
// 参数：
// 返回：
// 版本：
//========================================================================
void IIcStart(void) 
{	
	sda = 1;
	_nop_();
	scl = 1;
	_nop_();
	sda = 0;
	_nop_();
}

//========================================================================
// 函数：void IIcStop(void) 
// 描述：IIC 通信停止信号
// 参数：
// 返回：
// 版本：
//========================================================================
void IIcStop(void)   
{
	sda = 0;
	_nop_();
	scl = 1;
	_nop_();
	sda = 1;
	_nop_();
}


//========================================================================
// 函数：void IIcRespons(void)
// 描述：IIC 通信应答信号
// 参数：
// 返回：
// 版本：
//========================================================================
void IIcRespons(void) 
{
	uchar i;
	scl = 1;
	_nop_();
	while((sda==1)&&(i<250))
	{
		i++;
	}
	scl = 0;
	_nop_();
}


//========================================================================
// 函数：void At24c02Init(void)
// 描述：AT24C02硬件初始化
// 参数：
// 返回：
// 版本：
//========================================================================
void At24c02Init(void)
{
	sda = 1;
	_nop_();
	scl = 1;
	_nop_();
}

//========================================================================
// 函数：void IIcWriteByte(uchar date)
// 描述：IIC通信写数据
// 参数：date：要写入的数据
// 返回：
// 版本：
//========================================================================
void IIcWriteByte(uchar date)
{
	uchar i,temp;
	temp = date;
	for(i=0; i<8; i++)
	{
		temp = temp << 1;
		scl = 0;
		_nop_();
		sda = CY;
		_nop_();
		scl = 1;
		_nop_();
	}
	scl = 0;
	_nop_();
	sda = 1;
	_nop_();
}


//========================================================================
// 函数：uchar IIcReadByte(void)
// 描述：IIC通信读数据
// 参数：
// 返回：返回读取的数据
// 版本：
//========================================================================
uchar IIcReadByte(void)
{
	uchar i, k;
	scl = 0;
	_nop_();
	sda = 1;
	_nop_();
	for(i=0; i < 8; i++)
	{
		scl = 1;
		_nop_();	
		k = (k<<1)|sda;
		scl = 0;
		_nop_();	
	}
	return k;
}


//========================================================================
// 函数：void At24c02WriteAdd(uchar address,uchar date)
// 描述：向 AT24C02 指定的地址 写数据
// 参数：address：AT24C02内部的EEPROM地址（0x00-0xff）; date: 要写入的数据
// 返回：
// 版本：
//========================================================================
void At24c02WriteAdd(uchar address,uchar date)
{
	IIcStart();
	IIcWriteByte(0xa0);  // 1010 0000 = 0xa0
	IIcRespons();
	IIcWriteByte(address);
	IIcRespons();
	IIcWriteByte(date);
	IIcRespons();
	IIcStop();
}


//========================================================================
// 函数：uchar At24c02ReadAdd(uchar address)
// 描述：向 AT24C02 指定的地址 读数据
// 参数：address：AT24C02内部的EEPROM地址（0x00-0xff）
// 返回：读取到的数据
// 版本：
//========================================================================
uchar At24c02ReadAdd(uchar address)
{
	uchar date;
	IIcStart();
	IIcWriteByte(0xa0);
	IIcRespons();
	IIcWriteByte(address);
	IIcRespons();
	IIcStart();
	IIcWriteByte(0xa1);
	IIcRespons();
	date = IIcReadByte();
	IIcStop();
	return date;
}
