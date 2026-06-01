/**
 * @file    Compiler.h
 * @brief   编译器抽象宏（C51 / Keil）
 *
 * 提供 AUTOSAR 风格的内存类别和指针类别抽象。
 *
 * 在完整的 AUTOSAR 中，这些宏会映射到编译器关键字（C51 的 __near、
 * __far、__code、__xdata 等）。在当前轻量版中，memclass/ptrclass
 * 参数被忽略，全部展开为普通 C 代码，保证可移植性。
 *
 * 使用示例（AUTOSAR 约定写法）：
 *   FUNC(void, UART_CODE) Uart_Init(void);        → void Uart_Init(void)
 *   VAR(uint8, UART_VAR)  Uart_Buffer;            → uint8 Uart_Buffer
 *   P2VAR(uint8, AUTOMATIC, UART_APPL_DATA) ptr;  → uint8 *ptr
 *
 * 本项目当前直接使用普通 C 语法，这些宏保留给以后扩展用。
 */

#ifndef COMPILER_H
#define COMPILER_H

/* ---- 函数声明辅助宏 ---- */
#define AUTOMATIC           /* 自动（栈变量） */
#define STATIC        static
#define INLINE              /* 内联展开（Keil C51 不支持） */
#define LOCAL_INLINE  static

#define TYPEDEF

#define FUNC(rettype, memclass)                    rettype
#define FUNC_P2CONST(rettype, ptrclass, memclass)  const rettype *   /* 返回指向常量的指针 */
#define FUNC_P2VAR(rettype, ptrclass, memclass)    rettype *         /* 返回指向变量的指针 */

/* ---- 变量声明辅助宏 ---- */
#define VAR(vartype, memclass)     vartype
#define CONST(consttype, memclass) const consttype

/* ---- 指针声明辅助宏 ---- */
#define P2VAR(ptrtype, memclass, ptrclass)         ptrtype *         /* 指向变量的指针 */
#define P2CONST(ptrtype, memclass, ptrclass)       const ptrtype *   /* 指向常量的指针 */
#define CONSTP2VAR(ptrtype, memclass, ptrclass)    ptrtype * const   /* 指向变量的常指针 */
#define CONSTP2CONST(ptrtype, memclass, ptrclass)  const ptrtype * const /* 指向常量的常指针 */

/* ---- 函数指针辅助宏 ---- */
#define P2FUNC(rettype, ptrclass, fctname)  rettype (*fctname)   /* 函数指针 */

#endif
