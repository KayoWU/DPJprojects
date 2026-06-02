#include "Iap.h"

#define IAP_CMD_IDLE     0u
#define IAP_CMD_READ     1u
#define IAP_CMD_PROGRAM  2u
#define IAP_CMD_ERASE    3u
#define IAP_ENABLE       0x81u

/* 关闭IAP/ISP访问：每次读写擦除结束后都要回到空闲状态。 */
static void Iap_Idle(void)
{
    ISP_CONTR = 0u;
    ISP_CMD = IAP_CMD_IDLE;
    ISP_TRIG = 0u;
    ISP_ADDRH = 0x80u;
    ISP_ADDRL = 0u;
}

/* 读取内部EEPROM一个字节：按STC要求写命令、地址，再触发操作。 */
uint8 Iap_ReadByte(uint16 address)
{
    uint8 dataValue;

    ISP_CONTR = IAP_ENABLE;
    ISP_CMD = IAP_CMD_READ;
    ISP_ADDRL = (uint8)address;
    ISP_ADDRH = (uint8)(address >> 8);
    ISP_TRIG = 0x46u;
    ISP_TRIG = 0xb9u;
    _nop_();
    dataValue = ISP_DATA;
    Iap_Idle();

    return dataValue;
}

/* 写内部EEPROM一个字节：调用前通常要先擦除所在扇区。 */
void Iap_ProgramByte(uint16 address, uint8 dataValue)
{
    ISP_CONTR = IAP_ENABLE;
    ISP_CMD = IAP_CMD_PROGRAM;
    ISP_ADDRL = (uint8)address;
    ISP_ADDRH = (uint8)(address >> 8);
    ISP_DATA = dataValue;
    ISP_TRIG = 0x46u;
    ISP_TRIG = 0xb9u;
    _nop_();
    Iap_Idle();
}

/* 擦除内部EEPROM扇区：STC的IAP写入必须以擦除为前提。 */
void Iap_EraseSector(uint16 address)
{
    ISP_CONTR = IAP_ENABLE;
    ISP_CMD = IAP_CMD_ERASE;
    ISP_ADDRL = (uint8)address;
    ISP_ADDRH = (uint8)(address >> 8);
    ISP_TRIG = 0x46u;
    ISP_TRIG = 0xb9u;
    _nop_();
    Iap_Idle();
}
