/**
 * @file    Serial.c
 * @brief   Serial ECU Abstraction implementation
 *
 * Adds init-state guarding on top of MCAL Uart.
 * If any API is called before Serial_Init(), it reports
 * SERIAL_E_UNINIT to Det and returns safely.
 */

#include "Serial.h"
#include "Uart.h"
#include "Det.h"

/** 模块初始化标志 — Serial_Init() 将该标志置 TRUE */
static boolean Serial_IsInitialized = FALSE;

void Serial_Init(void)
{
    Uart_Init();                      /* 配置UART硬件寄存器 */
    Serial_IsInitialized = TRUE;      /* 标记已初始化 */
}

uint8 Serial_GetReceivedData(void)
{
    if(Serial_IsInitialized == FALSE)
    {
        /* 还没初始化就调用 → 记录错误到Det，返回0 */
        (void)Det_ReportError(SERIAL_MODULE_ID, SERIAL_INSTANCE_ID,
                              SERIAL_GET_RECEIVED_DATA_API_ID, SERIAL_E_UNINIT);
        return 0u;
    }
    return Uart_GetReceivedData();    /* 委托给MCAL层读取 */
}

void Serial_ClearReceivedData(void)
{
    if(Serial_IsInitialized == FALSE)
    {
        (void)Det_ReportError(SERIAL_MODULE_ID, SERIAL_INSTANCE_ID,
                              SERIAL_CLEAR_RECEIVED_DATA_API_ID, SERIAL_E_UNINIT);
        return;
    }
    Uart_ClearReceivedData();         /* 委托给MCAL层清除 */
}

void Serial_SendByte(uint8 txData)
{
    if(Serial_IsInitialized == FALSE)
    {
        (void)Det_ReportError(SERIAL_MODULE_ID, SERIAL_INSTANCE_ID,
                              SERIAL_SEND_BYTE_API_ID, SERIAL_E_UNINIT);
        return;
    }
    Uart_SendByte(txData);            /* 委托给MCAL层发送 */
}
