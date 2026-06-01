/**
 * @file    EcuM.c
 * @brief   ECU Manager implementation
 *
 * Calls Rte_Init() which cascades through:
 *   Serial_Init() -> Uart_Init()  (MCAL: configures UART registers)
 *   IoHwAb_Init()                 (marks initialized)
 *   Display_Init()                (marks initialized)
 */

#include "EcuM.h"
#include "Rte.h"

/* 系统启动管理：当前只负责触发RTE初始化，后续模块都从这里串起来。 */
void EcuM_Init(void)
{
    Rte_Init();  /* 级联初始化：Serial→IoHwAb→Display */
}
