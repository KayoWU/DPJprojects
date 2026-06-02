/* 数码管实现：先锁存位选，再锁存段码，轮流点亮各位。 */

#include "Display.h"
#include "Delay.h"
#include "Det.h"

/* P0同时接位选锁存器和段码锁存器，P2.6/P2.7决定当前锁哪一路。 */
sbit Display_Duan = P2^6;   /* 段锁存使能（P2.6）：高=透明，低=锁存 */
sbit Display_Wei  = P2^7;   /* 位锁存使能（P2.7）：高=透明，低=锁存 */

/* 共阴数码管段码表：段为1点亮，支持0~F。 */
static uchar code Display_SegmentTable[] =
{
    0x3f, 0x06, 0x5b, 0x4f,   /* 显示 0, 1, 2, 3 */
    0x66, 0x6d, 0x7d, 0x07,   /* 显示 4, 5, 6, 7 */
    0x7f, 0x6f, 0x77, 0x7c,   /* 显示 8, 9, A, B */
    0x39, 0x5e, 0x79, 0x71    /* 显示 C, D, E, F */
};

/* 位选表：每次只选中一位，低电平有效。 */
static uchar code Display_PositionTable[] =
{
    0x7e, 0x7d, 0x7b, 0x77,   /* 第 0, 1, 2, 3 位 */
    0x6f, 0x3f                 /* 第 4, 5 位 */
};

/* 初始化保护标志。 */
static boolean Display_IsInitialized = FALSE;

/* 数码管抽象层初始化：当前硬件无需额外配置，只记录模块可用。 */
void Display_Init(void)
{
    Display_IsInitialized = TRUE;  /* 数码管模块无需硬件配置，只置标志 */
}

/* 显示一位数码管：先选位置，再送段码，最后短暂保持显示。 */
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
        /* 位数超出范围，报告参数错误。 */
        (void)Det_ReportError(DISPLAY_MODULE_ID, DISPLAY_INSTANCE_ID,
                              DISPLAY_SHOW_DIGIT_API_ID, DISPLAY_E_PARAM_DIGIT_POSITION);
        return;
    }
    if(digitValue >= DISPLAY_VALUE_COUNT)
    {
        /* 显示值超出段码表范围，报告参数错误。 */
        (void)Det_ReportError(DISPLAY_MODULE_ID, DISPLAY_INSTANCE_ID,
                              DISPLAY_SHOW_DIGIT_API_ID, DISPLAY_E_PARAM_DIGIT_VALUE);
        return;
    }

    /* 第一步：位选锁存，选通哪一位数码管。 */
    P0 = 0xff;                                   /* 总线预充：全部输出高 */
    Display_Wei = 1;                             /* 位锁存变透明 */
    P0 = Display_PositionTable[digitPosition];   /* 输出位选数据 */
    Display_Wei = 0;                             /* 下降沿锁存位选 */

    /* 第二步：段码锁存，决定这一位显示什么字符。 */
    P0 = 0x00;                                   /* 总线清零 */
    Display_Duan = 1;                            /* 段锁存变透明 */
    P0 = Display_SegmentTable[digitValue];       /* 输出段码数据 */
    Display_Duan = 0;                            /* 下降沿锁存段码 */

    Delay_WaitMs(1u);                            /* 保持 1ms 让人眼看到 */
}

/* 显示三位十进制数：把0~255拆成百位、十位、个位轮流显示。 */
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
