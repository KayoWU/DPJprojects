/**
 * @file    Uart.h
 * @brief   MCAL — UART driver (STC89C52RC on-chip UART)
 *
 * Bottom-level hardware driver. Configures the 8051 UART in mode 1
 * (8-bit, timer-1 baud). Provides interrupt-safe access to the RX
 * buffer via SchM critical sections.
 *
 * Layer position: Mcal (direct register access, called by EcuAb/Serial).
 */

#ifndef UART_H
#define UART_H

#include "Config.h"

/** Module identification (AUTOSAR-lite convention) */
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
