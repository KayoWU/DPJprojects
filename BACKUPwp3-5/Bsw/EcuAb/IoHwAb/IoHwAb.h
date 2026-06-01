/**
 * @file    IoHwAb.h
 * @brief   ECU Abstraction — I/O Hardware Abstraction
 *
 * Maps board-level I/O requests to MCAL Dio ports/channels.
 * The mapping is configured in IoHwAb_Cfg.h.
 *
 * Layer position: EcuAb (calls Dio in Mcal, called by Rte).
 */

#ifndef IOHWAB_H
#define IOHWAB_H

#include "Config.h"
#include "IoHwAb_Cfg.h"

/** Module identification (AUTOSAR-lite convention) */
#define IOHWAB_VENDOR_ID                 0u
#define IOHWAB_MODULE_ID                 204u
#define IOHWAB_SW_MAJOR_VERSION          1u
#define IOHWAB_SW_MINOR_VERSION          0u
#define IOHWAB_SW_PATCH_VERSION          0u

#define IOHWAB_INSTANCE_ID               0u

/** API identifiers for Det error reporting */
#define IOHWAB_INIT_API_ID               0x00u
#define IOHWAB_WRITE_BOARD_PORT_API_ID   0x01u
#define IOHWAB_WRITE_BOARD_CHANNEL_API_ID 0x02u

/** Error codes */
#define IOHWAB_E_UNINIT                  0x00u  /**< Called before IoHwAb_Init() */

void IoHwAb_Init(void);                           /* 初始化I/O抽象模块 */
void IoHwAb_WriteBoardPort(uint8 portLevel);      /* 写整板端口 → Dio_WritePort(P1, x) */
void IoHwAb_WriteBoardChannel(uint8 channelLevel); /* 写单通道 → Dio_WriteChannel(P2.3, x) */

#endif
