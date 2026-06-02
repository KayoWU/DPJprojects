/* EcuM实现：当前只负责把启动流程交给RTE。 */

#include "EcuM.h"
#include "Rte.h"

/* 系统启动管理：当前只负责触发RTE初始化，后续模块都从这里串起来。 */
void EcuM_Init(void)
{
    Rte_Init();  /* 级联初始化各个BSW服务和硬件驱动 */
}
