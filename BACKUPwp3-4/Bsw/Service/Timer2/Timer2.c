#include "Timer2.h"

static volatile uint16 Timer2_Tick = 0u;

/* 初始化Timer2：配置重装值并打开中断，用来产生周期节拍标志。 */
void Timer2_Init(void)
{
    RCAP2L = TL2 = (uint8)TIMER2_RELOAD;
    RCAP2H = TH2 = (uint8)(TIMER2_RELOAD >> 8);
    Timer2_Tick = 0u;
    TR2 = 1;
    ET2 = 1;
    EA = 1;
}

/* 读取1ms系统节拍：临界区保护16位变量，避免读到一半被中断改写。 */
uint16 Timer2_GetTick(void)
{
    uint16 tick;
    boolean savedEa;

    savedEa = EA;
    EA = 0;
    tick = Timer2_Tick;
    EA = savedEa;

    return tick;
}

/* Timer2中断：只做计数和置位标志，避免在中断里做复杂业务。 */
void Timer2_Isr(void) interrupt 5
{
    TF2 = 0;
    Timer2_Tick++;
}
