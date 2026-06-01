/**
 * @file    App_Command.h
 * @brief   Command processing module — maps UART commands to I/O actions
 *
 * Part of the Application layer. Encapsulates the command protocol:
 *   1 -> port off,    2 -> channel off,
 *   3 -> port+channel recover,   4 -> echo + clear display.
 *
 * Configuration constants (command codes and output levels) live in
 * App_Command_Cfg.h so the logic stays unchanged when values change.
 */

#ifndef APP_COMMAND_H
#define APP_COMMAND_H

#include "Config.h"
#include "App_Command_Cfg.h"

/** Module identification (AUTOSAR-lite convention) */
#define APP_COMMAND_VENDOR_ID                 0u
#define APP_COMMAND_MODULE_ID                 1001u
#define APP_COMMAND_SW_MAJOR_VERSION          1u
#define APP_COMMAND_SW_MINOR_VERSION          0u
#define APP_COMMAND_SW_PATCH_VERSION          0u

/**
 * @brief 处理串口命令字节，返回要显示的数字
 * @param command  串口收到的命令 (1/2/3/4)
 * @return         显示到数码管的值
 *
 * 四个命令的协议：
 *   1 → 整端口输出0x00，显示"1"
 *   2 → 单通道输出0，显示"2"
 *   3 → 恢复端口0xFF+通道1，显示"3"
 *   4 → 回发字节、清接收缓存，显示"0"
 *
 * 会调用的RTE接口：写板级端口、写板级通道、串口发送、串口清除。
 */
uint8 App_CommandProcess(uint8 command);

#endif
