/* Serial抽象层接口：上层通过这里收发字节，不直接接触Uart寄存器。 */

#ifndef SERIAL_H
#define SERIAL_H

#include "Config.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define SERIAL_VENDOR_ID                 0u
#define SERIAL_MODULE_ID                 203u
#define SERIAL_SW_MAJOR_VERSION          1u
#define SERIAL_SW_MINOR_VERSION          0u
#define SERIAL_SW_PATCH_VERSION          0u

#define SERIAL_INSTANCE_ID               0u

/* Det上报用的接口编号。 */
#define SERIAL_INIT_API_ID               0x01u
#define SERIAL_GET_RECEIVED_DATA_API_ID  0x02u
#define SERIAL_CLEAR_RECEIVED_DATA_API_ID 0x03u
#define SERIAL_SEND_BYTE_API_ID          0x04u

/* Det上报用的错误码。 */
#define SERIAL_E_UNINIT                  0x01u  /* 未初始化就调用接口 */

void   Serial_Init(void);               /* 初始化串口 → 调 Uart_Init() 配置硬件寄存器 */
uint8  Serial_GetReceivedData(void);    /* 读取接收到的串口字节 */
void   Serial_ClearReceivedData(void);  /* 清除接收缓存 */
void   Serial_SendByte(uint8 txData);   /* 发送一个字节 */
boolean Serial_IsTxReady(void);         /* 查询串口发送是否空闲 */

#endif
