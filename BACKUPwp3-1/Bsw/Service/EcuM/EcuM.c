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

/**
 * 唯一的初始化入口，只需调 Rte_Init() 即可完成全部底层初始化。
 */
void EcuM_Init(void)
{
    Rte_Init();  /* 级联初始化：Serial→IoHwAb→Display */
}
