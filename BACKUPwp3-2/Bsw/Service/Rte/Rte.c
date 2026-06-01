/**
 * @file    Rte.c
 * @brief   RTE implementation — thin wrappers over EcuAb services
 *
 * Each RTE API is a one-liner that delegates to the corresponding
 * EcuAb module. This indirection is the key AUTOSAR pattern:
 * if the hardware changes, only Rte.c needs updating — App code is
 * untouched.
 */

#include "Rte.h"
#include "Serial.h"
#include "IoHwAb.h"
#include "Display.h"
#include "Adc.h"
#include "Lcd1602.h"
#include "Iap.h"
#include "At24c02.h"
#include "Timer2.h"

/* 初始化RTE背后的全部服务，让App不用关心底层初始化顺序。 */
void Rte_Init(void)
{
    Serial_Init();   /* 串口初始化 → Uart_Init */
    IoHwAb_Init();   /* I/O抽象初始化 */
    Display_Init();  /* 数码管模块初始化 */
    Adc_Init();      /* ADC0804风格并口ADC初始化 */
    Lcd1602_Init();  /* LCD1602初始化 */
    At24c02_Init();  /* IIC EEPROM初始化 */
    Timer2_Init();   /* Timer2周期标志初始化 */
}

/* 读取串口接收缓存：App只通过RTE拿数据，不直接碰UART寄存器。 */
uint8 Rte_ReadSerialData(void)
{
    return Serial_GetReceivedData();  /* 读一个字节 */
}

/* 清空串口接收缓存：常用于命令处理完成后避免重复执行。 */
void Rte_ClearSerialData(void)
{
    Serial_ClearReceivedData();       /* 清接收缓存 */
}

/* 发送一个串口字节：RTE向下转给Serial，再由Uart写硬件。 */
void Rte_SendSerialByte(uint8 txData)
{
    Serial_SendByte(txData);          /* 发一个字节 */
}

/* 写整板输出端口：当前对应板上P1整口输出。 */
void Rte_WriteBoardPort(uint8 portLevel)
{
    IoHwAb_WriteBoardPort(portLevel);       /* 写整板端口(P1) */
}

/* 写单个板级通道：当前对应P2.3这个输出点。 */
void Rte_WriteBoardChannel(uint8 channelLevel)
{
    IoHwAb_WriteBoardChannel(channelLevel); /* 写单通道(P2.3) */
}

/* 刷新三位数码管：App给数字，Display负责拆位和位选。 */
void Rte_WriteDisplayNumber(uint8 displayValue)
{
    Display_ShowNumber(displayValue);       /* 显示三位数字 */
}

/* 读取ADC原始值：返回0到255的8位采样值。 */
uint8 Rte_ReadAdcValue(void)
{
    return Adc_ReadValue();
}

/* ADC换算：把8位采样值按配置参考电压换算成毫伏。 */
uint16 Rte_AdcConvertToMv(uint8 adcValue)
{
    return Adc_ConvertToMv(adcValue);
}

/* LCD显示字符串：row/column用人更容易理解的1起始坐标。 */
void Rte_Lcd1602ShowString(uint8 row, uint8 column, uint8 text[])
{
    Lcd1602_ShowString(row, column, text);
}

/* LCD显示单个字符：用于频繁刷新某一位数字，避免整行重写。 */
void Rte_Lcd1602ShowChar(uint8 row, uint8 column, uint8 dataValue)
{
    Lcd1602_ShowChar(row, column, dataValue);
}

/* 读单片机内部EEPROM/IAP数据：address是内部数据Flash地址。 */
uint8 Rte_IapReadByte(uint16 address)
{
    return Iap_ReadByte(address);
}

/* 写内部EEPROM/IAP一个字节：写之前通常需要先擦除所在扇区。 */
void Rte_IapProgramByte(uint16 address, uint8 dataValue)
{
    Iap_ProgramByte(address, dataValue);
}

/* 擦除内部EEPROM/IAP扇区：STC写数据前需要先擦除。 */
void Rte_IapEraseSector(uint16 address)
{
    Iap_EraseSector(address);
}

/* 读外部AT24C02一个字节：底层通过软件IIC完成。 */
uint8 Rte_At24c02ReadByte(uint8 address)
{
    return At24c02_ReadByte(address);
}

/* 写外部AT24C02一个字节：App不用关心IIC起停和应答。 */
void Rte_At24c02WriteByte(uint8 address, uint8 dataValue)
{
    At24c02_WriteByte(address, dataValue);
}

/* 查询Timer2节拍标志：只告诉App“时间到了没有”。 */
boolean Rte_Timer2GetElapsedFlag(void)
{
    return Timer2_GetElapsedFlag();
}

/* 清除Timer2节拍标志：App处理完一次周期事件后调用。 */
void Rte_Timer2ClearElapsedFlag(void)
{
    Timer2_ClearElapsedFlag();
}
