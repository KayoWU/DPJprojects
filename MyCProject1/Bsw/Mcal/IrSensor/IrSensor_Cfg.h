#ifndef IRSENSOR_CFG_H
#define IRSENSOR_CFG_H

/* NEC红外解码脉宽范围：Timer0计数值落在这些范围内，就判断为起始码/0/1。 */
#define IRSENSOR_START_MIN_COUNT   9000u
#define IRSENSOR_START_MAX_COUNT   14000u
#define IRSENSOR_BIT0_MIN_COUNT    700u
#define IRSENSOR_BIT_SPLIT_COUNT   1450u
#define IRSENSOR_BIT1_MAX_COUNT    3000u

/* ISR只负责把pulse放入环形缓冲区；主循环来不及时，最多缓存这么多个pulse。 */
#define IRSENSOR_PULSE_BUFFER_SIZE 16u

#endif
