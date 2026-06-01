/**
 * @file    SchM.c
 * @brief   Schedule Manager implementation — EA save/restore
 *
 * Saves the global interrupt enable flag (EA) on enter and restores
 * it on exit. This allows the critical section to nest correctly even
 * if called from code that already has interrupts disabled.
 */

#include "SchM.h"

/** 保存进入临界区前的EA（中断总开关状态），用于退出时恢复 */
static boolean SchM_UartRxDataSavedEa;

/* 进入UART接收数据临界区：暂时关总中断，避免读写接收缓存时被打断。 */
void SchM_Enter_Uart_RxData(void)
{
    SchM_UartRxDataSavedEa = EA;   /* 先存当前中断状态 */
    EA = 0;                        /* 再关总中断 */
}

/* 退出UART接收数据临界区：恢复进入前的总中断状态。 */
void SchM_Exit_Uart_RxData(void)
{
    EA = SchM_UartRxDataSavedEa;   /* 恢复之前存的中断状态 */
}
