/* Dio底层接口：统一写端口和单个引脚，非法ID会报到Det。 */

#ifndef DIO_H
#define DIO_H

#include "Config.h"
#include "Dio_Cfg.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define DIO_VENDOR_ID                 0u
#define DIO_MODULE_ID                 120u
#define DIO_INSTANCE_ID               0u
#define DIO_SW_MAJOR_VERSION          1u
#define DIO_SW_MINOR_VERSION          0u
#define DIO_SW_PATCH_VERSION          0u

/* Det上报用的接口编号。 */
#define DIO_WRITE_CHANNEL_API_ID      0x01u
#define DIO_WRITE_PORT_API_ID         0x03u

/* Det上报用的错误码。 */
#define DIO_E_PARAM_INVALID_CHANNEL_ID  0x0Au  /* 不支持的通道ID */
#define DIO_E_PARAM_INVALID_PORT_ID     0x14u  /* 不支持的端口ID */

void Dio_WritePort(uint8 portId, uint8 portLevel);       /* 写整个端口（如 P1=0x00） */
void Dio_WriteChannel(uint8 channelId, uint8 channelLevel); /* 写单个引脚（如 P2.3/P2.5） */

#endif
