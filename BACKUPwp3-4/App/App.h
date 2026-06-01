/**
 * @file    App.h
 * @brief   Application layer — cyclic main function declaration
 *
 * Part of the AUTOSAR-lite Application layer.
 * Owns the application-level cyclic logic entry point.
 */

#ifndef APP_H
#define APP_H

#include "Config.h"

/** Module identification (AUTOSAR-lite convention) */
#define APP_VENDOR_ID                 0u
#define APP_MODULE_ID                 1000u
#define APP_SW_MAJOR_VERSION          1u
#define APP_SW_MINOR_VERSION          0u
#define APP_SW_PATCH_VERSION          0u

/**
 * @brief 应用层主循环函数（App_MainFunction）
 *
 * 被 main() 的死循环每秒多次调用。
 * 流程：读串口数据 → 处理命令 → 刷新数码管显示。
 */
void App_MainFunction(void);

#endif
