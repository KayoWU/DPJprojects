/* Dio配置：把板级用到的端口/引脚映射成逻辑ID。 */

#ifndef DIO_CFG_H
#define DIO_CFG_H

#include "Std_Types.h"

#define DIO_PORT_P1      1u    /* P1.0~P1.7整端口 */
#define DIO_CHANNEL_P23  23u   /* P2.3单引脚 */
#define DIO_CHANNEL_P25  25u   /* P2.5单引脚，8位LED锁存LE */

#endif
