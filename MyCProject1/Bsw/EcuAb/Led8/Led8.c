#include "Led8.h"
#include "Dio.h"

/* LED初始化：默认全灭，并释放P1总线给ADC读取使用。 */
void Led8_Init(void)
{
    Led8_Write(0x00u);
}

/*
 * 写8位LED显示值。
 * value的bit为1表示对应LED亮；硬件低电平点亮，所以输出到P1前取反。
 * 74HC573的LE=1透明传输，LE=0锁存保持；锁存后P1重新写1，避免影响ADC并口读取。
 */
void Led8_Write(uint8 value)
{
    uint8 portLevel;

#if (LED8_ACTIVE_LOW == STD_ON)
    portLevel = (uint8)(~value);
#else
    portLevel = value;
#endif

    Dio_WriteChannel(LED8_LATCH_CHANNEL, 1u); /* LE=1，P1数据透明到锁存器输出 */
    Dio_WritePort(LED8_DATA_PORT, portLevel);
    Dio_WriteChannel(LED8_LATCH_CHANNEL, 0u); /* LE=0，锁存当前LED数据 */
    Dio_WritePort(LED8_DATA_PORT, 0xffu);     /* 释放P1，方便ADC把数据送到P1 */
}
