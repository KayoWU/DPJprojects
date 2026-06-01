#include "At24c02.h"
#include "Iic.h"

/* AT24C02初始化：先把软件IIC总线释放到空闲高电平。 */
void At24c02_Init(void)
{
    Iic_Init();
}

/* 向AT24C02写一个字节：发送器件地址、存储地址、数据。 */
void At24c02_WriteByte(uint8 address, uint8 dataValue)
{
    Iic_Start();
    Iic_WriteByte(0xa0u);
    Iic_WaitAck();
    Iic_WriteByte(address);
    Iic_WaitAck();
    Iic_WriteByte(dataValue);
    Iic_WaitAck();
    Iic_Stop();
}

/* 从AT24C02读一个字节：先写入要读的地址，再重新启动读取。 */
uint8 At24c02_ReadByte(uint8 address)
{
    uint8 dataValue;

    Iic_Start();
    Iic_WriteByte(0xa0u);
    Iic_WaitAck();
    Iic_WriteByte(address);
    Iic_WaitAck();
    Iic_Start();
    Iic_WriteByte(0xa1u);
    Iic_WaitAck();
    dataValue = Iic_ReadByte();
    Iic_Stop();

    return dataValue;
}
