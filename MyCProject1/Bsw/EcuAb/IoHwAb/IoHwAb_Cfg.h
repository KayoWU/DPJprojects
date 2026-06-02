/* IoHwAb配置：板级信号到Dio端口/通道的映射。 */

#ifndef IOHWAB_CFG_H
#define IOHWAB_CFG_H

#include "Dio_Cfg.h"

/* 板级8位输出端口映射到P1。 */
#define IOHWAB_BOARD_PORT     DIO_PORT_P1

/* 板级单路输出映射到P2.3。 */
#define IOHWAB_BOARD_CHANNEL  DIO_CHANNEL_P23

#endif
