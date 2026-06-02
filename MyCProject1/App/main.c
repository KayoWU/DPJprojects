/* 程序入口：上电初始化一次，然后进入无OS的超级循环。 */

#include "EcuM.h"
#include "App.h"

int main(void)
{
    EcuM_Init();          /* 上电后一次性初始化所有硬件和BSW模块 */
    while(1)
    {
        App_MainFunction(); /* 周期调度RTE和各个SWC */
    }
}
