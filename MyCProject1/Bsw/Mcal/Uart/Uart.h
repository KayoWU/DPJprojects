/* UART MCAL接口：只在驱动内部访问SBUF/SCON等寄存器。 */

#ifndef UART_H
#define UART_H

#include "Config.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define UART_VENDOR_ID                 0u
#define UART_MODULE_ID                 201u
#define UART_SW_MAJOR_VERSION          1u
#define UART_SW_MINOR_VERSION          0u
#define UART_SW_PATCH_VERSION          0u

void   Uart_Init(void);               /* 配置串口硬件（波特率/模式/中断） */
uint8  Uart_GetReceivedData(void);    /* 读取中断收到的字节（临界区保护） */
void   Uart_ClearReceivedData(void);  /* 清除接收缓存 */
void   Uart_SendByte(uint8 txData);   /* 发送一个字节（写 SBUF） */
boolean Uart_IsTxReady(void);         /* 查询发送缓冲是否空闲 */

#endif
