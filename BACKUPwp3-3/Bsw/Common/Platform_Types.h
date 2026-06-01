/**
 * @file    Platform_Types.h
 * @brief   平台相关类型定义（C51 / Keil 编译器）
 *
 * 按 AUTOSAR 标准把 C 基本类型映射为固定位宽的类型名：
 *   char = 8位,  int = 16位,  long = 32位。
 *
 * boolean 用 unsigned char，在 8051 上占内存最小。
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

typedef unsigned char boolean;   /* 布尔类型（占 1 字节） */

/* 固定位宽有符号类型 */
typedef signed   char  sint8;    /*  8 位有符号 */
typedef signed   int   sint16;   /* 16 位有符号 */
typedef signed   long  sint32;   /* 32 位有符号 */

/* 固定位宽无符号类型 */
typedef unsigned char  uint8;    /*  8 位无符号 */
typedef unsigned int   uint16;   /* 16 位无符号 */
typedef unsigned long  uint32;   /* 32 位无符号 */

#ifndef TRUE
#define TRUE  1u  /* 真 */
#endif

#ifndef FALSE
#define FALSE 0u  /* 假 */
#endif

#endif
