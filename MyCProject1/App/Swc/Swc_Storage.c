#include "Swc_Storage.h"
#include "Rte.h"

#define SWC_STORAGE_AT24_ADDRESS     0x00u
#define SWC_STORAGE_DATA_LENGTH      3u

#define SWC_STORAGE_JOB_NONE         0u
#define SWC_STORAGE_JOB_SAVE         1u
#define SWC_STORAGE_JOB_READ         2u

static uint8 Swc_Storage_Job = SWC_STORAGE_JOB_NONE;

void Swc_Storage_Init(void)
{
    Swc_Storage_Job = SWC_STORAGE_JOB_NONE;
}

static void Swc_Storage_StartSave(void)
{
    uint8 writeData[SWC_STORAGE_DATA_LENGTH];
    uint8 adcRaw;
    uint16 adcMv;

    if(Rte_At24c02IsBusy() == TRUE)
    {
        /* EEPROM状态机仍在跑，本次命令只提示忙，不打断当前任务。 */
        Rte_DisplayRequestStatusLine("AT24 BUSY");
        return;
    }

    /* EEPROM保存3字节：ADC原始值 + 毫伏高字节 + 毫伏低字节。 */
    adcRaw = Rte_ReadAdcRaw();
    adcMv = Rte_ReadAdcMv();
    writeData[0] = adcRaw;
    writeData[1] = (uint8)(adcMv >> 8);
    writeData[2] = (uint8)adcMv;
    if(Rte_At24c02AsyncWrite(SWC_STORAGE_AT24_ADDRESS, writeData, SWC_STORAGE_DATA_LENGTH) == E_OK)
    {
        Swc_Storage_Job = SWC_STORAGE_JOB_SAVE;
        Rte_DisplayRequestStatusLine("SAVING..."); /* 先显示进行中，完成后再显示SAVE OK。 */
    }
}

static void Swc_Storage_StartRead(void)
{
    if(Rte_At24c02IsBusy() == TRUE)
    {
        /* 读写共用同一个AT24C02状态机，忙时不发起新任务。 */
        Rte_DisplayRequestStatusLine("AT24 BUSY");
        return;
    }

    /* 只发起异步读，读完后的处理在CheckDone里完成。 */
    if(Rte_At24c02AsyncRead(SWC_STORAGE_AT24_ADDRESS, SWC_STORAGE_DATA_LENGTH) == E_OK)
    {
        Swc_Storage_Job = SWC_STORAGE_JOB_READ;
        Rte_DisplayRequestStatusLine("READING...");
    }
}

static void Swc_Storage_CheckDone(void)
{
    uint8 adcRaw;
    uint16 adcMv;

    if((Swc_Storage_Job != SWC_STORAGE_JOB_NONE) && (Rte_At24c02IsJobDone() == TRUE))
    {
        if(Swc_Storage_Job == SWC_STORAGE_JOB_SAVE)
        {
            Rte_DisplayRequestStatusLine("SAVE OK");
        }
        else
        {
            /* 读回时按保存格式还原ADC原始值和毫伏值。 */
            adcRaw = Rte_At24c02GetData(0u);
            adcMv = (uint16)(((uint16)Rte_At24c02GetData(1u) << 8) |
                             Rte_At24c02GetData(2u));
            Rte_DisplayRequestReadLine(adcRaw, adcMv);   /* LCD第二行显示读回值 */
            Rte_RequestSerialReadMessage(adcRaw, adcMv); /* 同步请求串口回传 */
        }
        Swc_Storage_Job = SWC_STORAGE_JOB_NONE;
    }
}

/* 执行存储业务：消费命令请求，推进AT24C02任务完成后的响应。 */
void Swc_Storage_MainFunction(void)
{
    if(Rte_ConsumeAdcSaveRequest() == TRUE)
    {
        Swc_Storage_StartSave();
    }
    if(Rte_ConsumeAdcReadRequest() == TRUE)
    {
        Swc_Storage_StartRead();
    }
    Swc_Storage_CheckDone();
}
