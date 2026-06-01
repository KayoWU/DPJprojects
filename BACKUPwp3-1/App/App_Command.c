/**
 * @file    App_Command.c
 * @brief   Command protocol implementation
 *
 * Maps the 4 UART commands to board I/O actions through RTE.
 * The command code doubles as the default display value —
 * command 4 overrides this to clear (0).
 */

#include "App_Command.h"
#include "Rte.h"

uint8 App_CommandProcess(uint8 command)
{
    uint8 displayValue;

    displayValue = command;  /* 默认显示值 = 命令本身(1→"1", 2→"2", 3→"3") */

    if(command == APP_COMMAND_PORT_OFF)
    {
        /** 命令"1"：整板端口全拉低 */
        Rte_WriteBoardPort(APP_COMMAND_PORT_OFF_LEVEL);
    }
    if(command == APP_COMMAND_CHANNEL_OFF)
    {
        /** 命令"2"：单个通道拉低 */
        Rte_WriteBoardChannel(APP_COMMAND_CHANNEL_OFF_LEVEL);
    }
    if(command == APP_COMMAND_OUTPUT_RECOVER)
    {
        /** 命令"3"：恢复端口为0xFF，通道为1 */
        Rte_WriteBoardPort(APP_COMMAND_PORT_RECOVER_LEVEL);
        Rte_WriteBoardChannel(APP_COMMAND_CHANNEL_RECOVER_LEVEL);
    }
    if(command == APP_COMMAND_SERIAL_ECHO)
    {
        /** 命令"4"：回发收到的字节，清空接收缓存，显示"0" */
        Rte_SendSerialByte(command);
        Rte_ClearSerialData();
        displayValue = APP_COMMAND_DISPLAY_CLEAR_VALUE;  /* 显示 000 */
    }

    return displayValue;
}
