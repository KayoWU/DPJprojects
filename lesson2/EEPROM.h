#ifndef _EEPROM_H
#define _EEPROM_H

#include "Config.h"

#define IAP_ADDRESS 0x2000 //Start address for STC89C52xx EEPROM

uchar IapReadByte(uint addr);
void IapProgramByte(uint addr, uchar dat);
void IapEraseSector(uint addr);

#endif 