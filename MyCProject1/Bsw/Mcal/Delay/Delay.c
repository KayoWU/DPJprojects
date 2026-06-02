/* Delay实现：用嵌套空循环粗略消耗时间。 */

#include "Delay.h"

/* 阻塞延时：靠空循环消耗时间，简单但CPU会一直等在这里。 */
void Delay_WaitMs(uint16 ms)
{
    uint i, j;
    for(i = ms; i > 0; i--)          /* 外层：ms 次 */
    {
        for(j = 115; j > 0; j--);    /* 内层：115 次 ≈ 1ms（11.0592MHz） */
    }
}
