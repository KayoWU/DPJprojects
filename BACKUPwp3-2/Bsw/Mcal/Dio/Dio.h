/**
 * @file    Dio.h
 * @brief   MCAL — Digital I/O driver
 *
 * Provides port-level and channel-level write access.
 * Validates port/channel IDs and reports invalid parameters to Det.
 *
 * Currently supported:
 *   Ports:    P1 (DIO_PORT_P1)
 *   Channels: P2.3 (DIO_CHANNEL_P23)
 *
 * Layer position: Mcal (direct register access, called by EcuAb/IoHwAb).
 */

#ifndef DIO_H
#define DIO_H

#include "Config.h"
#include "Dio_Cfg.h"

/** Module identification (AUTOSAR-lite convention) */
#define DIO_VENDOR_ID                 0u
#define DIO_MODULE_ID                 120u
#define DIO_INSTANCE_ID               0u
#define DIO_SW_MAJOR_VERSION          1u
#define DIO_SW_MINOR_VERSION          0u
#define DIO_SW_PATCH_VERSION          0u

/** API identifiers for Det error reporting */
#define DIO_WRITE_CHANNEL_API_ID      0x01u
#define DIO_WRITE_PORT_API_ID         0x03u

/** Error codes */
#define DIO_E_PARAM_INVALID_CHANNEL_ID  0x0Au  /**< Unsupported channel ID  */
#define DIO_E_PARAM_INVALID_PORT_ID     0x14u  /**< Unsupported port ID     */

void Dio_WritePort(uint8 portId, uint8 portLevel);       /* 写整个端口（如 P1=0x00） */
void Dio_WriteChannel(uint8 channelId, uint8 channelLevel); /* 写单个引脚（如 P2.3=1） */

#endif
