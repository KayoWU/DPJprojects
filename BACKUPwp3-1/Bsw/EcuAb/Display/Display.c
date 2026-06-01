/**
 * @file    Display.c
 * @brief   7-segment display driver implementation
 *
 * Hardware interface:
 *   - P0:        data bus (shared for segment and position)
 *   - P2.6 (段): segment latch control — latches segment data on falling edge
 *   - P2.7 (位): position latch control — latches position data on falling edge
 *
 * Display cycle:  latch position → latch segment → hold 1ms
 * Each digit is illuminated in turn by ShowNumber().
 *
 * Note: this module accesses P0/P2.6/P2.7 directly for C51 code-size
 * efficiency. A full AUTOSAR implementation would route these through
 * an Mcal/Port module.
 */

#include "Display.h"
#include "Delay.h"
#include "Det.h"

/**
 * 硬件锁存器控制引脚
 *
 * 电路原理：P0 口同时连接段锁存器和位锁存器的数据输入端。
 * 要显示一个数字，需要先用位锁存器选通某一位数码管，再用段锁存器输出段码。
 * 锁存器是透明型的：Duan/Wei=1 时跟着输入走，=0 时锁住。
 */
sbit Display_Duan = P2^6;   /* 段锁存使能（P2.6）：高=透明，低=锁存 */
sbit Display_Wei  = P2^7;   /* 位锁存使能（P2.7）：高=透明，低=锁存 */

/**
 * 共阴数码管段码表（0-F）
 * 每个字节的位对应：{dp, g, f, e, d, c, b, a}
 * 共阴 → 段为 1 点亮
 */
static uchar code Display_SegmentTable[] =
{
    0x3f, 0x06, 0x5b, 0x4f,   /* 显示 0, 1, 2, 3 */
    0x66, 0x6d, 0x7d, 0x07,   /* 显示 4, 5, 6, 7 */
    0x7f, 0x6f, 0x77, 0x7c,   /* 显示 8, 9, A, B */
    0x39, 0x5e, 0x79, 0x71    /* 显示 C, D, E, F */
};

/**
 * 位选表（第 0~5 位数码管）
 * 每次只选中其中一位，低电平有效。
 */
static uchar code Display_PositionTable[] =
{
    0x7e, 0x7d, 0x7b, 0x77,   /* 第 0, 1, 2, 3 位 */
    0x6f, 0x3f                 /* 第 4, 5 位 */
};

/** 模块初始化标志 */
static boolean Display_IsInitialized = FALSE;

void Display_Init(void)
{
    Display_IsInitialized = TRUE;  /* 数码管模块无需硬件配置，只置标志 */
}

/**
 * 显示一位数码管
 *
 * 参数有效检查 → 位选锁存 → 段码锁存 → 延时 1ms 让眼睛看到
 */
void Display_ShowDigit(uint8 digitPosition, uint8 digitValue)
{
    /* 参数有效性检查 */
    if(Display_IsInitialized == FALSE)
    {
        (void)Det_ReportError(DISPLAY_MODULE_ID, DISPLAY_INSTANCE_ID,
                              DISPLAY_SHOW_DIGIT_API_ID, DISPLAY_E_UNINIT);
        return;
    }
    if(digitPosition >= DISPLAY_DIGIT_COUNT)
    {
        /** 位数超出范围（≥6），报告参数错误 */
        (void)Det_ReportError(DISPLAY_MODULE_ID, DISPLAY_INSTANCE_ID,
                              DISPLAY_SHOW_DIGIT_API_ID, DISPLAY_E_PARAM_DIGIT_POSITION);
        return;
    }
    if(digitValue >= DISPLAY_VALUE_COUNT)
    {
        /** 显示值超出段码表范围（≥16），报告参数错误 */
        (void)Det_ReportError(DISPLAY_MODULE_ID, DISPLAY_INSTANCE_ID,
                              DISPLAY_SHOW_DIGIT_API_ID, DISPLAY_E_PARAM_DIGIT_VALUE);
        return;
    }

    /* ---- 第一步：位选锁存（选通哪一位数码管） ---- */
    P0 = 0xff;                                   /* 总线预充：全部输出高 */
    Display_Wei = 1;                             /* 位锁存变透明 */
    P0 = Display_PositionTable[digitPosition];   /* 输出位选数据 */
    Display_Wei = 0;                             /* 下降沿锁存位选 */

    /* ---- 第二步：段码锁存（显示什么字符） ---- */
    P0 = 0x00;                                   /* 总线清零 */
    Display_Duan = 1;                            /* 段锁存变透明 */
    P0 = Display_SegmentTable[digitValue];       /* 输出段码数据 */
    Display_Duan = 0;                            /* 下降沿锁存段码 */

    Delay_WaitMs(1u);                            /* 保持 1ms 让人眼看到 */
}

/**
 * 显示三位十进制数（范围 0~255）
 *
 * 例如：123 → 百位显示"1"、十位显示"2"、个位显示"3"
 */
void Display_ShowNumber(uint8 displayValue)
{
    if(Display_IsInitialized == FALSE)
    {
        (void)Det_ReportError(DISPLAY_MODULE_ID, DISPLAY_INSTANCE_ID,
                              DISPLAY_SHOW_NUMBER_API_ID, DISPLAY_E_UNINIT);
        return;
    }

    Display_ShowDigit(DISPLAY_DIGIT_HUNDREDS, displayValue / 100u);       /* 百位：123/100=1 */
    Display_ShowDigit(DISPLAY_DIGIT_TENS,     displayValue / 10u % 10u);  /* 十位：12%10=2  */
    Display_ShowDigit(DISPLAY_DIGIT_ONES,     displayValue % 10u);        /* 个位：123%10=3 */
}
