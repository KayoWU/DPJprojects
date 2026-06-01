/**
 * @file    SchM.h
 * @brief   Schedule Manager — lightweight critical-section guards
 *
 * AUTOSAR-lite SchM: provides enter/exit pairs for shared data
 * that is accessed from both main-loop code and ISRs.
 *
 * Currently guards:
 *   - Uart_RxData (shared between Uart_Isr and Uart_GetReceivedData)
 *
 * Implementation saves/restores EA to nest safely.
 */

#ifndef SCHM_H
#define SCHM_H

#include "Config.h"

/** Module identification (AUTOSAR-lite convention) */
#define SCHM_VENDOR_ID                 0u
#define SCHM_MODULE_ID                 130u
#define SCHM_SW_MAJOR_VERSION          1u
#define SCHM_SW_MINOR_VERSION          0u
#define SCHM_SW_PATCH_VERSION          0u

/**
 * @brief 进入UART接收数据临界区（保存EA并关总中断）
 *
 * 保护 Uart_RxData 变量，因为它被主循环和中断同时访问。
 */
void SchM_Enter_Uart_RxData(void);

/**
 * @brief 退出UART接收数据临界区（恢复之前保存的EA）
 *
 * 注意：不是简单地 EA=1，而是恢复进入之前的状态，防止嵌套问题。
 */
void SchM_Exit_Uart_RxData(void);

#endif
