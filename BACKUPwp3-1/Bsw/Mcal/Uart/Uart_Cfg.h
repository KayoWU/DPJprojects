/**
 * @file    Uart_Cfg.h
 * @brief   UART configuration — oscillator frequency and baud rate
 *
 * Timer 1 reload value is computed as: 256 - (FOSC / 12 / 32 / BAUD)
 *   e.g. 256 - (11059200 / 12 / 32 / 9600) = 256 - 3 = 253 (0xFD)
 */

#ifndef UART_CFG_H
#define UART_CFG_H

#define UART_FOSC 11059200L   /**< System oscillator frequency (Hz) */
#define UART_BAUD 9600u       /**< Desired baud rate                */

#endif
