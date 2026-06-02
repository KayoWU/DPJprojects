/* 数码管抽象接口：通过P0数据总线和P2.6/P2.7锁存信号显示数字。 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Config.h"
#include "Display_Cfg.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define DISPLAY_VENDOR_ID                 0u
#define DISPLAY_MODULE_ID                 200u
#define DISPLAY_INSTANCE_ID               0u
#define DISPLAY_SW_MAJOR_VERSION          1u
#define DISPLAY_SW_MINOR_VERSION          0u
#define DISPLAY_SW_PATCH_VERSION          0u

/* Det上报用的接口编号。 */
#define DISPLAY_INIT_API_ID               0x00u
#define DISPLAY_SHOW_DIGIT_API_ID         0x01u
#define DISPLAY_SHOW_NUMBER_API_ID        0x02u

/* Det上报用的错误码。 */
#define DISPLAY_E_UNINIT                  0x00u  /* 未初始化就调用接口 */
#define DISPLAY_E_PARAM_DIGIT_POSITION    0x01u  /* 数码管位置越界 */
#define DISPLAY_E_PARAM_DIGIT_VALUE       0x02u  /* 段码表取值越界 */

void Display_Init(void);                                  /* 初始化数码管模块 */
void Display_ShowDigit(uint8 digitPosition, uint8 digitValue); /* 显示一位数码管 */
void Display_ShowNumber(uint8 displayValue);               /* 显示三位十进制数(0-255) */

#endif
