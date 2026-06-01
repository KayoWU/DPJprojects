#ifndef AT24C02_H
#define AT24C02_H

#include "Config.h"
#include "At24c02_Cfg.h"

#define AT24C02_VENDOR_ID                 0u
#define AT24C02_MODULE_ID                 209u
#define AT24C02_SW_MAJOR_VERSION          1u
#define AT24C02_SW_MINOR_VERSION          0u
#define AT24C02_SW_PATCH_VERSION          0u

void At24c02_Init(void);
void At24c02_WriteByte(uint8 address, uint8 dataValue);
uint8 At24c02_ReadByte(uint8 address);

#endif
