#include "Iic.h"

sbit Iic_Sda = P2^0;
sbit Iic_Scl = P2^1;

#define IIC_STATE_IDLE        0u
#define IIC_STATE_DONE        1u
#define IIC_STATE_START_0     2u
#define IIC_STATE_START_1     3u
#define IIC_STATE_START_2     4u
#define IIC_STATE_STOP_0      5u
#define IIC_STATE_STOP_1      6u
#define IIC_STATE_STOP_2      7u
#define IIC_STATE_ACK         8u
#define IIC_STATE_WRITE_LOW   9u
#define IIC_STATE_WRITE_HIGH  10u
#define IIC_STATE_READ_LOW    11u
#define IIC_STATE_READ_HIGH   12u
#define IIC_STATE_NACK_LOW    13u
#define IIC_STATE_NACK_HIGH   14u

static uint8 Iic_State = IIC_STATE_IDLE;
static uint8 Iic_CurrentByte = 0u;
static uint8 Iic_ReadData = 0u;
static uint8 Iic_BitIndex = 0u;
static uint8 Iic_AckCount = 0u;

/* IIC总线初始化：SDA和SCL都拉高，表示总线空闲。 */
void Iic_Init(void)
{
    Iic_Sda = 1;
    _nop_();
    Iic_Scl = 1;
    _nop_();
    Iic_State = IIC_STATE_IDLE;
}

/* 启动一个IIC小操作：起始、停止、等ACK、写1字节、读1字节。 */
Std_ReturnType Iic_StartOperation(uint8 operation, uint8 dataValue)
{
    if(Iic_IsBusy() == TRUE)
    {
        return E_NOT_OK;
    }

    Iic_CurrentByte = dataValue;
    Iic_ReadData = 0u;
    Iic_BitIndex = 0u;
    Iic_AckCount = 0u;

    if(operation == IIC_OPERATION_START)
    {
        Iic_State = IIC_STATE_START_0;
    }
    else if(operation == IIC_OPERATION_STOP)
    {
        Iic_State = IIC_STATE_STOP_0;
    }
    else if(operation == IIC_OPERATION_WAIT_ACK)
    {
        Iic_State = IIC_STATE_ACK;
    }
    else if(operation == IIC_OPERATION_WRITE_BYTE)
    {
        Iic_State = IIC_STATE_WRITE_LOW;
    }
    else if(operation == IIC_OPERATION_READ_BYTE)
    {
        Iic_State = IIC_STATE_READ_LOW;
    }
    else if(operation == IIC_OPERATION_SEND_NACK)
    {
        Iic_State = IIC_STATE_NACK_LOW;
    }
    else
    {
        Iic_State = IIC_STATE_IDLE;
        return E_NOT_OK;
    }
    return E_OK;
}

/* IIC运行期状态机：每次只推进一个很小的总线动作。 */
void Iic_MainFunction(void)
{
    switch(Iic_State)
    {
        case IIC_STATE_START_0:
            Iic_Sda = 1;
            Iic_Scl = 1;
            Iic_State = IIC_STATE_START_1;
            break;
        case IIC_STATE_START_1:
            Iic_Sda = 0;
            Iic_State = IIC_STATE_START_2;
            break;
        case IIC_STATE_START_2:
            Iic_Scl = 0;
            Iic_State = IIC_STATE_DONE;
            break;
        case IIC_STATE_STOP_0:
            Iic_Sda = 0;
            Iic_State = IIC_STATE_STOP_1;
            break;
        case IIC_STATE_STOP_1:
            Iic_Scl = 1;
            Iic_State = IIC_STATE_STOP_2;
            break;
        case IIC_STATE_STOP_2:
            Iic_Sda = 1;
            Iic_State = IIC_STATE_DONE;
            break;
        case IIC_STATE_ACK:
            Iic_Scl = 1;
            if((Iic_Sda == 0) || (Iic_AckCount >= 250u))
            {
                Iic_Scl = 0;
                Iic_State = IIC_STATE_DONE;
            }
            else
            {
                Iic_AckCount++;
            }
            break;
        case IIC_STATE_WRITE_LOW:
            Iic_Scl = 0;
            if((Iic_CurrentByte & 0x80u) != 0u)
            {
                Iic_Sda = 1;
            }
            else
            {
                Iic_Sda = 0;
            }
            Iic_State = IIC_STATE_WRITE_HIGH;
            break;
        case IIC_STATE_WRITE_HIGH:
            Iic_Scl = 1;
            Iic_CurrentByte = (uint8)(Iic_CurrentByte << 1);
            Iic_BitIndex++;
            if(Iic_BitIndex >= 8u)
            {
                Iic_Scl = 0;
                Iic_Sda = 1;
                Iic_State = IIC_STATE_DONE;
            }
            else
            {
                Iic_State = IIC_STATE_WRITE_LOW;
            }
            break;
        case IIC_STATE_READ_LOW:
            Iic_Scl = 0;
            Iic_Sda = 1;
            Iic_State = IIC_STATE_READ_HIGH;
            break;
        case IIC_STATE_READ_HIGH:
            Iic_Scl = 1;
            Iic_ReadData = (uint8)((Iic_ReadData << 1) | Iic_Sda);
            Iic_BitIndex++;
            if(Iic_BitIndex >= 8u)
            {
                Iic_Scl = 0;
                Iic_State = IIC_STATE_DONE;
            }
            else
            {
                Iic_State = IIC_STATE_READ_LOW;
            }
            break;
        case IIC_STATE_NACK_LOW:
            Iic_Scl = 0;
            Iic_Sda = 1;
            Iic_State = IIC_STATE_NACK_HIGH;
            break;
        case IIC_STATE_NACK_HIGH:
            Iic_Scl = 1;
            Iic_Scl = 0;
            Iic_State = IIC_STATE_DONE;
            break;
        default:
            break;
    }
}

/* IIC是否还在执行小操作。 */
boolean Iic_IsBusy(void)
{
    if((Iic_State != IIC_STATE_IDLE) && (Iic_State != IIC_STATE_DONE))
    {
        return TRUE;
    }
    return FALSE;
}

/* IIC小操作是否已完成。读取后不会自动清除，由下一次StartOperation覆盖。 */
boolean Iic_IsDone(void)
{
    if(Iic_State == IIC_STATE_DONE)
    {
        return TRUE;
    }
    return FALSE;
}

/* 取得最近一次读字节操作的结果。 */
uint8 Iic_GetReadData(void)
{
    return Iic_ReadData;
}
