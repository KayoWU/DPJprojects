#ifndef IRSENSOR_H
#define IRSENSOR_H

#include "Config.h"
#include "IrSensor_Cfg.h"

#define IRSENSOR_VENDOR_ID                 0u
#define IRSENSOR_MODULE_ID                 211u
#define IRSENSOR_SW_MAJOR_VERSION          1u
#define IRSENSOR_SW_MINOR_VERSION          0u
#define IRSENSOR_SW_PATCH_VERSION          0u

void IrSensor_Init(void);           /* 初始化INT1和Timer0，准备接收红外波形 */
void IrSensor_MainFunction(void);   /* 周期解码pulse缓冲区，生成按键码 */
boolean IrSensor_IsKeyReady(void);  /* 是否已有一个校验正确的按键码 */
uint8 IrSensor_GetKey(void);        /* 读取最近一次按键码 */
void IrSensor_ClearKey(void);       /* App处理完按键后清标志 */

#endif
