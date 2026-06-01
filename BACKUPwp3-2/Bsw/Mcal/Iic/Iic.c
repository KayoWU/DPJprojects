#include "Iic.h"

sbit Iic_Sda = P2^0;
sbit Iic_Scl = P2^1;

/* IIC总线初始化：SDA和SCL都拉高，表示总线空闲。 */
void Iic_Init(void)
{
    Iic_Sda = 1;
    _nop_();
    Iic_Scl = 1;
    _nop_();
}

/* 产生IIC起始信号：SCL为高时，SDA由高变低。 */
void Iic_Start(void)
{
    Iic_Sda = 1;
    _nop_();
    Iic_Scl = 1;
    _nop_();
    Iic_Sda = 0;
    _nop_();
}

/* 产生IIC停止信号：SCL为高时，SDA由低变高。 */
void Iic_Stop(void)
{
    Iic_Sda = 0;
    _nop_();
    Iic_Scl = 1;
    _nop_();
    Iic_Sda = 1;
    _nop_();
}

/* 等待从机应答：SDA被从机拉低表示ACK。 */
void Iic_WaitAck(void)
{
    uint8 i;

    i = 0u;
    Iic_Scl = 1;
    _nop_();
    while((Iic_Sda == 1) && (i < 250u))
    {
        i++;
    }
    Iic_Scl = 0;
    _nop_();
}

/* IIC写1字节：从高位到低位依次放到SDA，并用SCL打时钟。 */
void Iic_WriteByte(uint8 dataValue)
{
    uint8 i;
    uint8 temp;

    temp = dataValue;
    for(i = 0u; i < 8u; i++)
    {
        temp = temp << 1;
        Iic_Scl = 0;
        _nop_();
        Iic_Sda = CY;
        _nop_();
        Iic_Scl = 1;
        _nop_();
    }
    Iic_Scl = 0;
    _nop_();
    Iic_Sda = 1;
    _nop_();
}

/* IIC读1字节：每个SCL高电平采样一次SDA。 */
uint8 Iic_ReadByte(void)
{
    uint8 i;
    uint8 dataValue;

    dataValue = 0u;
    Iic_Scl = 0;
    _nop_();
    Iic_Sda = 1;
    _nop_();
    for(i = 0u; i < 8u; i++)
    {
        Iic_Scl = 1;
        _nop_();
        dataValue = (uint8)((dataValue << 1) | Iic_Sda);
        Iic_Scl = 0;
        _nop_();
    }
    return dataValue;
}
