/**
 * @file    EcuM.h
 * @brief   ECU Manager — lightweight startup initialization
 *
 * AUTOSAR-lite EcuM: owns the ECU initialization sequence.
 * EcuM_Init() is called once from main() and delegates to
 * Rte_Init() which initializes all ECU abstraction services.
 */

#ifndef ECUM_H
#define ECUM_H

#include "Config.h"

/** Module identification (AUTOSAR-lite convention) */
#define ECUM_VENDOR_ID                 0u
#define ECUM_MODULE_ID                 10u
#define ECUM_SW_MAJOR_VERSION          1u
#define ECUM_SW_MINOR_VERSION          0u
#define ECUM_SW_PATCH_VERSION          0u

/**
 * @brief ECU 总初始化函数（EcuM_Init）
 *
 * 上电后只调用一次，启动初始化链：
 *   EcuM_Init → Rte_Init → Serial_Init / IoHwAb_Init / Display_Init
 * 最终到达硬件层：Serial_Init → Uart_Init（配置串口寄存器）。
 */
void EcuM_Init(void);

#endif
