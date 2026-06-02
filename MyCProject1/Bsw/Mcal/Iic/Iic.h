#ifndef IIC_H
#define IIC_H

#include "Config.h"

#define IIC_VENDOR_ID                 0u
#define IIC_MODULE_ID                 208u
#define IIC_SW_MAJOR_VERSION          1u
#define IIC_SW_MINOR_VERSION          0u
#define IIC_SW_PATCH_VERSION          0u

void Iic_Init(void);
Std_ReturnType Iic_StartOperation(uint8 operation, uint8 dataValue);
void Iic_MainFunction(void);
boolean Iic_IsBusy(void);
boolean Iic_IsDone(void);
uint8 Iic_GetReadData(void);

#define IIC_OPERATION_START       1u
#define IIC_OPERATION_STOP        2u
#define IIC_OPERATION_WAIT_ACK    3u
#define IIC_OPERATION_WRITE_BYTE  4u
#define IIC_OPERATION_READ_BYTE   5u
#define IIC_OPERATION_SEND_NACK   6u

#endif
