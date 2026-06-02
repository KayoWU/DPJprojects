#include "Swc_AdcSampler.h"
#include "Rte.h"

#define SWC_ADC_SAMPLER_PERIOD_MS 100u

static uint16 Swc_AdcSampler_NextTick = 0u;

void Swc_AdcSampler_Init(void)
{
    Swc_AdcSampler_NextTick = 0u;
}

/* 周期采样ADC，并把结果通过RTE交给其他SWC使用。 */
void Swc_AdcSampler_MainFunction(uint16 tick)
{
    uint8 adcRaw;
    uint16 adcMv;

    /* 用有符号差值判断时间到达，可兼容16位tick回绕。 */
    if((sint16)(tick - Swc_AdcSampler_NextTick) >= 0)
    {
        adcRaw = Rte_ReadAdcValue();
        adcMv = Rte_AdcConvertToMv(adcRaw);
        Rte_WriteAdcSample(adcRaw, adcMv);        /* 给存储SWC读取最新值 */
        Rte_DisplayRequestAdcLine(adcRaw, adcMv); /* 请求LCD第一行刷新 */
        Swc_AdcSampler_NextTick = (uint16)(tick + SWC_ADC_SAMPLER_PERIOD_MS);
    }
}
