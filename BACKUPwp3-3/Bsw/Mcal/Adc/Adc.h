#ifndef ADC_H
#define ADC_H

#include "Config.h"
#include "Adc_Cfg.h"

#define ADC_VENDOR_ID                 0u
#define ADC_MODULE_ID                 205u
#define ADC_SW_MAJOR_VERSION          1u
#define ADC_SW_MINOR_VERSION          0u
#define ADC_SW_PATCH_VERSION          0u

void Adc_Init(void);
uint8 Adc_ReadValue(void);
uint16 Adc_ConvertToMv(uint8 adcValue);

#endif
