#ifndef TIMER2_H
#define TIMER2_H

#include "Config.h"
#include "Timer2_Cfg.h"

#define TIMER2_VENDOR_ID                 0u
#define TIMER2_MODULE_ID                 210u
#define TIMER2_SW_MAJOR_VERSION          1u
#define TIMER2_SW_MINOR_VERSION          0u
#define TIMER2_SW_PATCH_VERSION          0u

void Timer2_Init(void);
uint16 Timer2_GetTick(void);

#endif
