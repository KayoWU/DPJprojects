/**
 * @file    Det.c
 * @brief   Development Error Tracer implementation
 *
 * Stores the last-reported error identity and an error counter.
 * No runtime action taken — purely for debugger inspection.
 */

#include "Det.h"

volatile uint16 Det_LastModuleId;
volatile uint8  Det_LastInstanceId;
volatile uint8  Det_LastApiId;
volatile uint8  Det_LastErrorId;
volatile uint16 Det_ErrorCount;

/**
 * 报告错误：记录是谁(moduleId/instanceId)、在哪里(apiId)、出了什么错(errorId)。
 * 错误计数+1，便于在调试器中看哪个模块老是报错。
 */
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
