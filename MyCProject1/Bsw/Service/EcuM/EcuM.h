/* EcuM启动管理：main上电后调用一次，用来拉起RTE和底层服务。 */

#ifndef ECUM_H
#define ECUM_H

#include "Config.h"

/* 模块标识和版本号：轻量AUTOSAR风格保留。 */
#define ECUM_VENDOR_ID                 0u
#define ECUM_MODULE_ID                 10u
#define ECUM_SW_MAJOR_VERSION          1u
#define ECUM_SW_MINOR_VERSION          0u
#define ECUM_SW_PATCH_VERSION          0u

/* ECU总初始化入口：上电后只调用一次。 */
void EcuM_Init(void);

#endif
