/* UART底层驱动：配置8051串口和Timer1波特率，中断里收发字节。 */

#include "Uart.h"
#include "Uart_Cfg.h"
#include "SchM.h"

/* 接收缓存：中断写入，主循环读取，读写时用SchM保护。 */
static volatile uint8 Uart_RxData;

/* TRUE表示上一个字节还没发完，避免连续写SBUF覆盖数据。 */
static volatile boolean Uart_TxBusy = FALSE;

/* 串口初始化：模式1接收，Timer1自动重装产生9600波特率，开启串口中断。 */
void Uart_Init(void)
{
    SCON = 0x50;                                   /* 模式1, REN=1(允许接收) */
    PCON = PCON & 0x7f;                            /* SMOD=0, 波特率不加倍 */
    ES   = 1;                                      /* 开串口中断 */
    TMOD = 0x20;                                   /* T1: 8位自动重装 */
    TH1  = TL1 = 256 - (UART_FOSC / 12 / 32 / UART_BAUD); /* 波特率 = 晶振/12/32/(256-TH1) */
    Uart_TxBusy = FALSE;
    TR1  = 1;                                      /* 启动定时器1 */
    EA   = 1;                                      /* 开总中断 */
}

/* 读取收到的字节：临界区内取值，避免和串口中断同时访问。 */
uint8 Uart_GetReceivedData(void)
{
    uint8 rxData;

    SchM_Enter_Uart_RxData();  /* 关中断 */
    rxData = Uart_RxData;      /* 安全读取 */
    SchM_Exit_Uart_RxData();   /* 恢复中断 */

    return rxData;
}

/* 清除接收缓存：上层消费完命令后调用。 */
void Uart_ClearReceivedData(void)
{
    SchM_Enter_Uart_RxData();
    Uart_RxData = 0u;
    SchM_Exit_Uart_RxData();
}

/* 发送一个字节：SBUF只能在空闲时写，发送完成由TI中断释放忙标志。 */
void Uart_SendByte(uint8 txData)
{
    if(Uart_TxBusy == FALSE)
    {
        Uart_TxBusy = TRUE;
        SBUF = txData;  /* 写发送缓冲 → 硬件自动开始发送 */
    }
}

/* 查询串口发送缓冲是否可写。 */
boolean Uart_IsTxReady(void)
{
    if(Uart_TxBusy == FALSE)
    {
        return TRUE;
    }
    return FALSE;
}

/* 串口中断：RI表示收到字节，TI表示发送完成。 */
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
        Uart_TxBusy = FALSE;       /* 允许发送下一个字节 */
    }
}
