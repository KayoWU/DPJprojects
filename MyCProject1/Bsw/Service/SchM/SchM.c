/* SchM实现：进入临界区时保存EA，退出时恢复EA。 */

#include "SchM.h"

/* 保存进入临界区前的EA，避免退出时误开中断。 */
static boolean SchM_UartRxDataSavedEa;

/* 进入UART接收数据临界区：暂时关总中断，避免读写接收缓存时被打断。 */
void SchM_Enter_Uart_RxData(void)
{
    SchM_UartRxDataSavedEa = EA;   /* 先存当前中断状态 */
    EA = 0;                        /* 再关总中断 */
}

/* 退出UART接收数据临界区：恢复进入前的总中断状态。 */
void SchM_Exit_Uart_RxData(void)
{
    EA = SchM_UartRxDataSavedEa;   /* 恢复之前存的中断状态 */
}
