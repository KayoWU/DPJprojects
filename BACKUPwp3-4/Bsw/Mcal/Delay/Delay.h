/**
 * @file    Delay.h
 * @brief   MCAL — Software delay (blocking, approximate)
 *
 * Simple busy-loop delay. Accuracy depends on system clock and
 * compiler optimisation level. Suitable for non-critical timing
 * like display refresh.
 *
 * Layer position: Mcal (called by EcuAb/Display).
 */

#ifndef DELAY_H
#define DELAY_H

#include "Config.h"

/** Module identification (AUTOSAR-lite convention) */
#define DELAY_VENDOR_ID                 0u
#define DELAY_MODULE_ID                 202u
#define DELAY_SW_MAJOR_VERSION          1u
#define DELAY_SW_MINOR_VERSION          0u
#define DELAY_SW_PATCH_VERSION          0u

/**
 * @brief 阻塞式毫秒延时（大致值，非精确计时）
 * @param ms  延时的毫秒数
 *
 * 用嵌套空循环实现，内层 115 次 ≈ 1ms（11.0592MHz 晶振）。
 * 适合数码管刷新等不要求精确时间的场景。
 */
void Delay_WaitMs(uint16 ms);

#endif 
