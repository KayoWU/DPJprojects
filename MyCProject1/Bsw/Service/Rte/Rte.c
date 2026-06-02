/* RTE：上层SWC只通过这里访问底层服务，SWC之间的数据和事件也在这里中转。 */

#include "Rte.h"
#include "Serial.h"
#include "Adc.h"
#include "IrSensor.h"
#include "Led8.h"
#include "Lcd1602.h"
#include "At24c02.h"
#include "Timer2.h"
#include "Iic.h"

#define RTE_DISPLAY_TEXT_LENGTH 16u

/* 最新ADC采样值：AdcSampler写入，Storage读取。 */
static uint8 Rte_AdcRaw = 0u;
static uint16 Rte_AdcMv = 0u;

/* 串口命令转换成一次性请求，Storage消费后自动清除。 */
static boolean Rte_SaveRequest = FALSE;
static boolean Rte_ReadRequest = FALSE;

/* LCD第一行显示请求，主要用于实时ADC显示。 */
static boolean Rte_DisplayAdcLinePending = FALSE;
static uint8 Rte_DisplayAdcRaw = 0u;
static uint16 Rte_DisplayAdcMv = 0u;

/* LCD第二行显示请求：状态、EEPROM读回值、红外键值共用这一组端口。 */
static boolean Rte_DisplayLine2Pending = FALSE;
static uint8 Rte_DisplayLine2Type = RTE_DISPLAY_LINE2_NONE;
static uint8 Rte_DisplayLine2Raw = 0u;
static uint16 Rte_DisplayLine2Mv = 0u;
static uint8 Rte_DisplayLine2Key = 0u;
static uint8 xdata Rte_DisplayLine2Text[RTE_DISPLAY_TEXT_LENGTH + 1u];

/* EEPROM读回后，SerialTx消费这组数据并逐字节发送。 */
static boolean Rte_SerialReadMessagePending = FALSE;
static uint8 Rte_SerialReadRaw = 0u;
static uint16 Rte_SerialReadMv = 0u;

static void Rte_CopyText(uint8 destination[], uint8 source[])
{
    uint8 i;

    for(i = 0u; (i < RTE_DISPLAY_TEXT_LENGTH) && (source[i] != '\0'); i++)
    {
        destination[i] = source[i];
    }
    destination[i] = '\0';
}

/* 初始化RTE背后的全部服务，让App不用关心底层初始化顺序。 */
void Rte_Init(void)
{
    Serial_Init();   /* 串口初始化 → Uart_Init */
    Adc_Init();      /* ADC0804风格并口ADC初始化 */
    IrSensor_Init(); /* 红外接收初始化：INT1 + Timer0 */
    Led8_Init();     /* 8位LED锁存输出初始化 */
    Lcd1602_Init();  /* LCD1602初始化 */
    At24c02_Init();  /* IIC EEPROM初始化 */
    Timer2_Init();   /* Timer2周期标志初始化 */
}

/* RTE周期调度：推进所有非阻塞状态机。 */
void Rte_MainFunction(void)
{
    uint16 tick;

    tick = Timer2_GetTick();
    Iic_MainFunction();        /* 软件IIC每轮只走一小步 */
    At24c02_MainFunction(tick);/* EEPROM读写状态机 */
    Lcd1602_MainFunction(tick);/* LCD异步刷新队列 */
    IrSensor_MainFunction();   /* 红外pulse缓冲解码 */
}

/* 获取1ms系统节拍，给App做周期判断。 */
uint16 Rte_GetSystemTick(void)
{
    return Timer2_GetTick();
}

/* 读取串口接收缓存：App只通过RTE拿数据，不直接碰UART寄存器。 */
uint8 Rte_ReadSerialData(void)
{
    return Serial_GetReceivedData();  /* 读一个字节 */
}

/* 清空串口接收缓存：常用于命令处理完成后避免重复执行。 */
void Rte_ClearSerialData(void)
{
    Serial_ClearReceivedData();       /* 清接收缓存 */
}

/* 发送一个串口字节：RTE向下转给Serial，再由Uart写硬件。 */
void Rte_SendSerialByte(uint8 txData)
{
    Serial_SendByte(txData);          /* 发一个字节 */
}

/* 查询串口是否可以发送下一个字节。 */
boolean Rte_IsSerialTxReady(void)
{
    return Serial_IsTxReady();
}

/* 读取ADC原始值：返回0到255的8位采样值。 */
uint8 Rte_ReadAdcValue(void)
{
    return Adc_ReadValue();
}

/* ADC换算：把8位采样值按配置参考电压换算成毫伏。 */
uint16 Rte_AdcConvertToMv(uint8 adcValue)
{
    return Adc_ConvertToMv(adcValue);
}

/* RTE红外接口：App只问“有没有键”，不直接碰外部中断和Timer0。 */
boolean Rte_IrSensorIsKeyReady(void)
{
    return IrSensor_IsKeyReady();
}

/* 读取红外按键值：实际数据来自IrSensor ISR解码结果。 */
uint8 Rte_IrSensorGetKey(void)
{
    return IrSensor_GetKey();
}

/* 清除红外按键就绪标志：表示App已经消费了这次按键。 */
void Rte_IrSensorClearKey(void)
{
    IrSensor_ClearKey();
}

/* 写8位LED：value的二进制位直接对应8个LED的亮灭。 */
void Rte_Led8Write(uint8 value)
{
    Led8_Write(value);
}

/* 异步请求LCD显示字符串：运行阶段推荐使用，不阻塞主循环。 */
Std_ReturnType Rte_Lcd1602AsyncShowString(uint8 row, uint8 column, uint8 text[])
{
    return Lcd1602_AsyncShowString(row, column, text);
}

/* 查询LCD状态机是否还在发送。 */
boolean Rte_Lcd1602IsBusy(void)
{
    return Lcd1602_IsBusy();
}

/* 异步写AT24C02：只发起任务，实际IIC动作由状态机完成。 */
Std_ReturnType Rte_At24c02AsyncWrite(uint8 address, uint8 dataValue[], uint8 length)
{
    return At24c02_AsyncWrite(address, dataValue, length);
}

/* 异步读AT24C02：只发起任务，完成后用GetData取结果。 */
Std_ReturnType Rte_At24c02AsyncRead(uint8 address, uint8 length)
{
    return At24c02_AsyncRead(address, length);
}

boolean Rte_At24c02IsBusy(void)
{
    return At24c02_IsBusy();
}

boolean Rte_At24c02IsJobDone(void)
{
    return At24c02_IsJobDone();
}

uint8 Rte_At24c02GetData(uint8 index)
{
    return At24c02_GetData(index);
}

void Rte_WriteAdcSample(uint8 adcRaw, uint16 adcMv)
{
    Rte_AdcRaw = adcRaw;
    Rte_AdcMv = adcMv;
}

uint8 Rte_ReadAdcRaw(void)
{
    return Rte_AdcRaw;
}

uint16 Rte_ReadAdcMv(void)
{
    return Rte_AdcMv;
}

void Rte_RequestAdcSave(void)
{
    /* Command SWC调用；Storage SWC下一轮消费。 */
    Rte_SaveRequest = TRUE;
}

void Rte_RequestAdcRead(void)
{
    Rte_ReadRequest = TRUE;
}

boolean Rte_ConsumeAdcSaveRequest(void)
{
    if(Rte_SaveRequest == TRUE)
    {
        /* 一次性事件：读到TRUE后立即清除。 */
        Rte_SaveRequest = FALSE;
        return TRUE;
    }
    return FALSE;
}

boolean Rte_ConsumeAdcReadRequest(void)
{
    if(Rte_ReadRequest == TRUE)
    {
        Rte_ReadRequest = FALSE;
        return TRUE;
    }
    return FALSE;
}

void Rte_DisplayRequestAdcLine(uint8 adcRaw, uint16 adcMv)
{
    Rte_DisplayAdcRaw = adcRaw;
    Rte_DisplayAdcMv = adcMv;
    Rte_DisplayAdcLinePending = TRUE;
}

boolean Rte_DisplayConsumeAdcLine(uint8 *adcRaw, uint16 *adcMv)
{
    if(Rte_DisplayAdcLinePending == TRUE)
    {
        *adcRaw = Rte_DisplayAdcRaw;
        *adcMv = Rte_DisplayAdcMv;
        Rte_DisplayAdcLinePending = FALSE;
        return TRUE;
    }
    return FALSE;
}

void Rte_DisplayRequestStatusLine(uint8 text[])
{
    /* 文本先复制到RTE缓存，避免调用方局部变量失效。 */
    Rte_CopyText(Rte_DisplayLine2Text, text);
    Rte_DisplayLine2Type = RTE_DISPLAY_LINE2_STATUS;
    Rte_DisplayLine2Pending = TRUE;
}

void Rte_DisplayRequestReadLine(uint8 adcRaw, uint16 adcMv)
{
    Rte_DisplayLine2Raw = adcRaw;
    Rte_DisplayLine2Mv = adcMv;
    Rte_DisplayLine2Type = RTE_DISPLAY_LINE2_READ_ADC;
    Rte_DisplayLine2Pending = TRUE;
}

void Rte_DisplayRequestIrLine(uint8 keyValue)
{
    Rte_DisplayLine2Key = keyValue;
    Rte_DisplayLine2Type = RTE_DISPLAY_LINE2_IR_KEY;
    Rte_DisplayLine2Pending = TRUE;
}

boolean Rte_DisplayConsumeLine2(uint8 *lineType,
                                uint8 *adcRaw,
                                uint16 *adcMv,
                                uint8 *keyValue,
                                uint8 text[])
{
    if(Rte_DisplayLine2Pending == TRUE)
    {
        *lineType = Rte_DisplayLine2Type;
        *adcRaw = Rte_DisplayLine2Raw;
        *adcMv = Rte_DisplayLine2Mv;
        *keyValue = Rte_DisplayLine2Key;
        Rte_CopyText(text, Rte_DisplayLine2Text); /* 交给LCD Presenter格式化/显示 */
        Rte_DisplayLine2Type = RTE_DISPLAY_LINE2_NONE;
        Rte_DisplayLine2Pending = FALSE;
        return TRUE;
    }
    return FALSE;
}

void Rte_RequestSerialReadMessage(uint8 adcRaw, uint16 adcMv)
{
    /* Storage读完EEPROM后发布，SerialTx负责真正发送。 */
    Rte_SerialReadRaw = adcRaw;
    Rte_SerialReadMv = adcMv;
    Rte_SerialReadMessagePending = TRUE;
}

boolean Rte_ConsumeSerialReadMessage(uint8 *adcRaw, uint16 *adcMv)
{
    if(Rte_SerialReadMessagePending == TRUE)
    {
        *adcRaw = Rte_SerialReadRaw;
        *adcMv = Rte_SerialReadMv;
        Rte_SerialReadMessagePending = FALSE;
        return TRUE;
    }
    return FALSE;
}
