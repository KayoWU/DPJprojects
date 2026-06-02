/* SchM临界区接口：保护主循环和中断同时访问的共享数据。 */

#ifndef SCHM_H
#define SCHM_H

#include "Config.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define SCHM_VENDOR_ID                 0u
#define SCHM_MODULE_ID                 130u
#define SCHM_SW_MAJOR_VERSION          1u
#define SCHM_SW_MINOR_VERSION          0u
#define SCHM_SW_PATCH_VERSION          0u

/* 进入UART接收缓存临界区：保存EA并关总中断。 */
void SchM_Enter_Uart_RxData(void);

/* 退出UART接收缓存临界区：恢复进入前的EA状态。 */
void SchM_Exit_Uart_RxData(void);

#endif
