#ifndef LED8_H
#define LED8_H

#include "Config.h"
#include "Led8_Cfg.h"

#define LED8_VENDOR_ID                 0u
#define LED8_MODULE_ID                 212u
#define LED8_SW_MAJOR_VERSION          1u
#define LED8_SW_MINOR_VERSION          0u
#define LED8_SW_PATCH_VERSION          0u

void Led8_Init(void);
void Led8_Write(uint8 value);

#endif
