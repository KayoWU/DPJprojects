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
#include "App_FeatureDemo.h"

/* 应用层周期任务：读串口命令、处理命令、运行整合演示、刷新数码管。 */
void App_MainFunction(void)
{
    uint8 rxData;
    uint8 displayValue;

    rxData = Rte_ReadSerialData();               /* ① 读串口数据 */
    displayValue = App_CommandProcess(rxData);   /* ② 处理命令 */
    App_FeatureDemo_MainFunction();              /* ③ 运行整合功能演示 */
    Rte_WriteDisplayNumber(displayValue);        /* ④ 刷新显示 */
}
