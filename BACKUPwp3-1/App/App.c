/**
 * @file    App.c
 * @brief   Application cyclic logic — the "main runnable"
 *
 * In AUTOSAR terms this is the single cyclic application runnable.
 * It reads UART data via RTE, delegates command handling, and
 * writes the display result back via RTE.
 */

#include "App.h"
#include "Rte.h"
#include "App_Command.h"

/**
 * 应用层唯一周期性任务，每次执行三步：
 * ① 通过RTE读取串口收到的命令字节
 * ② 调用命令处理模块，得到要显示的数字
 * ③ 通过RTE刷新数码管
 */
void App_MainFunction(void)
{
    uint8 rxData;
    uint8 displayValue;

    rxData = Rte_ReadSerialData();               /* ① 读串口数据 */
    displayValue = App_CommandProcess(rxData);   /* ② 处理命令 */
    Rte_WriteDisplayNumber(displayValue);        /* ③ 刷新显示 */
}
