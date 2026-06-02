# SWC 与 RTE 模块关系说明

本文档用于快速理解当前工程的 AUTOSAR-lite 应用层拆分结果。

## 总体结构

当前应用层分成三部分：

- `App/main.c`：上电入口，调用 `EcuM_Init()`，然后循环调用 `App_MainFunction()`。
- `App/App.c`：应用调度器，只负责初始化 SWC 和周期调用 SWC。
- `App/Swc/*`：具体业务组件，每个 SWC 只负责一类业务。

SWC 之间不直接包含彼此头文件，也不直接调用彼此函数。需要共享的数据和事件，统一经过 `Rte.h/Rte.c`。

## 周期调用顺序

`App_MainFunction()` 当前调用顺序如下：

```text
Rte_MainFunction()
Swc_AdcSampler_MainFunction()
Swc_Command_MainFunction()
Swc_Storage_MainFunction()
Swc_IrHandler_MainFunction()
Swc_LcdPresenter_MainFunction()
Swc_SerialTx_MainFunction()
```

其中 `Rte_MainFunction()` 会先推进底层非阻塞状态机，例如 IIC、AT24C02、LCD1602、红外解码。

## SWC 职责

### Swc_AdcSampler

负责每 100ms 采样一次 ADC。

主要动作：

- 调用 `Rte_ReadAdcValue()` 读取 ADC 原始值。
- 调用 `Rte_AdcConvertToMv()` 换算成毫伏。
- 调用 `Rte_WriteAdcSample()` 发布最新 ADC 数据。
- 调用 `Rte_DisplayRequestAdcLine()` 请求 LCD 第一行显示 ADC 数据。

### Swc_Command

负责解析串口命令。

命令含义：

- `1` 或 ASCII `'1'`：请求保存当前 ADC 数据到 AT24C02。
- `2` 或 ASCII `'2'`：请求从 AT24C02 读取 ADC 数据。

主要动作：

- 调用 `Rte_ReadSerialData()` 读取串口接收值。
- 调用 `Rte_RequestAdcSave()` 或 `Rte_RequestAdcRead()` 发布请求。
- 调用 `Rte_ClearSerialData()` 清除已处理的命令。

### Swc_Storage

负责 AT24C02 存储业务。

主要动作：

- 消费 `Rte_ConsumeAdcSaveRequest()`，把当前 ADC 数据写入 AT24C02。
- 消费 `Rte_ConsumeAdcReadRequest()`，从 AT24C02 读取保存的数据。
- 写入完成后请求 LCD 第二行显示 `SAVE OK`。
- 读取完成后请求 LCD 第二行显示读出的 ADC 数据。
- 读取完成后调用 `Rte_RequestSerialReadMessage()` 请求串口回传文本。

### Swc_IrHandler

负责红外按键业务。

主要动作：

- 调用 `Rte_IrSensorIsKeyReady()` 判断是否收到新按键。
- 调用 `Rte_IrSensorGetKey()` 获取按键值。
- 调用 `Rte_Led8Write()` 把按键值的二进制位显示到 8 位 LED。
- 调用 `Rte_DisplayRequestIrLine()` 请求 LCD 第二行显示红外键值。

### Swc_LcdPresenter

负责 LCD 显示内容组织。

主要动作：

- 消费 `Rte_DisplayConsumeAdcLine()`，更新 LCD 第一行缓存。
- 消费 `Rte_DisplayConsumeLine2()`，更新 LCD 第二行缓存。
- 调用 `Rte_Lcd1602IsBusy()` 判断 LCD 状态机是否空闲。
- 调用 `Rte_Lcd1602AsyncShowString()` 异步刷新 LCD。

它只关心“显示格式”，不直接处理 ADC、EEPROM、红外等业务。

### Swc_SerialTx

负责串口文本发送。

主要动作：

- 消费 `Rte_ConsumeSerialReadMessage()`，准备 `READ ADC=xxx MV=xxxx` 文本。
- 调用 `Rte_IsSerialTxReady()` 判断串口是否空闲。
- 调用 `Rte_SendSerialByte()` 每次只发送一个字节。

## RTE 端口分类

### 底层服务接口

这些接口向 SWC 提供底层能力：

- 串口：`Rte_ReadSerialData()`、`Rte_ClearSerialData()`、`Rte_SendSerialByte()`、`Rte_IsSerialTxReady()`
- ADC：`Rte_ReadAdcValue()`、`Rte_AdcConvertToMv()`
- 红外：`Rte_IrSensorIsKeyReady()`、`Rte_IrSensorGetKey()`、`Rte_IrSensorClearKey()`
- LED：`Rte_Led8Write()`
- LCD：`Rte_Lcd1602AsyncShowString()`、`Rte_Lcd1602IsBusy()`
- EEPROM：`Rte_At24c02AsyncWrite()`、`Rte_At24c02AsyncRead()`、`Rte_At24c02IsBusy()`、`Rte_At24c02IsJobDone()`、`Rte_At24c02GetData()`

### SWC 数据端口

这些接口用于 SWC 之间共享最新 ADC 数据：

- `Rte_WriteAdcSample()`
- `Rte_ReadAdcRaw()`
- `Rte_ReadAdcMv()`

### SWC 事件端口

这些接口用于 SWC 之间传递一次性请求：

- 命令到存储：`Rte_RequestAdcSave()`、`Rte_RequestAdcRead()`
- 存储消费命令：`Rte_ConsumeAdcSaveRequest()`、`Rte_ConsumeAdcReadRequest()`
- 显示请求：`Rte_DisplayRequestAdcLine()`、`Rte_DisplayRequestStatusLine()`、`Rte_DisplayRequestReadLine()`、`Rte_DisplayRequestIrLine()`
- 显示消费：`Rte_DisplayConsumeAdcLine()`、`Rte_DisplayConsumeLine2()`
- 串口回传请求：`Rte_RequestSerialReadMessage()`、`Rte_ConsumeSerialReadMessage()`

## 上板验收建议

建议按下面顺序验证：

1. 上电后 LCD 第二行显示 `READY`，没有乱码。
2. ADC 数据约每 100ms 刷新到 LCD 第一行。
3. 串口发送 `1` 后，LCD 第二行显示 `SAVING...`，完成后显示 `SAVE OK`。
4. 串口发送 `2` 后，LCD 第二行显示读出的 ADC 数据，串口回传 `READ ADC=xxx MV=xxxx`。
5. 红外按键后，LCD 第二行显示 `IR KEY:0xXX`。
6. 红外按键值同步显示到 P1 的 8 位 LED。

