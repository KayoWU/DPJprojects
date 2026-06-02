/* App层对外只暴露一个周期调度入口。 */

#ifndef APP_H
#define APP_H

#include "Config.h"

/* 轻量模块标识，便于后续调试或扩展Det。 */
#define APP_VENDOR_ID                 0u
#define APP_MODULE_ID                 1000u
#define APP_SW_MAJOR_VERSION          1u
#define APP_SW_MINOR_VERSION          0u
#define APP_SW_PATCH_VERSION          0u

/* main()死循环反复调用；函数内部只调度RTE和各个SWC。 */
void App_MainFunction(void);

#endif
