# Lightweight AUTOSAR Architecture

This project applies a small, C51-friendly AUTOSAR-style layering model to an STC89C52RC demo.

## Layers

- `App`: application scheduler and SWC business components.
  - `main.c` calls `EcuM_Init()` once, then runs `App_MainFunction()`.
  - `App.c` only initializes and cyclically calls SWC modules.
  - `App/Swc/Swc_AdcSampler` periodically samples ADC and publishes the latest sample through RTE.
  - `App/Swc/Swc_Command` parses UART commands and publishes save/read requests through RTE.
  - `App/Swc/Swc_Storage` owns the AT24C02 save/read business flow.
  - `App/Swc/Swc_LcdPresenter` formats LCD lines and submits asynchronous LCD1602 refresh requests.
  - `App/Swc/Swc_IrHandler` consumes IR keys, requests LCD display, and writes LED bits.
  - `App/Swc/Swc_SerialTx` sends response text byte by byte.
- `Bsw/Service`: lightweight service layer.
  - `EcuM` owns ECU startup initialization.
  - `Det` records the last development error and an error counter for debugging.
  - `Rte` provides lightweight application-facing access to ECU abstraction services.
  - `SchM` provides lightweight critical sections for shared interrupt data.
  - `Timer2` provides a 1ms system tick.
- `Bsw/EcuAb`: ECU abstraction layer.
  - `Serial` wraps UART communication for the application.
  - `Lcd1602` wraps the LCD1602 character display and uses a runtime non-blocking queue.
  - `At24c02` wraps the external IIC EEPROM device and uses an asynchronous transaction state machine.
  - `Led8` wraps the 8-bit active-low LED latch on P1/P2.5.
- `Bsw/Mcal`: microcontroller abstraction layer.
  - `Uart` owns UART registers and interrupt handling.
  - `Dio` owns port/channel register writes used by the LED latch.
  - `Delay` owns the simple delay loop and exposes `Delay_WaitMs()`.
  - `Adc` owns the ADC control pins and 8-bit sample read.
  - `Iic` owns the bit-banged IIC bus pins and advances one small bus step per `Iic_MainFunction()`.
  - `IrSensor` owns NEC IR remote decoding through INT1 and Timer0 pulse measurement.
- `Bsw/Common`: small common type layer.
  - `Platform_Types.h`, `Std_Types.h`, and `Compiler.h` provide lightweight AUTOSAR-style common definitions.
  - `Compiler.h` defines C51-friendly `FUNC`, `VAR`, `CONST`, and pointer abstraction macros.
  - `Std_Types.h` defines `Std_ReturnType` and `Std_VersionInfoType`.
- `Config`: project-wide legacy compatibility include.

## Dependency Direction

Dependencies should generally point downward:

`main -> EcuM/App -> Rte -> EcuAb -> Mcal -> Device registers`

Application code should not directly access hardware registers such as `SBUF`, `P0`, `P1`, or `P2`.
Application and SWC logic should call `Rte` APIs rather than directly including `Serial`, `Lcd1602`, `At24c02`, or `Iic`.
SWC modules do not include each other. Shared ADC values, command requests, display requests, and serial response requests are passed through lightweight RTE data/event ports.

## Current Application Behavior

- ADC is sampled every 100ms.
- LCD1602 line 1 shows the latest ADC raw value and voltage, for example `ADC:123 2.34V`.
- UART command `1` or ASCII `'1'`: save the current ADC raw value and voltage to AT24C02 address `0x00`.
- UART command `2` or ASCII `'2'`: read the saved ADC data from AT24C02, show it on LCD1602 line 2, and send a text response to the host.
- NEC IR remote key reception: decoded key values are shown on LCD1602 line 2, for example `IR KEY:0x44`.
- The same IR key value is written to the 8-bit LED module as binary bits. A value bit of `1` means the corresponding LED is on.
- The old board I/O and seven-segment command behavior is intentionally not kept in this application phase.

## Non-Blocking Runtime Model

- Blocking delay is still allowed during initialization, mainly for LCD1602 power-up setup.
- Runtime LCD1602 updates use `Lcd1602_AsyncShowString()` plus `Lcd1602_MainFunction()`.
- Runtime AT24C02 access uses `At24c02_AsyncWrite()`, `At24c02_AsyncRead()`, and `At24c02_MainFunction()`.
- Runtime IIC bit-banging is advanced by `Iic_MainFunction()`, so one loop only performs a small bus step.
- UART text response is sent byte by byte only when `Uart_IsTxReady()` reports the hardware is ready.
- IR decoding is interrupt-driven: INT1 captures falling edges, Timer0 measures pulse width, and App only reads the decoded key value.
- The LED latch shares P1 with the ADC data bus. `Led8_Write()` latches the LED value through P2.5, then writes `0xff` back to P1 to release the bus for ADC reads.

The intended cyclic order is:

`App_MainFunction -> Rte_MainFunction -> Iic/At24c02/Lcd1602 state machines -> SWC logic`

Current SWC order:

`AdcSampler -> Command -> Storage -> IrHandler -> LcdPresenter -> SerialTx`

## Development Error Reporting

- `Det_ReportError()` stores the last `moduleId`, `instanceId`, `apiId`, and `errorId`.
- `Det_ErrorCount` increments each time a development error is reported.
- `Serial` reports uninitialized receive, clear, or send API usage.

## Initialization State

- `EcuM_Init()` owns startup initialization and calls `Rte_Init()`.
- `Rte_Init()` initializes `Serial`, `Adc`, `IrSensor`, `Led8`, `Lcd1602`, `At24c02`, and `Timer2`.
- `Serial` keeps a private initialized flag.
- Serial APIs report `SERIAL_E_UNINIT` through Det when used before `Serial_Init()`.
- The normal path is `main -> EcuM_Init -> App_MainFunction`.

## Critical Sections

- `SchM_Enter_Uart_RxData()` saves the current global interrupt state and disables interrupts.
- `SchM_Exit_Uart_RxData()` restores the saved interrupt state.
- `Uart_GetReceivedData()` and `Uart_ClearReceivedData()` use this critical section because `Uart_RxData` is written in the UART ISR.

## Module Metadata

- Public module headers define lightweight `*_VENDOR_ID`, `*_MODULE_ID`, and `*_SW_MAJOR/MINOR/PATCH_VERSION` macros.
- Module IDs are kept stable for Det reports and debugging.
- Configuration headers such as `Uart_Cfg.h`, `Adc_Cfg.h`, `IrSensor_Cfg.h`, `Timer2_Cfg.h`, and `At24c02_Cfg.h` hold board-specific values rather than public module metadata.

## Next Candidate Steps

- Add small timeout/error status reporting for asynchronous IIC and AT24C02 jobs if hardware is disconnected.
