#ifndef LED8_CFG_H
#define LED8_CFG_H

#include "Dio_Cfg.h"

/* 8位LED通过74HC573锁存，P1是数据总线，P2.5是LE。 */
#define LED8_DATA_PORT          DIO_PORT_P1
#define LED8_LATCH_CHANNEL      DIO_CHANNEL_P25

/* 低电平点亮，所以显示值写入硬件前需要按位取反。 */
#define LED8_ACTIVE_LOW         STD_ON

#endif
