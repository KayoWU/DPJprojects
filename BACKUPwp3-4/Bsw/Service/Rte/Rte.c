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
#include "Adc.h"
#include "Lcd1602.h"
#include "At24c02.h"
#include "Timer2.h"
#include "Iic.h"

/* 初始化RTE背后的全部服务，让App不用关心底层初始化顺序。 */
void Rte_Init(void)
{
    Serial_Init();   /* 串口初始化 → Uart_Init */
    Adc_Init();      /* ADC0804风格并口ADC初始化 */
    Lcd1602_Init();  /* LCD1602初始化 */
    At24c02_Init();  /* IIC EEPROM初始化 */
    Timer2_Init();   /* Timer2周期标志初始化 */
}

/* RTE周期调度：推进所有非阻塞状态机。 */
void Rte_MainFunction(void)
{
    uint16 tick;

    tick = Timer2_GetTick();
    Iic_MainFunction();
    At24c02_MainFunction(tick);
    Lcd1602_MainFunction(tick);
}

/* 获取1ms系统节拍，给App做周期判断。 */
uint16 Rte_GetSystemTick(void)
{
    return Timer2_GetTick();
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

/* 查询串口是否可以发送下一个字节。 */
boolean Rte_IsSerialTxReady(void)
{
    return Serial_IsTxReady();
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

/* 异步请求LCD显示字符串：运行阶段推荐使用，不阻塞主循环。 */
Std_ReturnType Rte_Lcd1602AsyncShowString(uint8 row, uint8 column, uint8 text[])
{
    return Lcd1602_AsyncShowString(row, column, text);
}

/* 查询LCD状态机是否还在发送。 */
boolean Rte_Lcd1602IsBusy(void)
{
    return Lcd1602_IsBusy();
}

/* 异步写AT24C02：只发起任务，实际IIC动作由状态机完成。 */
Std_ReturnType Rte_At24c02AsyncWrite(uint8 address, uint8 dataValue[], uint8 length)
{
    return At24c02_AsyncWrite(address, dataValue, length);
}

/* 异步读AT24C02：只发起任务，完成后用GetData取结果。 */
Std_ReturnType Rte_At24c02AsyncRead(uint8 address, uint8 length)
{
    return At24c02_AsyncRead(address, length);
}

boolean Rte_At24c02IsBusy(void)
{
    return At24c02_IsBusy();
}

boolean Rte_At24c02IsJobDone(void)
{
    return At24c02_IsJobDone();
}

uint8 Rte_At24c02GetData(uint8 index)
{
    return At24c02_GetData(index);
}
