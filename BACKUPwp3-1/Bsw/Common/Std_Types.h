/**
 * @file    Std_Types.h
 * @brief   AUTOSAR 标准类型定义
 *
 * 定义了整个项目通用的：
 *   - Std_ReturnType：函数返回值类型（E_OK=成功, E_NOT_OK=失败）
 *   - Std_VersionInfoType：模块版本信息结构体
 *   - STD_ON / STD_OFF：开关常量
 */

#ifndef STD_TYPES_H
#define STD_TYPES_H

#include "Platform_Types.h"
#include "Compiler.h"

/** 模块API返回类型：E_OK(成功) 或 E_NOT_OK(失败) */
typedef uint8 Std_ReturnType;

/** 模块版本信息（AUTOSAR 规范） */
typedef struct
{
    uint16 vendorID;           /* 厂商ID */
    uint16 moduleID;           /* 模块ID */
    uint8  sw_major_version;   /* 主版本号 */
    uint8  sw_minor_version;   /* 次版本号 */
    uint8  sw_patch_version;   /* 补丁版本号 */
} Std_VersionInfoType;

#define E_OK     0u   /**< 操作成功 */
#define E_NOT_OK 1u   /**< 操作失败 */

#ifndef STD_ON
#define STD_ON  1u    /**< 开启 */
#endif

#ifndef STD_OFF
#define STD_OFF 0u    /**< 关闭 */
#endif

#endif
