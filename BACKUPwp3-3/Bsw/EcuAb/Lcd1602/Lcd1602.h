#ifndef LCD1602_H
#define LCD1602_H

#include "Config.h"

#define LCD1602_VENDOR_ID                 0u
#define LCD1602_MODULE_ID                 206u
#define LCD1602_SW_MAJOR_VERSION          1u
#define LCD1602_SW_MINOR_VERSION          0u
#define LCD1602_SW_PATCH_VERSION          0u

void Lcd1602_Init(void);
void Lcd1602_WriteCommand(uint8 command);
Std_ReturnType Lcd1602_AsyncShowString(uint8 row, uint8 column, uint8 text[]);
void Lcd1602_MainFunction(uint16 tick);
boolean Lcd1602_IsBusy(void);

#endif
