/**
 * @file    Dio_Cfg.h
 * @brief   Dio port/channel identifier mapping
 *
 * Maps logical port/channel names to numeric IDs.
 * Add new entries here when supporting additional ports or pins.
 */

#ifndef DIO_CFG_H
#define DIO_CFG_H

#include "Std_Types.h"

#define DIO_PORT_P1      1u    /**< Port 1 (8-bit, P1.0–P1.7)  */
#define DIO_CHANNEL_P23  23u   /**< Pin P2.3 (single bit)       */
#define DIO_CHANNEL_P25  25u   /**< Pin P2.5 (single bit)       */

#endif
