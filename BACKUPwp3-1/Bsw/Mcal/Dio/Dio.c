/**
 * @file    Dio.c
 * @brief   Digital I/O driver implementation
 *
 * Direct register writes to 8051 port registers.
 * Each API validates the port/channel ID before accessing hardware.
 */

#include "Dio.h"
#include "Det.h"

/**
 * 写整个端口（8位）
 *
 * 目前只支持 P1 端口。如果传入不支持的端口号，向 Det 报告错误。
 */
void Dio_WritePort(uint8 portId, uint8 portLevel)
{
    if(portId == DIO_PORT_P1)
    {
        P1 = portLevel;  /* 一次写8位：P1.0~P1.7 */
    }
    else
    {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_WRITE_PORT_API_ID, DIO_E_PARAM_INVALID_PORT_ID);
    }
}

/**
 * 写单个引脚（1位）
 *
 * 目前只支持 P2.3 引脚。如果传入不支持的引脚号，向 Det 报告错误。
 */
void Dio_WriteChannel(uint8 channelId, uint8 channelLevel)
{
    if(channelId == DIO_CHANNEL_P23)
    {
        P23 = channelLevel;  /* 写单个位：P2.3 */
    }
    else
    {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_WRITE_CHANNEL_API_ID, DIO_E_PARAM_INVALID_CHANNEL_ID);
    }
}
