# AUTOSAR-lite 阶段追踪矩阵

本文档把本阶段目标、代码位置、验证方式和仍需上板确认的项目对应起来。

## 结构目标追踪

| 目标 | 当前实现位置 | 代码层验证方式 |
| --- | --- | --- |
| `App.c` 仅保留初始化与周期调度 | `App/App.c` | `Tools/Check_AppLayer.ps1` 检查 6 个 SWC 是否都被初始化和调度 |
| ADC 采集拆出 SWC | `App/Swc/Swc_AdcSampler.c` | 代码调用 `Rte_ReadAdcValue()`、`Rte_AdcConvertToMv()`、`Rte_WriteAdcSample()` |
| 串口命令处理拆出 SWC | `App/Swc/Swc_Command.c` | 代码调用 `Rte_ReadSerialData()` 并发布保存/读取请求 |
| AT24C02 存储业务拆出 SWC | `App/Swc/Swc_Storage.c` | 代码消费保存/读取请求，并调用 AT24C02 RTE 异步接口 |
| LCD 显示业务拆出 SWC | `App/Swc/Swc_LcdPresenter.c` | 代码消费显示请求，并调用 `Rte_Lcd1602AsyncShowString()` |
| 红外处理拆出 SWC | `App/Swc/Swc_IrHandler.c` | 代码读取 IR RTE 接口，并请求 LCD/LED 更新 |
| 串口逐字节发送拆出 SWC | `App/Swc/Swc_SerialTx.c` | 代码消费串口文本请求，并在 `Rte_IsSerialTxReady()` 后发送 |
| SWC 不互相包含 | `App/Swc/*.c` | `Tools/Check_AppLayer.ps1` 检查每个 SWC 只包含自己的头文件和 `Rte.h` |
| SWC 不直接调用底层模块 | `App/Swc/*.c` | `Tools/Check_AppLayer.ps1` 检查 SWC 不包含底层驱动头文件 |
| App 不直接访问寄存器 | `App/App.c` | `Tools/Check_AppLayer.ps1` 检查 `SBUF/P0/P1/P2/P3` |
| RTE 提供 SWC 数据/事件端口 | `Bsw/Service/Rte/Rte.h`、`Rte.c` | `Tools/Check_AppLayer.ps1` 检查端口声明是否存在 |

## 功能目标追踪

| 功能 | 当前代码路径 | 软件证据 | 上板验收 |
| --- | --- | --- | --- |
| ADC 周期显示 | `Swc_AdcSampler` → `Rte_DisplayRequestAdcLine` → `Swc_LcdPresenter` | `Swc_AdcSampler` 100ms 采样，`Swc_LcdPresenter` 格式化 `ADC:xxx x.xxV` | LCD 第一行周期刷新 ADC 数据 |
| 命令 1 保存 ADC 到 EEPROM | `Swc_Command` → `Rte_RequestAdcSave` → `Swc_Storage` | `Swc_Storage` 使用 `Rte_At24c02AsyncWrite()`，完成后请求 `SAVE OK` | 串口发 `1` 后 LCD 显示保存过程和 `SAVE OK` |
| 命令 2 读取 EEPROM | `Swc_Command` → `Rte_RequestAdcRead` → `Swc_Storage` | `Swc_Storage` 使用 `Rte_At24c02AsyncRead()`，完成后请求 LCD 和串口响应 | 串口发 `2` 后 LCD 显示 `READ:xxx x.xxV` |
| 串口读取结果回传 | `Swc_Storage` → `Rte_RequestSerialReadMessage` → `Swc_SerialTx` | `Swc_SerialTx` 生成 `READ ADC=xxx MV=xxxx` 并逐字节发送 | 上位机收到读取结果文本 |
| 红外按键显示 | `Swc_IrHandler` → `Rte_DisplayRequestIrLine` → `Swc_LcdPresenter` | `Swc_LcdPresenter` 格式化 `IR KEY:0xXX` | 红外按键后 LCD 第二行显示键值 |
| 红外按键输出到 8 位 LED | `Swc_IrHandler` → `Rte_Led8Write` → `Led8_Write` | `Led8_Write()` 通过 P2.5 锁存 P1 输出后释放 P1 | P1 低电平点亮 LED，与按键值二进制一致 |
| LCD 运行阶段非阻塞 | `Swc_LcdPresenter` → `Rte_Lcd1602AsyncShowString` → `Lcd1602_MainFunction` | LCD 显示请求入队，`Rte_MainFunction()` 推进状态机 | LCD 刷新时主循环仍能响应串口/红外 |
| AT24C02 运行阶段非阻塞 | `Swc_Storage` → `Rte_At24c02AsyncWrite/Read` → `At24c02_MainFunction` | AT24C02 只发起任务，IIC 由 `Rte_MainFunction()` 推进 | EEPROM 操作期间系统仍能继续轮询 |

## 回归命令

一键回归：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Tools\Run_StageRegression.ps1 -Root .
```

静态检查：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Tools\Check_AppLayer.ps1 -Root .
```

Keil 编译：

```powershell
& 'E:\Keil5\UV4\UV4.exe' -b UART.uvproj -j0 -o Objects\codex_build.log
```

## 完成度说明

代码层面已经可以证明：

- App 层耦合度已经降低。
- 业务已经按 SWC 拆分。
- SWC 通过 RTE 交互。
- 工程可编译。
- 没有引入 OS、生成器或复杂 RTE。

仍然必须由实物板证明：

- ADC、LCD、AT24C02、串口、红外、8 位 LED 在真实硬件上的组合行为完全符合预期。
