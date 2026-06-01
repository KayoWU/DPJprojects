#include "Display.h"
#include "Delay.h"

sbit duan = P2^6;  //控制数码管 段 的芯片锁存器端
sbit wei = P2^7;   //控制数码管 位 的芯片锁存器端

uchar code table[] =        //共阴数码管真值表，显示内容 0—F
{ 
	0x3f,0x06,0x5b,0x4f,
	0x66,0x6d,0x7d,0x07,
	0x7f,0x6f,0x77,0x7c,
	0x39,0x5e,0x79,0x71
};

uchar code index[] =        //共阴数码管位选真值表，依次选中第0-5位的数码管
{
	0x7e,0x7d,0x7b,0x77,    //0011 1111
	0x6f,0x3f
};

//========================================================================
// 函数：void dispaly(uchar num, uchar content)
// 描述：数码管任意显示
// 参数：num: 数码管的哪一位； content: 数码管的显示内容
// 返回：
// 版本：
//========================================================================
void display(uchar num, uchar content)
{
	P0 = 0xff;
	wei = 1;
	P0 = index[num];    
	wei = 0;
	P0 = 0x00;
	duan = 1;
	P0 = table[content];
	duan = 0;			
	delay(1);		
}
