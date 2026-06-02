/* 阻塞延时接口：只适合初始化或简单等待，运行期应尽量少用。 */

#ifndef DELAY_H
#define DELAY_H

#include "Config.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define DELAY_VENDOR_ID                 0u
#define DELAY_MODULE_ID                 202u
#define DELAY_SW_MAJOR_VERSION          1u
#define DELAY_SW_MINOR_VERSION          0u
#define DELAY_SW_PATCH_VERSION          0u

/* 阻塞式毫秒延时：空循环实现，时间只是近似值。 */
void Delay_WaitMs(uint16 ms);

#endif 
