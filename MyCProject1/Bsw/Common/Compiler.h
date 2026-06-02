/* 编译器抽象宏：当前展开为普通C写法，保留给后续AUTOSAR风格扩展。 */

#ifndef COMPILER_H
#define COMPILER_H

/* 函数声明辅助宏。 */
#define AUTOMATIC           /* 自动（栈变量） */
#define STATIC        static
#define INLINE              /* 内联展开（Keil C51 不支持） */
#define LOCAL_INLINE  static

#define TYPEDEF

#define FUNC(rettype, memclass)                    rettype
#define FUNC_P2CONST(rettype, ptrclass, memclass)  const rettype *   /* 返回指向常量的指针 */
#define FUNC_P2VAR(rettype, ptrclass, memclass)    rettype *         /* 返回指向变量的指针 */

/* 变量声明辅助宏。 */
#define VAR(vartype, memclass)     vartype
#define CONST(consttype, memclass) const consttype

/* 指针声明辅助宏。 */
#define P2VAR(ptrtype, memclass, ptrclass)         ptrtype *         /* 指向变量的指针 */
#define P2CONST(ptrtype, memclass, ptrclass)       const ptrtype *   /* 指向常量的指针 */
#define CONSTP2VAR(ptrtype, memclass, ptrclass)    ptrtype * const   /* 指向变量的常指针 */
#define CONSTP2CONST(ptrtype, memclass, ptrclass)  const ptrtype * const /* 指向常量的常指针 */

/* 函数指针辅助宏。 */
#define P2FUNC(rettype, ptrclass, fctname)  rettype (*fctname)   /* 函数指针 */

#endif
