#ifndef IAP_H
#define IAP_H

#include "Config.h"
#include "Iap_Cfg.h"

#define IAP_VENDOR_ID                 0u
#define IAP_MODULE_ID                 207u
#define IAP_SW_MAJOR_VERSION          1u
#define IAP_SW_MINOR_VERSION          0u
#define IAP_SW_PATCH_VERSION          0u

uint8 Iap_ReadByte(uint16 address);
void Iap_ProgramByte(uint16 address, uint8 dataValue);
void Iap_EraseSector(uint16 address);

#endif
