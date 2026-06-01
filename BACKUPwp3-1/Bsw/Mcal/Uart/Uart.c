/**
 * @file    Uart.c
 * @brief   UART MCAL driver implementation
 *
 * Hardware registers used:
 *   SCON — serial control (mode 1: 8-bit UART, variable baud)
 *   PCON — SMOD bit controls baud doubling
 *   TMOD — timer 1 in 8-bit auto-reload mode
 *   TH1/TL1 — timer 1 reload value (sets baud rate)
 *   SBUF — serial data buffer (write = TX, read = RX)
 *   IE — ES (serial interrupt), EA (global interrupt)
 *
 * Interrupt: Uart_Isr() (vector 4) handles both RX (RI) and TX (TI).
 * Shared data: Uart_RxData is guarded by SchM critical sections.
 */

#include "Uart.h"
#include "Uart_Cfg.h"
#include "SchM.h"

/**
 * UART 接收缓冲区
 *
 * 被中断（Uart_Isr）写入，被主循环（Uart_GetReceivedData）读取。
 * volatile 确保编译器每次都真正读内存而不是寄存器缓存。
 * 读写必须用 SchM 临界区保护，防止读到一半被中断打断。
 */
static volatile uint8 Uart_RxData;

/**
 * 串口硬件初始化
 *
 * 配置顺序：
 *   ① SCON → 模式1（8位UART，允许接收）
 *   ② PCON → SMOD=0（波特率不加倍）
 *   ③ ES   → 开串口中断
 *   ④ TMOD → 定时器1设为8位自动重装模式（产生波特率时钟）
 *   ⑤ TH1/TL1 → 根据晶振和波特率计算初值
 *   ⑥ TR1  → 启动定时器1
 *   ⑦ EA   → 开总中断
 */
void Uart_Init(void)
{
    SCON = 0x50;                                   /* 模式1, REN=1(允许接收) */
    PCON = PCON & 0x7f;                            /* SMOD=0, 波特率不加倍 */
    ES   = 1;                                      /* 开串口中断 */
    TMOD = 0x20;                                   /* T1: 8位自动重装 */
    TH1  = TL1 = 256 - (UART_FOSC / 12 / 32 / UART_BAUD); /* 波特率 = 晶振/12/32/(256-TH1) */
    TR1  = 1;                                      /* 启动定时器1 */
    EA   = 1;                                      /* 开总中断 */
}

/**
 * 读取串口收到的字节
 *
 * 整个过程在临界区内完成：关中断 → 读数据 → 恢复中断。
 * 防止读到一半时 ISR 又写入了新数据。
 */
uint8 Uart_GetReceivedData(void)
{
    uint8 rxData;

    SchM_Enter_Uart_RxData();  /* 关中断 */
    rxData = Uart_RxData;      /* 安全读取 */
    SchM_Exit_Uart_RxData();   /* 恢复中断 */

    return rxData;
}

/**
 * 清除接收缓存（写0）
 */
void Uart_ClearReceivedData(void)
{
    SchM_Enter_Uart_RxData();
    Uart_RxData = 0u;
    SchM_Exit_Uart_RxData();
}

/**
 * 发送一个字节
 *
 * 写 SBUF 即触发发送，发送完成后硬件自动置 TI=1。
 * 这里没有等待 TI，是"扔出去就不管"的方式。
 */
void Uart_SendByte(uint8 txData)
{
    SBUF = txData;  /* 写发送缓冲 → 硬件自动开始发送 */
}

/**
 * 串口中断服务程序（中断向量4）
 *
 * 两个中断源共用此向量：
 *   RI=1 → 收到一个字节：清标志，存到 Uart_RxData
 *   TI=1 → 发送完成：    清标志即可
 */
void Uart_Isr() interrupt 4
{
    if(RI == 1)
    {
        RI = 0;                    /* 清接收中断标志 */
        Uart_RxData = SBUF;        /* 从硬件缓冲读走数据 */
    }
    if(TI == 1)
    {
        TI = 0;                    /* 清发送中断标志 */
    }
}
