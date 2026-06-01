/**
 * @file    main.c
 * @brief   Application entry point
 *
 * AUTOSAR-lite architecture entry:
 *   EcuM_Init() -> Rte_Init() -> Serial/IoHwAb/Display init
 *   App_MainFunction() runs cyclically in superloop.
 *
 * Hardware: STC89C52RC board with UART, 7-seg display, digital I/O.
 */

#include "EcuM.h"
#include "App.h"

/* 程序入口：先完成系统初始化，再进入永远循环的应用调度。 */
int main(void)
{
    EcuM_Init();          /* 上电后一次性初始化所有硬件和BSW模块 */
    while(1)
    {
        App_MainFunction(); /* 死循环：不断读串口→处理命令→刷新显示 */
    }
}
