/* Det实现：记录最后一次错误来源和累计错误次数。 */

#include "Det.h"

volatile uint16 Det_LastModuleId;
volatile uint8  Det_LastInstanceId;
volatile uint8  Det_LastApiId;
volatile uint8  Det_LastErrorId;
volatile uint16 Det_ErrorCount;

/* 轻量错误记录：不打断程序，只保存最后一次错误，方便调试观察。 */
Std_ReturnType Det_ReportError(uint16 moduleId, uint8 instanceId, uint8 apiId, uint8 errorId)
{
    Det_LastModuleId   = moduleId;    /* 哪个模块 */
    Det_LastInstanceId = instanceId;  /* 哪个实例 */
    Det_LastApiId      = apiId;       /* 哪个函数 */
    Det_LastErrorId    = errorId;     /* 什么错误 */
    Det_ErrorCount++;                 /* 错误次数+1 */

    return E_OK;
}
