# 阶段验收清单

本文档记录当前 AUTOSAR-lite App 解耦阶段的验收点。

## 代码结构验收

- `App/App.c` 只保留 SWC 初始化和周期调度。
- `App/Swc` 下包含 6 个业务组件：
  - `Swc_AdcSampler`
  - `Swc_Command`
  - `Swc_Storage`
  - `Swc_IrHandler`
  - `Swc_LcdPresenter`
  - `Swc_SerialTx`
- SWC 之间不互相包含头文件。
- SWC 只通过 `Rte.h` 使用底层服务或传递数据/事件。
- `UART.uvproj` 已加入 `.\App\Swc` include path。
- `UART.uvproj` 已加入 6 个 SWC `.c` 文件。

## 代码层可验证项目

这些项目可以通过静态检查和 Keil 编译确认：

- App 层没有直接包含 `Serial.h`、`Lcd1602.h`、`At24c02.h`、`Iic.h`、`Adc.h`、`IrSensor.h`、`Led8.h`。
- App 层没有直接访问 `SBUF`、`P0`、`P1`、`P2`、`P3` 等硬件寄存器。
- LCD 运行阶段仍通过 `Rte_Lcd1602AsyncShowString()` 异步刷新。
- AT24C02 运行阶段仍通过 `Rte_At24c02AsyncWrite()`、`Rte_At24c02AsyncRead()` 异步读写。
- 串口回传仍通过 `Rte_IsSerialTxReady()` 判断空闲后逐字节发送。
- Keil C51 工程可以生成 HEX，且无 Error、无 Warning。

可使用下面命令做 App 分层静态检查：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Tools\Check_AppLayer.ps1 -Root .
```

也可以使用下面命令一次完成“App分层静态检查 + Keil编译”：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Tools\Run_StageRegression.ps1 -Root .
```

检查内容包括：

- 6 个 SWC 的 `.c/.h` 文件是否存在。
- Keil 工程是否加入 `.\App\Swc` include path 和 6 个 SWC `.c` 文件。
- `App.c` 是否只调度 SWC，没有直接包含底层驱动头文件。
- `App.c` 是否没有直接访问 `SBUF`、`P0`、`P1`、`P2`、`P3`。
- SWC 是否只包含自己的头文件和 `Rte.h`，不直接包含其他 SWC 或底层驱动。
- `Rte.h` 是否暴露当前 SWC 之间需要的数据/事件端口。

## 上板功能验收

这些项目必须在 STC89C52RC 实物板上确认：

1. 上电后 LCD1602 第二行显示 `READY`，无乱码。
2. LCD1602 第一行周期显示 ADC 数据，格式类似 `ADC:123 2.34V`。
3. 串口发送 `1` 后，AT24C02 保存当前 ADC 数据，LCD 第二行显示 `SAVING...` 后变为 `SAVE OK`。
4. 串口发送 `2` 后，从 AT24C02 读出数据，LCD 第二行显示 `READ:xxx x.xxV`。
5. 串口发送 `2` 后，上位机收到 `READ ADC=xxx MV=xxxx`。
6. 红外接收有效按键后，LCD 第二行显示 `IR KEY:0xXX`。
7. 红外按键值的二进制位同步输出到 P1 的 8 位 LED。
8. ADC 采样与 LED 共用 P1 时，ADC 显示不应因 LED 锁存输出而明显异常。

## 当前阶段停止点

当前阶段的重点是降低 App 层耦合度，不继续引入 OS、RTE 生成器或复杂配置工具。

后续如果上板发现功能异常，建议优先按以下顺序定位：

1. 先确认 `Rte_MainFunction()` 是否持续被调用。
2. 再确认对应 SWC 的 `MainFunction` 是否被调度。
3. 然后检查 RTE 请求/消费标志是否被正确置位和清除。
4. 最后再进入 BSW 底层模块检查硬件时序。
