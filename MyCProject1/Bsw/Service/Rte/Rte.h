/* RTE对上提供统一接口：SWC不直接碰底层模块，也不互相直接调用。 */

#ifndef RTE_H
#define RTE_H

#include "Config.h"

/* 轻量模块标识，便于后续调试或扩展Det。 */
#define RTE_VENDOR_ID                 0u
#define RTE_MODULE_ID                 2u
#define RTE_SW_MAJOR_VERSION          1u
#define RTE_SW_MINOR_VERSION          0u
#define RTE_SW_PATCH_VERSION          0u

#define RTE_DISPLAY_LINE2_NONE        0u
#define RTE_DISPLAY_LINE2_STATUS      1u
#define RTE_DISPLAY_LINE2_READ_ADC    2u
#define RTE_DISPLAY_LINE2_IR_KEY      3u

/* 级联初始化底层服务：Serial → ADC → IR → LED → LCD1602 → AT24C02 → Timer2。 */
void  Rte_Init(void);
void  Rte_MainFunction(void);
uint16 Rte_GetSystemTick(void);

/* 串口服务：命令接收和文本逐字节发送都走这里。 */
uint8 Rte_ReadSerialData(void);         /* 读取串口收到的字节 */
void  Rte_ClearSerialData(void);        /* 清空接收缓存 */
void  Rte_SendSerialByte(uint8 txData); /* 发送一个字节 */
boolean Rte_IsSerialTxReady(void);      /* 串口发送是否空闲 */

/* ADC服务：读取8位原始值，并换算成毫伏。 */
uint8  Rte_ReadAdcValue(void);
uint16 Rte_AdcConvertToMv(uint8 adcValue);

/* 红外服务：读取已解码的按键值，不暴露INT1/Timer0细节。 */
boolean Rte_IrSensorIsKeyReady(void);
uint8 Rte_IrSensorGetKey(void);
void Rte_IrSensorClearKey(void);

/* 8位LED服务：value的bit对应8个LED。 */
void Rte_Led8Write(uint8 value);

/* LCD服务：运行阶段只提交异步显示请求。 */
Std_ReturnType Rte_Lcd1602AsyncShowString(uint8 row, uint8 column, uint8 text[]);
boolean Rte_Lcd1602IsBusy(void);

/* AT24C02服务：读写只发起任务，状态机在Rte_MainFunction中推进。 */
Std_ReturnType Rte_At24c02AsyncWrite(uint8 address, uint8 dataValue[], uint8 length);
Std_ReturnType Rte_At24c02AsyncRead(uint8 address, uint8 length);
boolean Rte_At24c02IsBusy(void);
boolean Rte_At24c02IsJobDone(void);
uint8 Rte_At24c02GetData(uint8 index);

/* SWC数据端口：AdcSampler写入最新值，Storage读取。 */
void Rte_WriteAdcSample(uint8 adcRaw, uint16 adcMv);
uint8 Rte_ReadAdcRaw(void);
uint16 Rte_ReadAdcMv(void);

/* SWC事件端口：Command发布保存/读取请求，Storage消费。 */
void Rte_RequestAdcSave(void);
void Rte_RequestAdcRead(void);
boolean Rte_ConsumeAdcSaveRequest(void);
boolean Rte_ConsumeAdcReadRequest(void);

/* SWC显示端口：业务组件只提出“显示什么”，LCD Presenter负责格式化。 */
void Rte_DisplayRequestAdcLine(uint8 adcRaw, uint16 adcMv);
boolean Rte_DisplayConsumeAdcLine(uint8 *adcRaw, uint16 *adcMv);
void Rte_DisplayRequestStatusLine(uint8 text[]);
void Rte_DisplayRequestReadLine(uint8 adcRaw, uint16 adcMv);
void Rte_DisplayRequestIrLine(uint8 keyValue);
boolean Rte_DisplayConsumeLine2(uint8 *lineType,
                                uint8 *adcRaw,
                                uint16 *adcMv,
                                uint8 *keyValue,
                                uint8 text[]);

/* SWC串口回传端口：Storage发布读回数据，SerialTx逐字节发送。 */
void Rte_RequestSerialReadMessage(uint8 adcRaw, uint16 adcMv);
boolean Rte_ConsumeSerialReadMessage(uint8 *adcRaw, uint16 *adcMv);

#endif
