#include "Timer2.h"

static volatile boolean Timer2_ElapsedFlag = FALSE;

/* 初始化Timer2：配置重装值并打开中断，用来产生周期节拍标志。 */
void Timer2_Init(void)
{
    RCAP2L = TL2 = (uint8)TIMER2_RELOAD;
    RCAP2H = TH2 = (uint8)(TIMER2_RELOAD >> 8);
    TR2 = 1;
    ET2 = 1;
    EA = 1;
}

/* 读取周期标志：TRUE表示Timer2已经累计到一个应用周期。 */
boolean Timer2_GetElapsedFlag(void)
{
    return Timer2_ElapsedFlag;
}

/* 清除周期标志：应用处理完本次周期事件后调用。 */
void Timer2_ClearElapsedFlag(void)
{
    Timer2_ElapsedFlag = FALSE;
}

/* Timer2中断：只做计数和置位标志，避免在中断里做复杂业务。 */
void Timer2_Isr(void) interrupt 5
{
    static uint8 count = 0u;

    TF2 = 0;
    count++;
    if(count >= 20u)
    {
        count = 0u;
        Timer2_ElapsedFlag = TRUE;
    }
}
