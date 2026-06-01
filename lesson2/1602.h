#ifndef _LCD1602_H
#define _LCD1602_H

#include "Config.h"

void LcdWriteCom(uchar com);
void LcdWriteData(uchar dat);
void Lcd1602Init(void);
void ShowStringAny(uchar index, uchar line, uchar a[]);
void ShowContentAny(uchar index, uchar line, uchar date);


#endif