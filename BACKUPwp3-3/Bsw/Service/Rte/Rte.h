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
 *   Serial services:  receive, clear, byte transmit, transmit-ready
 *   Time services:    1ms system tick and runtime state-machine dispatch
 *   ADC services:     read raw ADC and convert to millivolts
 *   LCD services:     asynchronous LCD1602 display requests
 *   EEPROM services:  asynchronous AT24C02 read/write requests
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
 * 初始化顺序：Serial_Init → Adc_Init → Lcd1602_Init → At24c02_Init → Timer2_Init
 */
void  Rte_Init(void);
void  Rte_MainFunction(void);
uint16 Rte_GetSystemTick(void);

/** @name 串口服务（封装 EcuAb/Serial） */
uint8 Rte_ReadSerialData(void);         /* 读取串口收到的字节 */
void  Rte_ClearSerialData(void);        /* 清空接收缓存 */
void  Rte_SendSerialByte(uint8 txData); /* 发送一个字节 */
boolean Rte_IsSerialTxReady(void);      /* 串口发送是否空闲 */

/** @name ADC服务（封装 Mcal/Adc） */
uint8  Rte_ReadAdcValue(void);
uint16 Rte_AdcConvertToMv(uint8 adcValue);

/** @name LCD1602服务（封装 EcuAb/Lcd1602） */
Std_ReturnType Rte_Lcd1602AsyncShowString(uint8 row, uint8 column, uint8 text[]);
boolean Rte_Lcd1602IsBusy(void);

/** @name AT24C02服务（封装 EcuAb/At24c02） */
Std_ReturnType Rte_At24c02AsyncWrite(uint8 address, uint8 dataValue[], uint8 length);
Std_ReturnType Rte_At24c02AsyncRead(uint8 address, uint8 length);
boolean Rte_At24c02IsBusy(void);
boolean Rte_At24c02IsJobDone(void);
uint8 Rte_At24c02GetData(uint8 index);

/** @name Timer2服务（封装 Service/Timer2） */
#endif
