/**
 * @file    IoHwAb_Cfg.h
 * @brief   I/O Hardware Abstraction configuration
 *
 * Maps board-level signal names to MCAL Dio port/channel identifiers.
 * Change these when porting to a different board.
 */

#ifndef IOHWAB_CFG_H
#define IOHWAB_CFG_H

#include "Dio_Cfg.h"

/** Board output port — maps to MCU P1 */
#define IOHWAB_BOARD_PORT     DIO_PORT_P1

/** Board output channel — maps to MCU P2.3 */
#define IOHWAB_BOARD_CHANNEL  DIO_CHANNEL_P23

#endif
