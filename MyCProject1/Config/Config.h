/* 全局基础头：统一引入芯片寄存器、Keil内建函数和通用类型。 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stc89c5xrc.h>       /* 芯片寄存器 sfr/sbit 定义 */
#include <intrins.h>           /* Keil 内建函数 */
#include "Std_Types.h"         /* AUTOSAR 标准类型 */

/* 兼容旧代码的别名；新代码优先使用uint8/uint16。 */
typedef unsigned int uint;
typedef unsigned char uchar;

#endif
