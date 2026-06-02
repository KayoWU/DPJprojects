/* UART配置：晶振频率和目标波特率，Uart_Init()用它们计算Timer1重装值。 */

#ifndef UART_CFG_H
#define UART_CFG_H

#define UART_FOSC 11059200L   /* 系统晶振频率，单位Hz */
#define UART_BAUD 9600u       /* 目标串口波特率 */

#endif
