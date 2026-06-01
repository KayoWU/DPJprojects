/**
 * @file    Display.h
 * @brief   ECU Abstraction — 7-segment display
 *
 * Drives a common-cathode 7-segment LED display via P0 with
 * P2.6 (段 latch) and P2.7 (位 latch) control.
 *
 * Layer position: EcuAb (hardware-direct, called by Rte).
 * Note: a full AUTOSAR port would move P0/P2.6/P2.7 access into MCAL;
 *        this is kept direct for C51 code-size reasons.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Config.h"
#include "Display_Cfg.h"

/** Module identification (AUTOSAR-lite convention) */
#define DISPLAY_VENDOR_ID                 0u
#define DISPLAY_MODULE_ID                 200u
#define DISPLAY_INSTANCE_ID               0u
#define DISPLAY_SW_MAJOR_VERSION          1u
#define DISPLAY_SW_MINOR_VERSION          0u
#define DISPLAY_SW_PATCH_VERSION          0u

/** API identifiers for Det error reporting */
#define DISPLAY_INIT_API_ID               0x00u
#define DISPLAY_SHOW_DIGIT_API_ID         0x01u
#define DISPLAY_SHOW_NUMBER_API_ID        0x02u

/** Error codes */
#define DISPLAY_E_UNINIT                  0x00u  /**< Called before Display_Init()  */
#define DISPLAY_E_PARAM_DIGIT_POSITION    0x01u  /**< Position out of range         */
#define DISPLAY_E_PARAM_DIGIT_VALUE       0x02u  /**< Value out of segment-table    */

void Display_Init(void);                                  /* 初始化数码管模块 */
void Display_ShowDigit(uint8 digitPosition, uint8 digitValue); /* 显示一位数码管 */
void Display_ShowNumber(uint8 displayValue);               /* 显示三位十进制数(0-255) */

#endif
