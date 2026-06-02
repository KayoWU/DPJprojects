/* 通用标准类型：返回值、版本信息和开关宏。 */

#ifndef STD_TYPES_H
#define STD_TYPES_H

#include "Platform_Types.h"
#include "Compiler.h"

/* 模块API返回类型：E_OK表示成功，E_NOT_OK表示失败。 */
typedef uint8 Std_ReturnType;

/* 模块版本信息。 */
typedef struct
{
    uint16 vendorID;           /* 厂商ID */
    uint16 moduleID;           /* 模块ID */
    uint8  sw_major_version;   /* 主版本号 */
    uint8  sw_minor_version;   /* 次版本号 */
    uint8  sw_patch_version;   /* 补丁版本号 */
} Std_VersionInfoType;

#define E_OK     0u   /* 操作成功 */
#define E_NOT_OK 1u   /* 操作失败 */

#ifndef STD_ON
#define STD_ON  1u    /* 开启 */
#endif

#ifndef STD_OFF
#define STD_OFF 0u    /* 关闭 */
#endif

#endif
