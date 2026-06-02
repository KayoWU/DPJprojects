#include "Swc_IrHandler.h"
#include "Rte.h"

void Swc_IrHandler_Init(void)
{
}

/* 红外业务：收到按键后，同步更新LED和LCD显示请求。 */
void Swc_IrHandler_MainFunction(void)
{
    uint8 keyValue;

    if(Rte_IrSensorIsKeyReady() == TRUE)
    {
        keyValue = Rte_IrSensorGetKey();
        Rte_IrSensorClearKey();  /* 清掉就绪标志，等待下一次红外键。 */
        Rte_Led8Write(keyValue); /* 按键值的8个bit直接映射到8位LED。 */
        Rte_DisplayRequestIrLine(keyValue);
    }
}
