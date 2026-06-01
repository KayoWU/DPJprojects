/**
 * @file    Det.h
 * @brief   Development Error Tracer — lightweight debug hook
 *
 * AUTOSAR-lite Det: stores the last reported error context so you
 * can inspect it in the Keil debugger watch window.
 *
 * Usage:
 *   (void)Det_ReportError(MODULE_ID, INSTANCE_ID, API_ID, ERROR_CODE);
 *
 * Variables are volatile so the compiler won't optimize them away
 * even when -Ot is used.
 */

#ifndef DET_H
#define DET_H

#include "Std_Types.h"

/** Module identification (AUTOSAR-lite convention) */
#define DET_VENDOR_ID                 0u
#define DET_MODULE_ID                 15u
#define DET_SW_MAJOR_VERSION          1u
#define DET_SW_MINOR_VERSION          0u
#define DET_SW_PATCH_VERSION          0u

/** 错误现场 — 可在 Keil 调试器的 Watch 窗口中观察 */
extern volatile uint16 Det_LastModuleId;    /* 最后出错的模块ID */
extern volatile uint8  Det_LastInstanceId;  /* 最后出错的实例ID */
extern volatile uint8  Det_LastApiId;       /* 最后出错的API函数ID */
extern volatile uint8  Det_LastErrorId;     /* 最后出错的错误码 */
extern volatile uint16 Det_ErrorCount;      /* 累计错误次数 */

/**
 * @brief 报告一个开发期错误（Det_ReportError）
 *
 * 保存错误现场并递增错误计数。
 * 总是返回 E_OK，调用方可以链式写法：(void)Det_ReportError(...);
 *
 * @param moduleId    报告错误的模块ID
 * @param instanceId  模块实例ID（通常为0）
 * @param apiId       出错的API函数ID
 * @param errorId     错误码（各模块自定）
 * @return            始终返回 E_OK
 */
Std_ReturnType Det_ReportError(uint16 moduleId, uint8 instanceId, uint8 apiId, uint8 errorId);

#endif
