#include "IrSensor.h"

/* NEC一帧共4字节：地址码、地址反码、命令码、命令反码。 */
static volatile uint8 IrSensor_Frame[4] = {0u, 0u, 0u, 0u};

/* 当前已经接收到第几位，收满32位后校验命令码。 */
static volatile uint8 IrSensor_BitCount = 0u;

/* TRUE表示已经识别到起始码，后续下降沿开始按0/1解析。 */
static volatile boolean IrSensor_Receiving = FALSE;

/* ISR置位，App读取并清除；这样中断层和应用层解耦。 */
static volatile boolean IrSensor_KeyReady = FALSE;
static volatile uint8 IrSensor_KeyValue = 0u;
/* 第一次边沿只建立计时基准，不参与协议判断。 */
static volatile boolean IrSensor_FirstEdgeSeen = FALSE;

/* pulse环形缓冲区：ISR写入，MainFunction读取。 */
static volatile uint16 xdata IrSensor_PulseBuffer[IRSENSOR_PULSE_BUFFER_SIZE];
static volatile uint8 IrSensor_PulseWriteIndex = 0u;
static volatile uint8 IrSensor_PulseReadIndex = 0u;
static volatile boolean IrSensor_PulseOverflow = FALSE;

/* ISR里调用：把一次脉宽计数放入环形缓冲区。 */
static void IrSensor_PushPulse(uint16 pulseCount)
{
    uint8 nextWriteIndex;

    nextWriteIndex = (uint8)(IrSensor_PulseWriteIndex + 1u);
    if(nextWriteIndex >= IRSENSOR_PULSE_BUFFER_SIZE)
    {
        nextWriteIndex = 0u;
    }

    if(nextWriteIndex == IrSensor_PulseReadIndex)
    {
        IrSensor_PulseOverflow = TRUE; /* 缓冲区满，丢弃这次pulse，让主循环重置解码。 */
    }
    else
    {
        IrSensor_PulseBuffer[IrSensor_PulseWriteIndex] = pulseCount;
        IrSensor_PulseWriteIndex = nextWriteIndex;
    }
}

/* 主循环里调用：从环形缓冲区取出一个pulse。 */
static Std_ReturnType IrSensor_PopPulse(uint16 *pulseCount)
{
    boolean savedEa;

    savedEa = EA;
    EA = 0;
    /* 读写索引会被ISR修改，取pulse时短暂关中断保护。 */
    if(IrSensor_PulseReadIndex == IrSensor_PulseWriteIndex)
    {
        EA = savedEa;
        return E_NOT_OK;
    }

    *pulseCount = IrSensor_PulseBuffer[IrSensor_PulseReadIndex];
    IrSensor_PulseReadIndex++;
    if(IrSensor_PulseReadIndex >= IRSENSOR_PULSE_BUFFER_SIZE)
    {
        IrSensor_PulseReadIndex = 0u;
    }
    EA = savedEa;

    return E_OK;
}

/* 解码一个pulse：判断起始码、0、1，并在收满32位后生成按键值。 */
static void IrSensor_DecodePulse(uint16 pulseCount)
{
    /* 起始码时间最长，识别到它就认为一帧新数据开始。 */
    if((pulseCount > IRSENSOR_START_MIN_COUNT) &&
       (pulseCount < IRSENSOR_START_MAX_COUNT))
    {
        IrSensor_BitCount = 0u;
        IrSensor_Receiving = TRUE;
        IrSensor_Frame[0] = 0u;
        IrSensor_Frame[1] = 0u;
        IrSensor_Frame[2] = 0u;
        IrSensor_Frame[3] = 0u;
        return;
    }

    if(IrSensor_Receiving == TRUE)
    {
        /* 脉宽较长认为是1：右移一位，再把最高位置1。 */
        if((pulseCount > IRSENSOR_BIT_SPLIT_COUNT) &&
           (pulseCount < IRSENSOR_BIT1_MAX_COUNT))
        {
            IrSensor_Frame[IrSensor_BitCount / 8u] =
                (uint8)((IrSensor_Frame[IrSensor_BitCount / 8u] >> 1) | 0x80u);
            IrSensor_BitCount++;
        }
        /* 脉宽较短认为是0：只右移，不置最高位。 */
        else if((pulseCount > IRSENSOR_BIT0_MIN_COUNT) &&
                (pulseCount < IRSENSOR_BIT_SPLIT_COUNT))
        {
            IrSensor_Frame[IrSensor_BitCount / 8u] =
                (uint8)(IrSensor_Frame[IrSensor_BitCount / 8u] >> 1);
            IrSensor_BitCount++;
        }

        /* NEC完整帧为32位，命令码和命令反码互补才算有效。 */
        if(IrSensor_BitCount >= 32u)
        {
            IrSensor_BitCount = 0u;
            IrSensor_Receiving = FALSE;
            if(IrSensor_Frame[2] == (uint8)(~IrSensor_Frame[3]))
            {
                IrSensor_KeyValue = IrSensor_Frame[2];
                IrSensor_KeyReady = TRUE;
            }
        }
    }
}

/* 红外接收初始化：INT1下降沿触发，Timer0用于测量两个下降沿之间的时间。 */
void IrSensor_Init(void)
{
    IrSensor_BitCount = 0u;
    IrSensor_Receiving = FALSE;
    IrSensor_KeyReady = FALSE;
    IrSensor_KeyValue = 0u;
    IrSensor_FirstEdgeSeen = FALSE;
    IrSensor_PulseWriteIndex = 0u;
    IrSensor_PulseReadIndex = 0u;
    IrSensor_PulseOverflow = FALSE;

    IT1 = 1;                       /* INT1下降沿触发：红外接收头输出的脉冲边沿 */
    EX1 = 1;                       /* 允许外部中断1 */
    TMOD = (TMOD & 0xf0u) | 0x01u; /* 只改Timer0低4位，避免影响UART使用的Timer1 */
    TH0 = 0u;
    TL0 = 0u;
    TR0 = 1;                       /* 启动Timer0计脉宽 */
    EA = 1;
}

/* 周期解码：把中断采集到的pulse逐个取出，在主循环上下文里完成NEC解码。 */
void IrSensor_MainFunction(void)
{
    uint16 pulseCount;

    if(IrSensor_PulseOverflow == TRUE)
    {
        /* 丢过pulse后本帧已经不可靠，丢弃当前解码进度。 */
        IrSensor_PulseOverflow = FALSE;
        IrSensor_Receiving = FALSE;
        IrSensor_BitCount = 0u;
    }

    while(IrSensor_PopPulse(&pulseCount) == E_OK)
    {
        IrSensor_DecodePulse(pulseCount);
    }
}

/* 是否已经收到一个校验正确的红外按键值。 */
boolean IrSensor_IsKeyReady(void)
{
    return IrSensor_KeyReady;
}

/* 读取最近一次红外按键值。 */
uint8 IrSensor_GetKey(void)
{
    uint8 keyValue;
    boolean savedEa;

    savedEa = EA;                  /* 按键值由ISR写入，读取时短暂关中断防止被改写 */
    EA = 0;
    keyValue = IrSensor_KeyValue;
    EA = savedEa;

    return keyValue;
}

/* 清除按键就绪标志，表示App已经消费了这次按键。 */
void IrSensor_ClearKey(void)
{
    boolean savedEa;

    savedEa = EA;                  /* 标志位由ISR置位，清除时也做临界区保护 */
    EA = 0;
    IrSensor_KeyReady = FALSE;
    EA = savedEa;
}

/* INT1中断：只测量脉宽并入队，复杂解码放到主循环里做。 */
void IrSensor_Isr(void) interrupt 2 using 1
{
    uint16 pulseCount;

    pulseCount = (uint16)((uint16)TH0 * 256u + TL0); /* 两次下降沿之间的Timer0计数 */
    TH0 = 0u;
    TL0 = 0u;

    /*
     * 第一次下降沿只用来建立计时基准。
     * 因为Timer0在初始化后已经开始计数，第一次读到的不是红外协议脉宽，
     * 如果直接参与判断，可能刚好落入起始码范围而误判。
     */
    if(IrSensor_FirstEdgeSeen == FALSE)
    {
        IrSensor_FirstEdgeSeen = TRUE;
        return;
    }

    IrSensor_PushPulse(pulseCount);
}
