/**
 * @file    Config.h
 * @brief   项目全局兼容头文件
 *
 * 作用：每个模块都包含这个头文件，它统一引入了：
 *   ① STC89C52RC 的寄存器定义 (P0, SBUF, SCON ...)
 *   ② 内建函数 (nop, 等等)
 *   ③ AUTOSAR-lite 标准类型 (uint8, Std_ReturnType ...)
 *
 * 所有模块都 #include "Config.h" 作为基础头文件。
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stc89c5xrc.h>       /* 芯片寄存器 sfr/sbit 定义 */
#include <intrins.h>           /* Keil 内建函数 */
#include "Std_Types.h"         /* AUTOSAR 标准类型 */

/** 兼容旧代码的别名 — 新代码建议直接用 uint8 / uint16 */
typedef unsigned int uint;
typedef unsigned char uchar;

#endif
