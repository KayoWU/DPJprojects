#include "Swc_Command.h"
#include "Rte.h"

#define SWC_COMMAND_SAVE_ADC       1u
#define SWC_COMMAND_READ_ADC       2u
#define SWC_ASCII_COMMAND_SAVE_ADC '1'
#define SWC_ASCII_COMMAND_READ_ADC '2'

void Swc_Command_Init(void)
{
}

/* 只解析串口命令；真正存储动作交给Storage SWC。 */
void Swc_Command_MainFunction(void)
{
    uint8 command;

    command = Rte_ReadSerialData();
    if((command == SWC_COMMAND_SAVE_ADC) || (command == SWC_ASCII_COMMAND_SAVE_ADC))
    {
        Rte_ClearSerialData(); /* 命令已消费，避免下一轮重复执行。 */
        Rte_RequestAdcSave();  /* 只发布请求，实际写EEPROM由Storage处理。 */
    }
    else if((command == SWC_COMMAND_READ_ADC) || (command == SWC_ASCII_COMMAND_READ_ADC))
    {
        Rte_ClearSerialData();
        Rte_RequestAdcRead();  /* Storage收到请求后再启动异步读。 */
    }
}
