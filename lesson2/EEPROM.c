#include "EEPROM.h"

#define CMD_IDLE    0               //Stand-By
#define CMD_READ    1               //uchar-Read
#define CMD_PROGRAM 2               //uchar-Program
#define CMD_ERASE   3               //Sector-Erase
#define ENABLE_IAP  0x81            //if SYSCLK<20MHz

//========================================================================
// 函数：void IapIdle()
// 描述：EEPROM 失效
// 参数：
// 返回：
// 版本：
//========================================================================
void IapIdle()
{
    IAP_CONTR = 0;                  //Close IAP function
    IAP_CMD = 0;                    //Clear command to standby
    IAP_TRIG = 0;                   //Clear trigger register
    IAP_ADDRH = 0x80;               //Data ptr point to non-EEPROM area
    IAP_ADDRL = 0;                  //Clear IAP address to prevent misuse
}

//========================================================================
// 函数：uchar IapReadByte(uint addr)
// 描述：从指定的EEPROM地址读取数据
// 参数：addr：EEPROM的地址
// 返回：读取到的EEPROM数据
// 版本：
//========================================================================
uchar IapReadByte(uint addr)
{
    uchar dat;                      //Data buffer
    IAP_CONTR = ENABLE_IAP;         //Open IAP function, and set wait time
    IAP_CMD = CMD_READ;             //Set ISP/IAP/EEPROM READ command
    IAP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    IAP_TRIG = 0x46;                //Send trigger command1 (0x46)
    IAP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    dat = IAP_DATA;                 //Read ISP/IAP/EEPROM data
    IapIdle();                      //Close ISP/IAP/EEPROM function
    return dat;                     //Return Flash data
}

//========================================================================
// 函数：void IapProgramByte(uint addr, uchar dat)
// 描述：向指定的EEPROM地址写入数据
// 参数：addr：EEPROM的地址 ； dat：要写入的数据
// 返回：
// 版本：
//========================================================================
void IapProgramByte(uint addr, uchar dat)
{
    IAP_CONTR = ENABLE_IAP;         //Open IAP function, and set wait time
    IAP_CMD = CMD_PROGRAM;          //Set ISP/IAP/EEPROM PROGRAM command
    IAP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    IAP_DATA = dat;                 //Write ISP/IAP/EEPROM data
    IAP_TRIG = 0x46;                //Send trigger command1 (0x46)
    IAP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    IapIdle();
}

//========================================================================
// 函数：void IapEraseSector(uint addr)
// 描述：擦除指定地址的EEPROM里的数据
// 参数：addr：EEPROM的地址
// 返回：
// 版本：
//========================================================================
void IapEraseSector(uint addr)
{
    IAP_CONTR = ENABLE_IAP;         //Open IAP function, and set wait time
    IAP_CMD = CMD_ERASE;            //Set ISP/IAP/EEPROM ERASE command
    IAP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    IAP_TRIG = 0x46;                //Send trigger command1 (0x46)
    IAP_TRIG = 0xb9;                //Send trigger command2 (0xb9)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    IapIdle();
}

