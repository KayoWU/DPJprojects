/**
 * @file    Rte.h
 * @brief   Runtime Environment — application-facing service interface
 *
 * The RTE is the ONLY interface through which the Application layer
 * accesses hardware. App code never calls Serial/IoHwAb/Display directly.
 *
 * This implements the AUTOSAR principle of strict separation:
 *   App → Rte → EcuAb → Mcal → Hardware registers
 *
 * API grouping:
 *   Serial services:  Rte_ReadSerialData, Rte_ClearSerialData, Rte_SendSerialByte
 *   I/O services:     Rte_WriteBoardPort, Rte_WriteBoardChannel
 *   Display services: Rte_WriteDisplayNumber
 *   Demo services:    ADC, LCD1602, IAP EEPROM, AT24C02 and Timer2 wrappers
 */

#ifndef RTE_H
#define RTE_H

#include "Config.h"

/** Module identification (AUTOSAR-lite convention) */
#define RTE_VENDOR_ID                 0u
#define RTE_MODULE_ID                 2u
#define RTE_SW_MAJOR_VERSION          1u
#define RTE_SW_MINOR_VERSION          0u
#define RTE_SW_PATCH_VERSION          0u

/**
 * @brief RTE 初始化 — 级联初始化所有ECU抽象层服务
 *
 * 初始化顺序：Serial_Init → IoHwAb_Init → Display_Init → integrated demo modules
 */
void  Rte_Init(void);

/** @name 串口服务（封装 EcuAb/Serial） */
uint8 Rte_ReadSerialData(void);         /* 读取串口收到的字节 */
void  Rte_ClearSerialData(void);        /* 清空接收缓存 */
void  Rte_SendSerialByte(uint8 txData); /* 发送一个字节 */

/** @name 板级I/O服务（封装 EcuAb/IoHwAb） */
void  Rte_WriteBoardPort(uint8 portLevel);     /* 写整板端口 */
void  Rte_WriteBoardChannel(uint8 channelLevel); /* 写单通道 */

/** @name 显示服务（封装 EcuAb/Display） */
void  Rte_WriteDisplayNumber(uint8 displayValue); /* 刷新数码管显示 */

/** @name ADC服务（封装 Mcal/Adc） */
uint8  Rte_ReadAdcValue(void);
uint16 Rte_AdcConvertToMv(uint8 adcValue);

/** @name LCD1602服务（封装 EcuAb/Lcd1602） */
void Rte_Lcd1602ShowString(uint8 row, uint8 column, uint8 text[]);
void Rte_Lcd1602ShowChar(uint8 row, uint8 column, uint8 dataValue);

/** @name IAP EEPROM服务（封装 Mcal/Iap） */
uint8 Rte_IapReadByte(uint16 address);
void  Rte_IapProgramByte(uint16 address, uint8 dataValue);
void  Rte_IapEraseSector(uint16 address);

/** @name AT24C02服务（封装 EcuAb/At24c02） */
uint8 Rte_At24c02ReadByte(uint8 address);
void  Rte_At24c02WriteByte(uint8 address, uint8 dataValue);

/** @name Timer2服务（封装 Service/Timer2） */
boolean Rte_Timer2GetElapsedFlag(void);
void    Rte_Timer2ClearElapsedFlag(void);

#endif
