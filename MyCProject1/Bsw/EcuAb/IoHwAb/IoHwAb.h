/* IoHwAb板级I/O抽象：把业务里的板级端口请求映射到底层Dio。 */

#ifndef IOHWAB_H
#define IOHWAB_H

#include "Config.h"
#include "IoHwAb_Cfg.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define IOHWAB_VENDOR_ID                 0u
#define IOHWAB_MODULE_ID                 204u
#define IOHWAB_SW_MAJOR_VERSION          1u
#define IOHWAB_SW_MINOR_VERSION          0u
#define IOHWAB_SW_PATCH_VERSION          0u

#define IOHWAB_INSTANCE_ID               0u

/* Det上报用的接口编号。 */
#define IOHWAB_INIT_API_ID               0x00u
#define IOHWAB_WRITE_BOARD_PORT_API_ID   0x01u
#define IOHWAB_WRITE_BOARD_CHANNEL_API_ID 0x02u

/* Det上报用的错误码。 */
#define IOHWAB_E_UNINIT                  0x00u  /* 未初始化就调用接口 */

void IoHwAb_Init(void);                           /* 初始化I/O抽象模块 */
void IoHwAb_WriteBoardPort(uint8 portLevel);      /* 写整板端口 → Dio_WritePort(P1, x) */
void IoHwAb_WriteBoardChannel(uint8 channelLevel); /* 写单通道 → Dio_WriteChannel(P2.3, x) */

#endif
