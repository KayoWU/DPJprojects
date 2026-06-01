#include "Lcd1602.h"
#include "Delay.h"

#define LCD1602_DATA_PORT P0

sbit Lcd1602_Rs = P3^5;
sbit Lcd1602_En = P3^4;

/* 写LCD1602命令：RS=0表示当前总线上放的是控制命令。 */
void Lcd1602_WriteCommand(uint8 command)
{
    Lcd1602_En = 0;
    Lcd1602_Rs = 0;
    LCD1602_DATA_PORT = command;
    Lcd1602_En = 1;
    Delay_WaitMs(1u);
    Lcd1602_En = 0;
}

/* 写LCD1602数据：RS=1表示当前总线上放的是要显示的字符。 */
void Lcd1602_WriteData(uint8 dataValue)
{
    Lcd1602_En = 0;
    Lcd1602_Rs = 1;
    LCD1602_DATA_PORT = dataValue;
    Lcd1602_En = 1;
    Delay_WaitMs(1u);
    Lcd1602_En = 0;
}

/* LCD1602初始化：配置显示模式、开显示、光标移动方式，并清屏。 */
void Lcd1602_Init(void)
{
    Lcd1602_WriteCommand(0x38u);
    Delay_WaitMs(40u);
    Lcd1602_WriteCommand(0x0cu);
    Delay_WaitMs(40u);
    Lcd1602_WriteCommand(0x06u);
    Delay_WaitMs(40u);
    Lcd1602_WriteCommand(0x01u);
    Delay_WaitMs(40u);
}

/* 在指定位置显示字符串：row为1/2行，column从1开始计数。 */
void Lcd1602_ShowString(uint8 row, uint8 column, uint8 text[])
{
    uint8 i;

    if((row != 1u && row != 2u) || column < 1u || column > 40u)
    {
        return;
    }
    if(row == 1u)
    {
        Lcd1602_WriteCommand(0x80u + column - 1u);
    }
    if(row == 2u)
    {
        Lcd1602_WriteCommand(0x80u + 0x40u + column - 1u);
    }
    for(i = 0u; text[i] != '\0'; i++)
    {
        Lcd1602_WriteData(text[i]);
    }
}

/* 在指定位置显示一个字符：适合只刷新某一位数字。 */
void Lcd1602_ShowChar(uint8 row, uint8 column, uint8 dataValue)
{
    if((row != 1u && row != 2u) || column < 1u || column > 40u)
    {
        return;
    }
    if(row == 1u)
    {
        Lcd1602_WriteCommand(0x80u + column - 1u);
    }
    if(row == 2u)
    {
        Lcd1602_WriteCommand(0x80u + 0x40u + column - 1u);
    }
    Lcd1602_WriteData(dataValue);
}
