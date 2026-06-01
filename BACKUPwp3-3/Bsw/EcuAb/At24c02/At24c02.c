#include "At24c02.h"
#include "Iic.h"

#define AT24C02_BUFFER_SIZE       8u
#define AT24C02_STATE_IDLE        0u
#define AT24C02_STATE_START_REQ   1u
#define AT24C02_STATE_START_WAIT  2u
#define AT24C02_STATE_DEVW_REQ    3u
#define AT24C02_STATE_DEVW_WAIT   4u
#define AT24C02_STATE_ACK_REQ     5u
#define AT24C02_STATE_ACK_WAIT    6u
#define AT24C02_STATE_ADDR_REQ    7u
#define AT24C02_STATE_ADDR_WAIT   8u
#define AT24C02_STATE_DATA_REQ    9u
#define AT24C02_STATE_DATA_WAIT   10u
#define AT24C02_STATE_RSTART_REQ  11u
#define AT24C02_STATE_RSTART_WAIT 12u
#define AT24C02_STATE_DEVR_REQ    13u
#define AT24C02_STATE_DEVR_WAIT   14u
#define AT24C02_STATE_READ_REQ    15u
#define AT24C02_STATE_READ_WAIT   16u
#define AT24C02_STATE_STOP_REQ    17u
#define AT24C02_STATE_STOP_WAIT   18u
#define AT24C02_STATE_WRITE_WAIT  19u
#define AT24C02_STATE_DONE        20u
#define AT24C02_STATE_NACK_REQ    21u
#define AT24C02_STATE_NACK_WAIT   22u

#define AT24C02_JOB_NONE          0u
#define AT24C02_JOB_WRITE         1u
#define AT24C02_JOB_READ          2u

static uint8 At24c02_State = AT24C02_STATE_IDLE;
static uint8 At24c02_Job = AT24C02_JOB_NONE;
static uint8 At24c02_Address = 0u;
static uint8 xdata At24c02_Buffer[AT24C02_BUFFER_SIZE];
static uint8 At24c02_Length = 0u;
static uint8 At24c02_Index = 0u;
static uint8 At24c02_AfterAckState = AT24C02_STATE_IDLE;
static uint16 At24c02_WaitUntil = 0u;

/* AT24C02初始化：先把软件IIC总线释放到空闲高电平。 */
void At24c02_Init(void)
{
    Iic_Init();
    At24c02_State = AT24C02_STATE_IDLE;
    At24c02_Job = AT24C02_JOB_NONE;
}

/* 异步写入：复制数据后立即返回，实际IIC动作由MainFunction推进。 */
Std_ReturnType At24c02_AsyncWrite(uint8 address, uint8 dataValue[], uint8 length)
{
    uint8 i;

    if((At24c02_IsBusy() == TRUE) || (length == 0u) || (length > AT24C02_BUFFER_SIZE))
    {
        return E_NOT_OK;
    }
    for(i = 0u; i < length; i++)
    {
        At24c02_Buffer[i] = dataValue[i];
    }
    At24c02_Address = address;
    At24c02_Length = length;
    At24c02_Index = 0u;
    At24c02_Job = AT24C02_JOB_WRITE;
    At24c02_State = AT24C02_STATE_START_REQ;
    return E_OK;
}

/* 异步读取：只发起任务，读取结果后续用At24c02_GetData取得。 */
Std_ReturnType At24c02_AsyncRead(uint8 address, uint8 length)
{
    if((At24c02_IsBusy() == TRUE) || (length == 0u) || (length > AT24C02_BUFFER_SIZE))
    {
        return E_NOT_OK;
    }
    At24c02_Address = address;
    At24c02_Length = length;
    At24c02_Index = 0u;
    At24c02_Job = AT24C02_JOB_READ;
    At24c02_State = AT24C02_STATE_START_REQ;
    return E_OK;
}

/* AT24C02任务状态机：把一次读写拆成多个IIC小操作。 */
void At24c02_MainFunction(uint16 tick)
{
    switch(At24c02_State)
    {
        case AT24C02_STATE_START_REQ:
            if(Iic_StartOperation(IIC_OPERATION_START, 0u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_START_WAIT;
            }
            break;
        case AT24C02_STATE_START_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_State = AT24C02_STATE_DEVW_REQ;
            }
            break;
        case AT24C02_STATE_DEVW_REQ:
            if(Iic_StartOperation(IIC_OPERATION_WRITE_BYTE, 0xa0u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_DEVW_WAIT;
            }
            break;
        case AT24C02_STATE_DEVW_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_AfterAckState = AT24C02_STATE_ADDR_REQ;
                At24c02_State = AT24C02_STATE_ACK_REQ;
            }
            break;
        case AT24C02_STATE_ACK_REQ:
            if(Iic_StartOperation(IIC_OPERATION_WAIT_ACK, 0u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_ACK_WAIT;
            }
            break;
        case AT24C02_STATE_ACK_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_State = At24c02_AfterAckState;
            }
            break;
        case AT24C02_STATE_ADDR_REQ:
            if(Iic_StartOperation(IIC_OPERATION_WRITE_BYTE, (uint8)(At24c02_Address + At24c02_Index)) == E_OK)
            {
                At24c02_State = AT24C02_STATE_ADDR_WAIT;
            }
            break;
        case AT24C02_STATE_ADDR_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                if(At24c02_Job == AT24C02_JOB_WRITE)
                {
                    At24c02_AfterAckState = AT24C02_STATE_DATA_REQ;
                }
                else
                {
                    At24c02_AfterAckState = AT24C02_STATE_RSTART_REQ;
                }
                At24c02_State = AT24C02_STATE_ACK_REQ;
            }
            break;
        case AT24C02_STATE_DATA_REQ:
            if(Iic_StartOperation(IIC_OPERATION_WRITE_BYTE, At24c02_Buffer[At24c02_Index]) == E_OK)
            {
                At24c02_State = AT24C02_STATE_DATA_WAIT;
            }
            break;
        case AT24C02_STATE_DATA_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_Index++;
                if(At24c02_Index >= At24c02_Length)
                {
                    At24c02_AfterAckState = AT24C02_STATE_STOP_REQ;
                }
                else
                {
                    At24c02_AfterAckState = AT24C02_STATE_DATA_REQ;
                }
                At24c02_State = AT24C02_STATE_ACK_REQ;
            }
            break;
        case AT24C02_STATE_RSTART_REQ:
            if(Iic_StartOperation(IIC_OPERATION_START, 0u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_RSTART_WAIT;
            }
            break;
        case AT24C02_STATE_RSTART_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_State = AT24C02_STATE_DEVR_REQ;
            }
            break;
        case AT24C02_STATE_DEVR_REQ:
            if(Iic_StartOperation(IIC_OPERATION_WRITE_BYTE, 0xa1u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_DEVR_WAIT;
            }
            break;
        case AT24C02_STATE_DEVR_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_AfterAckState = AT24C02_STATE_READ_REQ;
                At24c02_State = AT24C02_STATE_ACK_REQ;
            }
            break;
        case AT24C02_STATE_READ_REQ:
            if(Iic_StartOperation(IIC_OPERATION_READ_BYTE, 0u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_READ_WAIT;
            }
            break;
        case AT24C02_STATE_READ_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_Buffer[At24c02_Index] = Iic_GetReadData();
                At24c02_Index++;
                At24c02_State = AT24C02_STATE_NACK_REQ;
            }
            break;
        case AT24C02_STATE_NACK_REQ:
            if(Iic_StartOperation(IIC_OPERATION_SEND_NACK, 0u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_NACK_WAIT;
            }
            break;
        case AT24C02_STATE_NACK_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                At24c02_State = AT24C02_STATE_STOP_REQ;
            }
            break;
        case AT24C02_STATE_STOP_REQ:
            if(Iic_StartOperation(IIC_OPERATION_STOP, 0u) == E_OK)
            {
                At24c02_State = AT24C02_STATE_STOP_WAIT;
            }
            break;
        case AT24C02_STATE_STOP_WAIT:
            if(Iic_IsDone() == TRUE)
            {
                if(At24c02_Job == AT24C02_JOB_WRITE)
                {
                    At24c02_WaitUntil = (uint16)(tick + 5u);
                    At24c02_State = AT24C02_STATE_WRITE_WAIT;
                }
                else if(At24c02_Index < At24c02_Length)
                {
                    At24c02_State = AT24C02_STATE_START_REQ;
                }
                else
                {
                    At24c02_State = AT24C02_STATE_DONE;
                }
            }
            break;
        case AT24C02_STATE_WRITE_WAIT:
            if((sint16)(tick - At24c02_WaitUntil) >= 0)
            {
                At24c02_State = AT24C02_STATE_DONE;
            }
            break;
        default:
            break;
    }
}

/* AT24C02是否正在执行读写任务。 */
boolean At24c02_IsBusy(void)
{
    if((At24c02_State != AT24C02_STATE_IDLE) && (At24c02_State != AT24C02_STATE_DONE))
    {
        return TRUE;
    }
    return FALSE;
}

/* 最近一次异步任务是否完成。 */
boolean At24c02_IsJobDone(void)
{
    if(At24c02_State == AT24C02_STATE_DONE)
    {
        return TRUE;
    }
    return FALSE;
}

/* 读取异步读任务缓冲区中的结果。 */
uint8 At24c02_GetData(uint8 index)
{
    if(index >= AT24C02_BUFFER_SIZE)
    {
        return 0u;
    }
    return At24c02_Buffer[index];
}
