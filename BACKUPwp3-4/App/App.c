/**
 * @file    App.c
 * @brief   New application: ADC display + UART command + AT24C02 storage
 *
 * 运行阶段采用非阻塞思路：
 *   - Timer2提供1ms节拍
 *   - LCD1602通过队列状态机慢慢刷新
 *   - AT24C02通过IIC状态机慢慢读写
 *   - 串口回传文本按“发送空闲”逐字节发送
 */

#include "App.h"
#include "Rte.h"

#define APP_ADC_PERIOD_MS          100u
#define APP_AT24_ADDRESS           0x00u
#define APP_AT24_DATA_LENGTH       3u
#define APP_LCD_LINE_LENGTH        16u
#define APP_TX_BUFFER_LENGTH       32u

#define APP_COMMAND_SAVE_ADC       1u
#define APP_COMMAND_READ_ADC       2u
#define APP_ASCII_COMMAND_SAVE_ADC '1'
#define APP_ASCII_COMMAND_READ_ADC '2'

#define APP_AT24_JOB_NONE          0u
#define APP_AT24_JOB_SAVE          1u
#define APP_AT24_JOB_READ          2u

static uint16 App_NextAdcTick = 0u;
static uint8 App_CurrentAdcRaw = 0u;
static uint16 App_CurrentAdcMv = 0u;
static uint8 App_At24Job = APP_AT24_JOB_NONE;
static boolean App_IsInitialized = FALSE;

static uint8 xdata App_Line1[APP_LCD_LINE_LENGTH + 1u];
static uint8 xdata App_Line2[APP_LCD_LINE_LENGTH + 1u];
static boolean App_Line1Dirty = TRUE;
static boolean App_Line2Dirty = TRUE;

static uint8 xdata App_TxBuffer[APP_TX_BUFFER_LENGTH];
static uint8 App_TxLength = 0u;
static uint8 App_TxIndex = 0u;

static void App_SetStatusLine(uint8 text[]);

/* 把LCD行清成16个空格，最后补字符串结束符。 */
static void App_ClearLine(uint8 line[])
{
    uint8 i;

    for(i = 0u; i < APP_LCD_LINE_LENGTH; i++)
    {
        line[i] = ' ';
    }
    line[APP_LCD_LINE_LENGTH] = '\0';
}

/* App运行期初始化：明确清空xdata显示缓存，避免上电后LCD显示随机内容。 */
static void App_Init(void)
{
    App_ClearLine(App_Line1);
    App_ClearLine(App_Line2);
    App_SetStatusLine("READY");
    App_Line1Dirty = TRUE;
    App_Line2Dirty = TRUE;
    App_IsInitialized = TRUE;
}

/* 在LCD行里写三位十进制数，范围0~255。 */
static void App_Write3Digits(uint8 line[], uint8 pos, uint8 value)
{
    line[pos] = (uint8)(value / 100u + '0');
    line[(uint8)(pos + 1u)] = (uint8)(value / 10u % 10u + '0');
    line[(uint8)(pos + 2u)] = (uint8)(value % 10u + '0');
}

/* 在LCD行里写x.xxV格式的电压。 */
static void App_WriteVoltage(uint8 line[], uint8 pos, uint16 mv)
{
    line[pos] = (uint8)(mv / 1000u + '0');
    line[(uint8)(pos + 1u)] = '.';
    line[(uint8)(pos + 2u)] = (uint8)(mv / 100u % 10u + '0');
    line[(uint8)(pos + 3u)] = (uint8)(mv / 10u % 10u + '0');
    line[(uint8)(pos + 4u)] = 'V';
}

/* LCD第一行显示当前ADC原始值和电压。 */
static void App_UpdateAdcLine(uint8 adcRaw, uint16 adcMv)
{
    App_ClearLine(App_Line1);
    App_Line1[0] = 'A';
    App_Line1[1] = 'D';
    App_Line1[2] = 'C';
    App_Line1[3] = ':';
    App_Write3Digits(App_Line1, 4u, adcRaw);
    App_WriteVoltage(App_Line1, 8u, adcMv);
    App_Line1Dirty = TRUE;
}

/* LCD第二行显示一段16字符以内的状态文本。 */
static void App_SetStatusLine(uint8 text[])
{
    uint8 i;

    App_ClearLine(App_Line2);
    for(i = 0u; (i < APP_LCD_LINE_LENGTH) && (text[i] != '\0'); i++)
    {
        App_Line2[i] = text[i];
    }
    App_Line2Dirty = TRUE;
}

/* LCD第二行显示从EEPROM读出的ADC数据。 */
static void App_SetReadLine(uint8 adcRaw, uint16 adcMv)
{
    App_ClearLine(App_Line2);
    App_Line2[0] = 'R';
    App_Line2[1] = 'E';
    App_Line2[2] = 'A';
    App_Line2[3] = 'D';
    App_Line2[4] = ':';
    App_Write3Digits(App_Line2, 5u, adcRaw);
    App_WriteVoltage(App_Line2, 9u, adcMv);
    App_Line2Dirty = TRUE;
}

/* 准备串口回传文本：后续每轮主循环只发送一个字节。 */
static void App_StartTxReadMessage(uint8 adcRaw, uint16 adcMv)
{
    uint8 i;

    for(i = 0u; i < APP_TX_BUFFER_LENGTH; i++)
    {
        App_TxBuffer[i] = '\0';
    }

    App_TxBuffer[0] = 'R';
    App_TxBuffer[1] = 'E';
    App_TxBuffer[2] = 'A';
    App_TxBuffer[3] = 'D';
    App_TxBuffer[4] = ' ';
    App_TxBuffer[5] = 'A';
    App_TxBuffer[6] = 'D';
    App_TxBuffer[7] = 'C';
    App_TxBuffer[8] = '=';
    App_TxBuffer[9] = (uint8)(adcRaw / 100u + '0');
    App_TxBuffer[10] = (uint8)(adcRaw / 10u % 10u + '0');
    App_TxBuffer[11] = (uint8)(adcRaw % 10u + '0');
    App_TxBuffer[12] = ' ';
    App_TxBuffer[13] = 'M';
    App_TxBuffer[14] = 'V';
    App_TxBuffer[15] = '=';
    App_TxBuffer[16] = (uint8)(adcMv / 1000u + '0');
    App_TxBuffer[17] = (uint8)(adcMv / 100u % 10u + '0');
    App_TxBuffer[18] = (uint8)(adcMv / 10u % 10u + '0');
    App_TxBuffer[19] = (uint8)(adcMv % 10u + '0');
    App_TxBuffer[20] = '\r';
    App_TxBuffer[21] = '\n';
    App_TxLength = 22u;
    App_TxIndex = 0u;
}

/* 串口发送状态机：发送空闲时才塞入下一个字节。 */
static void App_TxMainFunction(void)
{
    if((App_TxIndex < App_TxLength) && (Rte_IsSerialTxReady() == TRUE))
    {
        Rte_SendSerialByte(App_TxBuffer[App_TxIndex]);
        App_TxIndex++;
    }
}

/* 周期采样ADC并刷新LCD第一行缓存。 */
static void App_AdcMainFunction(uint16 tick)
{
    if((sint16)(tick - App_NextAdcTick) >= 0)
    {
        App_CurrentAdcRaw = Rte_ReadAdcValue();
        App_CurrentAdcMv = Rte_AdcConvertToMv(App_CurrentAdcRaw);
        App_UpdateAdcLine(App_CurrentAdcRaw, App_CurrentAdcMv);
        App_NextAdcTick = (uint16)(tick + APP_ADC_PERIOD_MS);
    }
}

/* 处理串口命令：1保存当前ADC，2读取EEPROM中的ADC。 */
static void App_CommandMainFunction(void)
{
    uint8 command;
    uint8 writeData[APP_AT24_DATA_LENGTH];

    command = Rte_ReadSerialData();
    if((command == APP_COMMAND_SAVE_ADC) || (command == APP_ASCII_COMMAND_SAVE_ADC))
    {
        Rte_ClearSerialData();
        if(Rte_At24c02IsBusy() == FALSE)
        {
            writeData[0] = App_CurrentAdcRaw;
            writeData[1] = (uint8)(App_CurrentAdcMv >> 8);
            writeData[2] = (uint8)App_CurrentAdcMv;
            if(Rte_At24c02AsyncWrite(APP_AT24_ADDRESS, writeData, APP_AT24_DATA_LENGTH) == E_OK)
            {
                App_At24Job = APP_AT24_JOB_SAVE;
                App_SetStatusLine("SAVING...");
            }
        }
        else
        {
            App_SetStatusLine("AT24 BUSY");
        }
    }
    else if((command == APP_COMMAND_READ_ADC) || (command == APP_ASCII_COMMAND_READ_ADC))
    {
        Rte_ClearSerialData();
        if(Rte_At24c02IsBusy() == FALSE)
        {
            if(Rte_At24c02AsyncRead(APP_AT24_ADDRESS, APP_AT24_DATA_LENGTH) == E_OK)
            {
                App_At24Job = APP_AT24_JOB_READ;
                App_SetStatusLine("READING...");
            }
        }
        else
        {
            App_SetStatusLine("AT24 BUSY");
        }
    }
}

/* 处理AT24C02异步任务完成后的显示和串口回传。 */
static void App_At24MainFunction(void)
{
    uint8 adcRaw;
    uint16 adcMv;

    if((App_At24Job != APP_AT24_JOB_NONE) && (Rte_At24c02IsJobDone() == TRUE))
    {
        if(App_At24Job == APP_AT24_JOB_SAVE)
        {
            App_SetStatusLine("SAVE OK");
        }
        else
        {
            adcRaw = Rte_At24c02GetData(0u);
            adcMv = (uint16)(((uint16)Rte_At24c02GetData(1u) << 8) |
                             Rte_At24c02GetData(2u));
            App_SetReadLine(adcRaw, adcMv);
            App_StartTxReadMessage(adcRaw, adcMv);
        }
        App_At24Job = APP_AT24_JOB_NONE;
    }
}

/* 尝试把脏行提交给LCD状态机；LCD忙时下轮再试。 */
static void App_LcdMainFunction(void)
{
    if(Rte_Lcd1602IsBusy() == TRUE)
    {
        return;
    }
    if(App_Line1Dirty == TRUE)
    {
        if(Rte_Lcd1602AsyncShowString(1u, 1u, App_Line1) == E_OK)
        {
            App_Line1Dirty = FALSE;
        }
    }
    else if(App_Line2Dirty == TRUE)
    {
        if(Rte_Lcd1602AsyncShowString(2u, 1u, App_Line2) == E_OK)
        {
            App_Line2Dirty = FALSE;
        }
    }
}

/* 应用层周期任务：先推进底层状态机，再执行ADC、命令、存储、显示和串口发送。 */
void App_MainFunction(void)
{
    uint16 tick;

    Rte_MainFunction();
    tick = Rte_GetSystemTick();

    if(App_IsInitialized == FALSE)
    {
        App_Init();
    }

    App_AdcMainFunction(tick);
    App_CommandMainFunction();
    App_At24MainFunction();
    App_LcdMainFunction();
    App_TxMainFunction();
}
