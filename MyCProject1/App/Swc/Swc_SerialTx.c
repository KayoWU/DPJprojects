#include "Swc_SerialTx.h"
#include "Rte.h"

#define SWC_SERIAL_TX_BUFFER_LENGTH 32u

static uint8 xdata Swc_SerialTx_Buffer[SWC_SERIAL_TX_BUFFER_LENGTH];
static uint8 Swc_SerialTx_Length = 0u;
static uint8 Swc_SerialTx_Index = 0u;

void Swc_SerialTx_Init(void)
{
    Swc_SerialTx_Length = 0u;
    Swc_SerialTx_Index = 0u;
}

static void Swc_SerialTx_StartReadMessage(uint8 adcRaw, uint16 adcMv)
{
    uint8 i;

    for(i = 0u; i < SWC_SERIAL_TX_BUFFER_LENGTH; i++)
    {
        Swc_SerialTx_Buffer[i] = '\0';
    }

    /* 固定格式回传，便于上位机直接观察或解析。 */
    Swc_SerialTx_Buffer[0] = 'R';
    Swc_SerialTx_Buffer[1] = 'E';
    Swc_SerialTx_Buffer[2] = 'A';
    Swc_SerialTx_Buffer[3] = 'D';
    Swc_SerialTx_Buffer[4] = ' ';
    Swc_SerialTx_Buffer[5] = 'A';
    Swc_SerialTx_Buffer[6] = 'D';
    Swc_SerialTx_Buffer[7] = 'C';
    Swc_SerialTx_Buffer[8] = '=';
    Swc_SerialTx_Buffer[9] = (uint8)(adcRaw / 100u + '0');
    Swc_SerialTx_Buffer[10] = (uint8)(adcRaw / 10u % 10u + '0');
    Swc_SerialTx_Buffer[11] = (uint8)(adcRaw % 10u + '0');
    Swc_SerialTx_Buffer[12] = ' ';
    Swc_SerialTx_Buffer[13] = 'M';
    Swc_SerialTx_Buffer[14] = 'V';
    Swc_SerialTx_Buffer[15] = '=';
    Swc_SerialTx_Buffer[16] = (uint8)(adcMv / 1000u + '0');
    Swc_SerialTx_Buffer[17] = (uint8)(adcMv / 100u % 10u + '0');
    Swc_SerialTx_Buffer[18] = (uint8)(adcMv / 10u % 10u + '0');
    Swc_SerialTx_Buffer[19] = (uint8)(adcMv % 10u + '0');
    Swc_SerialTx_Buffer[20] = '\r';
    Swc_SerialTx_Buffer[21] = '\n';
    Swc_SerialTx_Length = 22u;
    Swc_SerialTx_Index = 0u;
}

/* 串口发送SWC：消费发送请求，空闲时逐字节发出。 */
void Swc_SerialTx_MainFunction(void)
{
    uint8 adcRaw;
    uint16 adcMv;

    if(Rte_ConsumeSerialReadMessage(&adcRaw, &adcMv) == TRUE)
    {
        /* 新的读回结果会覆盖旧发送缓存，从头开始发。 */
        Swc_SerialTx_StartReadMessage(adcRaw, adcMv);
    }

    if((Swc_SerialTx_Index < Swc_SerialTx_Length) && (Rte_IsSerialTxReady() == TRUE))
    {
        Rte_SendSerialByte(Swc_SerialTx_Buffer[Swc_SerialTx_Index]);
        Swc_SerialTx_Index++;
    }
}
