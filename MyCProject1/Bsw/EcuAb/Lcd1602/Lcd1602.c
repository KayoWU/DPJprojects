#include "Lcd1602.h"
#include "Delay.h"

#define LCD1602_DATA_PORT P0
#define LCD1602_QUEUE_SIZE  24u
#define LCD1602_OP_COMMAND  0u
#define LCD1602_OP_DATA     1u

sbit Lcd1602_Rs = P3^5;
sbit Lcd1602_En = P3^4;

static uint8 xdata Lcd1602_QueueType[LCD1602_QUEUE_SIZE];
static uint8 xdata Lcd1602_QueueData[LCD1602_QUEUE_SIZE];
/* 队列里保存“设置地址 + 多个数据字节”，MainFunction每1ms发送一步。 */
static uint8 Lcd1602_QueueLength = 0u;
static uint8 Lcd1602_QueueIndex = 0u;
static uint16 Lcd1602_NextTick = 0u;

/* 运行期低层写入：只翻转引脚，不使用Delay，节奏交给MainFunction控制。 */
static void Lcd1602_WriteRaw(uint8 type, uint8 dataValue)
{
    Lcd1602_En = 0;
    if(type == LCD1602_OP_COMMAND)
    {
        Lcd1602_Rs = 0; /* RS=0：写命令 */
    }
    else
    {
        Lcd1602_Rs = 1; /* RS=1：写显示数据 */
    }
    LCD1602_DATA_PORT = dataValue;
    Lcd1602_En = 1;
    _nop_();
    Lcd1602_En = 0;
}

/* 把一次LCD操作放进队列：队列由MainFunction慢慢发送。 */
static Std_ReturnType Lcd1602_QueuePush(uint8 type, uint8 dataValue)
{
    if(Lcd1602_QueueLength >= LCD1602_QUEUE_SIZE)
    {
        return E_NOT_OK;
    }
    Lcd1602_QueueType[Lcd1602_QueueLength] = type;
    Lcd1602_QueueData[Lcd1602_QueueLength] = dataValue;
    Lcd1602_QueueLength++;
    return E_OK;
}

/* 根据行列计算LCD内部DDRAM地址，row和column都从1开始。 */
static uint8 Lcd1602_MakeAddress(uint8 row, uint8 column)
{
    if(row == 1u)
    {
        return (uint8)(0x80u + column - 1u);
    }
    return (uint8)(0x80u + 0x40u + column - 1u);
}

/* 写LCD1602命令：RS=0表示当前总线上放的是控制命令。 */
void Lcd1602_WriteCommand(uint8 command)
{
    Lcd1602_En = 0;
    Lcd1602_Rs = 0;
    LCD1602_DATA_PORT = command;
    Lcd1602_En = 1;
    Delay_WaitMs(1u); /* 只在初始化阶段使用阻塞等待，运行期不用它。 */
    Lcd1602_En = 0;
}

/* LCD1602初始化：配置显示模式、开显示、光标移动方式，并清屏。 */
void Lcd1602_Init(void)
{
    Lcd1602_WriteCommand(0x38u);
    Delay_WaitMs(40u);
    Lcd1602_WriteCommand(0x0cu);
    Delay_WaitMs(40u);
    Lcd1602_WriteCommand(0x06u);
    Delay_WaitMs(40u);
    Lcd1602_WriteCommand(0x01u);
    Delay_WaitMs(40u);
    Lcd1602_QueueLength = 0u;
    Lcd1602_QueueIndex = 0u;
}

/* 请求异步显示字符串：只入队，不等待LCD完成。 */
Std_ReturnType Lcd1602_AsyncShowString(uint8 row, uint8 column, uint8 text[])
{
    uint8 i;

    if((row != 1u && row != 2u) || column < 1u || column > 40u)
    {
        return E_NOT_OK;
    }
    if(Lcd1602_IsBusy() == TRUE)
    {
        /* 当前队列未发完时拒绝新请求，由上层下轮再提交。 */
        return E_NOT_OK;
    }

    /* 一次显示请求先设置DDRAM地址，再排入所有字符。 */
    Lcd1602_QueueLength = 0u;
    Lcd1602_QueueIndex = 0u;
    if(Lcd1602_QueuePush(LCD1602_OP_COMMAND, Lcd1602_MakeAddress(row, column)) != E_OK)
    {
        return E_NOT_OK;
    }
    for(i = 0u; text[i] != '\0'; i++)
    {
        if(Lcd1602_QueuePush(LCD1602_OP_DATA, text[i]) != E_OK)
        {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

/* LCD运行期状态机：每到1ms发送队列中的一步，避免阻塞主循环。 */
void Lcd1602_MainFunction(uint16 tick)
{
    if(Lcd1602_QueueIndex >= Lcd1602_QueueLength)
    {
        return;
    }
    if((sint16)(tick - Lcd1602_NextTick) < 0)
    {
        /* LCD需要一点执行时间，所以两次写入之间至少隔1ms。 */
        return;
    }

    Lcd1602_WriteRaw(Lcd1602_QueueType[Lcd1602_QueueIndex],
                     Lcd1602_QueueData[Lcd1602_QueueIndex]);
    Lcd1602_QueueIndex++;
    Lcd1602_NextTick = (uint16)(tick + 1u);
    if(Lcd1602_QueueIndex >= Lcd1602_QueueLength)
    {
        /* 队列发完后清空，表示LCD状态机空闲。 */
        Lcd1602_QueueLength = 0u;
        Lcd1602_QueueIndex = 0u;
    }
}

/* 查询LCD是否还有待发送内容。 */
boolean Lcd1602_IsBusy(void)
{
    if(Lcd1602_QueueLength == 0u)
    {
        return FALSE;
    }
    return TRUE;
}
