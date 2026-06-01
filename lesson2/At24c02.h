#ifndef _AT24C02_H
#define _AT24C02_H

#include "Config.h"

void At24c02Init(void);
void At24c02WriteAdd(uchar address,uchar date);
uchar At24c02ReadAdd(uchar address);

#endif 
