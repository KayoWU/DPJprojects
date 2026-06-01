# Lightweight AUTOSAR Architecture

This project keeps the original STC89C52RC behavior and applies a small, C51-friendly AUTOSAR-style layering model.

## Layers

- `App`: application logic and cyclic runnable.
  - `main.c` calls `EcuM_Init()` once, then runs `App_MainFunction()`.
  - `App.c` reads input, dispatches commands, and refreshes the display.
  - `App_Command.c` owns command behavior.
  - `App_Command_Cfg.h` owns command protocol constants and output levels.
- `Bsw/Service`: lightweight service layer.
  - `EcuM` owns ECU startup initialization.
  - `Det` records the last development error and an error counter for debugging.
  - `Rte` provides lightweight application-facing access to ECU abstraction services.
  - `SchM` provides lightweight critical sections for shared interrupt data.
- `Bsw/EcuAb`: ECU abstraction layer.
  - `Serial` wraps UART communication for the application.
  - `IoHwAb` wraps board-level digital outputs.
  - `Display` wraps the seven-segment display.
- `Bsw/Mcal`: microcontroller abstraction layer.
  - `Uart` owns UART registers and interrupt handling.
  - `Dio` owns digital port/channel register access.
  - `Delay` owns the simple delay loop and exposes `Delay_WaitMs()`.
- `Bsw/Common`: small common type layer.
  - `Platform_Types.h`, `Std_Types.h`, and `Compiler.h` provide lightweight AUTOSAR-style common definitions.
  - `Compiler.h` defines C51-friendly `FUNC`, `VAR`, `CONST`, and pointer abstraction macros.
  - `Std_Types.h` defines `Std_ReturnType` and `Std_VersionInfoType`.
- `Config`: project-wide legacy compatibility include.

## Dependency Direction

Dependencies should generally point downward:

`main -> EcuM/App -> Rte -> EcuAb -> Mcal -> Device registers`

Application code should not directly access hardware registers such as `SBUF`, `P0`, `P1`, `P2`, or `P23`.
Application logic should call `Rte` APIs rather than directly including `Serial`, `IoHwAb`, or `Display`.

## Preserved Behavior

- Command `1`: write `0x00` to the board output port and display `001`.
- Command `2`: write `0` to the board output channel and display `002`.
- Command `3`: write `0xff` to the board output port, write `1` to the board output channel, and display `003`.
- Command `4`: send byte `4`, clear received data, and display `000`.

## Development Error Reporting

- `Det_ReportError()` stores the last `moduleId`, `instanceId`, `apiId`, and `errorId`.
- `Det_ErrorCount` increments each time a development error is reported.
- `Dio` reports unsupported port/channel IDs.
- `Display` reports invalid digit positions and digit values before accessing lookup tables.
- `Serial` reports uninitialized receive, clear, or send API usage.

## Initialization State

- `EcuM_Init()` owns startup initialization and calls `Rte_Init()`.
- `Rte_Init()` initializes `Serial`, `IoHwAb`, and `Display`.
- `Serial`, `IoHwAb`, and `Display` keep private initialized flags.
- Serial APIs report `SERIAL_E_UNINIT` through Det when used before `Serial_Init()`.
- IoHwAb APIs report `IOHWAB_E_UNINIT` through Det when used before `IoHwAb_Init()`.
- Display APIs report `DISPLAY_E_UNINIT` through Det when used before `Display_Init()`.
- The normal `main -> EcuM_Init -> App_MainFunction` path preserves the original runtime behavior.

## Critical Sections

- `SchM_Enter_Uart_RxData()` saves the current global interrupt state and disables interrupts.
- `SchM_Exit_Uart_RxData()` restores the saved interrupt state.
- `Uart_GetReceivedData()` and `Uart_ClearReceivedData()` use this critical section because `Uart_RxData` is written in the UART ISR.

## Module Metadata

- Public module headers define lightweight `*_VENDOR_ID`, `*_MODULE_ID`, and `*_SW_MAJOR/MINOR/PATCH_VERSION` macros.
- Module IDs are kept stable for Det reports and debugging.
- Configuration headers such as `Dio_Cfg.h`, `Uart_Cfg.h`, `Display_Cfg.h`, and `IoHwAb_Cfg.h` hold board-specific values rather than public module metadata.
- Application configuration headers such as `App_Command_Cfg.h` hold protocol values and behavior constants used by App code.

## Next Candidate Steps

- Add init state checks to other modules only where they protect real misuse, while keeping C51 code size small.
