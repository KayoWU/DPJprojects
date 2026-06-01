/**
 * @file    Delay.c
 * @brief   Software delay implementation — nested busy loops
 *
 * The inner-loop count of 115 is calibrated for ~1 ms per outer
 * iteration at ~11.0592 MHz with default compiler optimisation.
 * This is approximate; use a hardware timer for precise timing.
 */

#include "Delay.h"

/**
 * 两层空循环实现阻塞延时。
 * 外层循环一次 ≈ 1ms，内层 115 次是在 11.0592MHz 晶振下校准的值。
 */
void Delay_WaitMs(uint16 ms)
{
    uint i, j;
    for(i = ms; i > 0; i--)          /* 外层：ms 次 */
    {
        for(j = 115; j > 0; j--);    /* 内层：115 次 ≈ 1ms（11.0592MHz） */
    }
}
