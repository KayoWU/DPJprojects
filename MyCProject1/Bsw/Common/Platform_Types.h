/* 平台类型定义：把C51基本类型统一成uint8/uint16等固定宽度名字。 */

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
