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

/* ---- 初始化 ---- */
void Rte_Init(void)
{
    Serial_Init();   /* 串口初始化 → Uart_Init */
    IoHwAb_Init();   /* I/O抽象初始化 */
    Display_Init();  /* 数码管模块初始化 */
}

/* ---- 串口服务（全部委托给 EcuAb/Serial） ---- */
uint8 Rte_ReadSerialData(void)
{
    return Serial_GetReceivedData();  /* 读一个字节 */
}

void Rte_ClearSerialData(void)
{
    Serial_ClearReceivedData();       /* 清接收缓存 */
}

void Rte_SendSerialByte(uint8 txData)
{
    Serial_SendByte(txData);          /* 发一个字节 */
}

/* ---- I/O 服务（全部委托给 EcuAb/IoHwAb） ---- */
void Rte_WriteBoardPort(uint8 portLevel)
{
    IoHwAb_WriteBoardPort(portLevel);       /* 写整板端口(P1) */
}

void Rte_WriteBoardChannel(uint8 channelLevel)
{
    IoHwAb_WriteBoardChannel(channelLevel); /* 写单通道(P2.3) */
}

/* ---- 显示服务（委托给 EcuAb/Display） ---- */
void Rte_WriteDisplayNumber(uint8 displayValue)
{
    Display_ShowNumber(displayValue);       /* 显示三位数字 */
}
