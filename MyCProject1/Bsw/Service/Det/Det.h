/* Det轻量错误记录：保存最后一次错误现场，方便在Keil Watch窗口观察。 */

#ifndef DET_H
#define DET_H

#include "Std_Types.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define DET_VENDOR_ID                 0u
#define DET_MODULE_ID                 15u
#define DET_SW_MAJOR_VERSION          1u
#define DET_SW_MINOR_VERSION          0u
#define DET_SW_PATCH_VERSION          0u

/* 错误现场：volatile避免被优化，调试时可直接观察。 */
extern volatile uint16 Det_LastModuleId;    /* 最后出错的模块ID */
extern volatile uint8  Det_LastInstanceId;  /* 最后出错的实例ID */
extern volatile uint8  Det_LastApiId;       /* 最后出错的API函数ID */
extern volatile uint8  Det_LastErrorId;     /* 最后出错的错误码 */
extern volatile uint16 Det_ErrorCount;      /* 累计错误次数 */

/* 报告开发期错误：只记录信息，不打断程序运行。 */
Std_ReturnType Det_ReportError(uint16 moduleId, uint8 instanceId, uint8 apiId, uint8 errorId);

#endif
