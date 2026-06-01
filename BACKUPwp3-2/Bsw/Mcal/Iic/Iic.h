#ifndef IIC_H
#define IIC_H

#include "Config.h"

#define IIC_VENDOR_ID                 0u
#define IIC_MODULE_ID                 208u
#define IIC_SW_MAJOR_VERSION          1u
#define IIC_SW_MINOR_VERSION          0u
#define IIC_SW_PATCH_VERSION          0u

void Iic_Init(void);
void Iic_Start(void);
void Iic_Stop(void);
void Iic_WaitAck(void);
void Iic_WriteByte(uint8 dataValue);
uint8 Iic_ReadByte(void);

#endif
