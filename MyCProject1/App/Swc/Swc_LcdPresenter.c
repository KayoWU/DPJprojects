#include "Swc_LcdPresenter.h"
#include "Rte.h"

#define SWC_LCD_LINE_LENGTH 16u

static uint8 xdata Swc_LcdPresenter_Line1[SWC_LCD_LINE_LENGTH + 1u];
static uint8 xdata Swc_LcdPresenter_Line2[SWC_LCD_LINE_LENGTH + 1u];
static boolean Swc_LcdPresenter_Line1Dirty = TRUE;
static boolean Swc_LcdPresenter_Line2Dirty = TRUE;

static void Swc_LcdPresenter_ClearLine(uint8 line[])
{
    uint8 i;

    for(i = 0u; i < SWC_LCD_LINE_LENGTH; i++)
    {
        line[i] = ' ';
    }
    line[SWC_LCD_LINE_LENGTH] = '\0';
}

static void Swc_LcdPresenter_Write3Digits(uint8 line[], uint8 pos, uint8 value)
{
    line[pos] = (uint8)(value / 100u + '0');
    line[(uint8)(pos + 1u)] = (uint8)(value / 10u % 10u + '0');
    line[(uint8)(pos + 2u)] = (uint8)(value % 10u + '0');
}

static void Swc_LcdPresenter_WriteVoltage(uint8 line[], uint8 pos, uint16 mv)
{
    line[pos] = (uint8)(mv / 1000u + '0');
    line[(uint8)(pos + 1u)] = '.';
    line[(uint8)(pos + 2u)] = (uint8)(mv / 100u % 10u + '0');
    line[(uint8)(pos + 3u)] = (uint8)(mv / 10u % 10u + '0');
    line[(uint8)(pos + 4u)] = 'V';
}

static uint8 Swc_LcdPresenter_ToHexChar(uint8 value)
{
    value = (uint8)(value & 0x0fu);
    if(value < 10u)
    {
        return (uint8)(value + '0');
    }
    return (uint8)(value - 10u + 'A');
}

static void Swc_LcdPresenter_SetAdcLine(uint8 adcRaw, uint16 adcMv)
{
    /* 第一行固定显示实时ADC，其他业务只抢占第二行。 */
    Swc_LcdPresenter_ClearLine(Swc_LcdPresenter_Line1);
    Swc_LcdPresenter_Line1[0] = 'A';
    Swc_LcdPresenter_Line1[1] = 'D';
    Swc_LcdPresenter_Line1[2] = 'C';
    Swc_LcdPresenter_Line1[3] = ':';
    Swc_LcdPresenter_Write3Digits(Swc_LcdPresenter_Line1, 4u, adcRaw);
    Swc_LcdPresenter_WriteVoltage(Swc_LcdPresenter_Line1, 8u, adcMv);
    Swc_LcdPresenter_Line1Dirty = TRUE;
}

static void Swc_LcdPresenter_SetStatusLine(uint8 text[])
{
    uint8 i;

    Swc_LcdPresenter_ClearLine(Swc_LcdPresenter_Line2);
    for(i = 0u; (i < SWC_LCD_LINE_LENGTH) && (text[i] != '\0'); i++)
    {
        Swc_LcdPresenter_Line2[i] = text[i];
    }
    Swc_LcdPresenter_Line2Dirty = TRUE;
}

static void Swc_LcdPresenter_SetReadLine(uint8 adcRaw, uint16 adcMv)
{
    /* 串口命令2读回EEPROM后，第二行显示保存过的ADC值。 */
    Swc_LcdPresenter_ClearLine(Swc_LcdPresenter_Line2);
    Swc_LcdPresenter_Line2[0] = 'R';
    Swc_LcdPresenter_Line2[1] = 'E';
    Swc_LcdPresenter_Line2[2] = 'A';
    Swc_LcdPresenter_Line2[3] = 'D';
    Swc_LcdPresenter_Line2[4] = ':';
    Swc_LcdPresenter_Write3Digits(Swc_LcdPresenter_Line2, 5u, adcRaw);
    Swc_LcdPresenter_WriteVoltage(Swc_LcdPresenter_Line2, 9u, adcMv);
    Swc_LcdPresenter_Line2Dirty = TRUE;
}

static void Swc_LcdPresenter_SetIrLine(uint8 keyValue)
{
    /* 红外按键显示为两位十六进制，便于和遥控键值表对照。 */
    Swc_LcdPresenter_ClearLine(Swc_LcdPresenter_Line2);
    Swc_LcdPresenter_Line2[0] = 'I';
    Swc_LcdPresenter_Line2[1] = 'R';
    Swc_LcdPresenter_Line2[2] = ' ';
    Swc_LcdPresenter_Line2[3] = 'K';
    Swc_LcdPresenter_Line2[4] = 'E';
    Swc_LcdPresenter_Line2[5] = 'Y';
    Swc_LcdPresenter_Line2[6] = ':';
    Swc_LcdPresenter_Line2[7] = '0';
    Swc_LcdPresenter_Line2[8] = 'x';
    Swc_LcdPresenter_Line2[9] = Swc_LcdPresenter_ToHexChar((uint8)(keyValue >> 4));
    Swc_LcdPresenter_Line2[10] = Swc_LcdPresenter_ToHexChar(keyValue);
    Swc_LcdPresenter_Line2Dirty = TRUE;
}

void Swc_LcdPresenter_Init(void)
{
    Swc_LcdPresenter_ClearLine(Swc_LcdPresenter_Line1);
    Swc_LcdPresenter_ClearLine(Swc_LcdPresenter_Line2);
    Swc_LcdPresenter_SetStatusLine("READY");
    Swc_LcdPresenter_Line1Dirty = TRUE;
    Swc_LcdPresenter_Line2Dirty = TRUE;
}

static void Swc_LcdPresenter_ConsumeRequests(void)
{
    uint8 lineType;
    uint8 adcRaw;
    uint16 adcMv;
    uint8 keyValue;
    uint8 xdata text[SWC_LCD_LINE_LENGTH + 1u];

    if(Rte_DisplayConsumeAdcLine(&adcRaw, &adcMv) == TRUE)
    {
        Swc_LcdPresenter_SetAdcLine(adcRaw, adcMv);
    }

    if(Rte_DisplayConsumeLine2(&lineType, &adcRaw, &adcMv, &keyValue, text) == TRUE)
    {
        if(lineType == RTE_DISPLAY_LINE2_STATUS)
        {
            Swc_LcdPresenter_SetStatusLine(text);
        }
        else if(lineType == RTE_DISPLAY_LINE2_READ_ADC)
        {
            Swc_LcdPresenter_SetReadLine(adcRaw, adcMv);
        }
        else if(lineType == RTE_DISPLAY_LINE2_IR_KEY)
        {
            Swc_LcdPresenter_SetIrLine(keyValue);
        }
    }
}

/* LCD呈现SWC：把各业务的显示请求整理成LCD1602异步刷新。 */
void Swc_LcdPresenter_MainFunction(void)
{
    Swc_LcdPresenter_ConsumeRequests();

    if(Rte_Lcd1602IsBusy() == TRUE)
    {
        /* LCD底层队列还没发完，保持脏标志，下轮再尝试提交。 */
        return;
    }

    if(Swc_LcdPresenter_Line1Dirty == TRUE)
    {
        if(Rte_Lcd1602AsyncShowString(1u, 1u, Swc_LcdPresenter_Line1) == E_OK)
        {
            Swc_LcdPresenter_Line1Dirty = FALSE;
        }
    }
    else if(Swc_LcdPresenter_Line2Dirty == TRUE)
    {
        if(Rte_Lcd1602AsyncShowString(2u, 1u, Swc_LcdPresenter_Line2) == E_OK)
        {
            Swc_LcdPresenter_Line2Dirty = FALSE;
        }
    }
}
