/**
 * @file    IoHwAb.c
 * @brief   I/O Hardware Abstraction implementation
 *
 * Maps board-level port/channel writes to MCAL Dio.
 * Init-state checking ensures hardware is only accessed after
 * IoHwAb_Init() has been called.
 */

#include "IoHwAb.h"
#include "Dio.h"
#include "Det.h"

/** 模块初始化标志 */
static boolean IoHwAb_IsInitialized = FALSE;

void IoHwAb_Init(void)
{
    IoHwAb_IsInitialized = TRUE;  /* I/O抽象模块无需硬件配置，只置标志 */
}

/**
 * 写板级整端口：通过配置映射到 MCU 的 P1 端口。
 * 未初始化时向 Det 报告错误并安全返回。
 */
void IoHwAb_WriteBoardPort(uint8 portLevel)
{
    if(IoHwAb_IsInitialized == FALSE)
    {
        (void)Det_ReportError(IOHWAB_MODULE_ID, IOHWAB_INSTANCE_ID,
                              IOHWAB_WRITE_BOARD_PORT_API_ID, IOHWAB_E_UNINIT);
        return;
    }
    Dio_WritePort(IOHWAB_BOARD_PORT, portLevel);  /* 写P1全部8位 */
}

/**
 * 写板级单通道：通过配置映射到 MCU 的 P2.3 引脚。
 */
void IoHwAb_WriteBoardChannel(uint8 channelLevel)
{
    if(IoHwAb_IsInitialized == FALSE)
    {
        (void)Det_ReportError(IOHWAB_MODULE_ID, IOHWAB_INSTANCE_ID,
                              IOHWAB_WRITE_BOARD_CHANNEL_API_ID, IOHWAB_E_UNINIT);
        return;
    }
    Dio_WriteChannel(IOHWAB_BOARD_CHANNEL, channelLevel);  /* 写P2.3单个位 */
}
