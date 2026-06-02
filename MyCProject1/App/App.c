/* App层调度器：只负责初始化和周期调用各个SWC，具体业务不放在这里。 */

#include "App.h"
#include "Rte.h"
#include "Swc_AdcSampler.h"
#include "Swc_Command.h"
#include "Swc_Storage.h"
#include "Swc_IrHandler.h"
#include "Swc_LcdPresenter.h"
#include "Swc_SerialTx.h"

static boolean App_IsInitialized = FALSE;

static void App_Init(void)
{
    /* 各SWC自己维护内部状态，App只统一调用初始化入口。 */
    Swc_AdcSampler_Init();
    Swc_Command_Init();
    Swc_Storage_Init();
    Swc_IrHandler_Init();
    Swc_LcdPresenter_Init();
    Swc_SerialTx_Init();
    App_IsInitialized = TRUE;
}

/* 应用层周期任务：先推进底层状态机，再调度各个SWC。 */
void App_MainFunction(void)
{
    uint16 tick;

    /* 先推进底层状态机，避免LCD/IIC/红外缓冲长期得不到处理。 */
    Rte_MainFunction();
    tick = Rte_GetSystemTick();

    /* App初始化依赖EcuM/Rte已完成，所以放在第一次主循环中执行。 */
    if(App_IsInitialized == FALSE)
    {
        App_Init();
    }

    Swc_AdcSampler_MainFunction(tick); /* 采样后发布ADC数据和显示请求 */
    Swc_Command_MainFunction();        /* 把串口字节翻译成业务请求 */
    Swc_Storage_MainFunction();        /* 消费保存/读取请求，推进EEPROM业务 */
    Swc_IrHandler_MainFunction();      /* 消费红外按键，更新LCD和LED */
    Swc_LcdPresenter_MainFunction();   /* 把显示请求送入LCD异步队列 */
    Swc_SerialTx_MainFunction();       /* 串口空闲时逐字节发送响应 */
}
