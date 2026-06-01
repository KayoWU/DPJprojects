#include "App_FeatureDemo.h"
#include "Rte.h"
#include "Iap_Cfg.h"
#include "At24c02_Cfg.h"

static uint8 App_FeatureDemo_Initialized = FALSE;
static uint8 App_FeatureDemo_IapNumber;
static uint8 App_FeatureDemo_At24Number;

static uint8 App_FeatureDemo_AdcText[] = "ADC:";
static uint8 App_FeatureDemo_MemText[] = "M:";

/* 在LCD第二行指定列显示两位十进制数，主要用于显示存储计数。 */
static void App_FeatureDemo_Show2Digits(uint8 column, uint8 value)
{
    Rte_Lcd1602ShowChar(2u, column, (uint8)(value / 10u + '0'));
    Rte_Lcd1602ShowChar(2u, (uint8)(column + 1u), (uint8)(value % 10u + '0'));
}

/* 整合演示初始化：读出两种EEPROM的旧值，并在LCD上写固定标签。 */
static void App_FeatureDemo_Init(void)
{
    App_FeatureDemo_IapNumber = Rte_IapReadByte(IAP_DEMO_ADDRESS);
    App_FeatureDemo_At24Number = Rte_At24c02ReadByte(AT24C02_DEMO_ADDRESS);

    if(App_FeatureDemo_IapNumber > 10u)
    {
        App_FeatureDemo_IapNumber = 0u;
    }
    if(App_FeatureDemo_At24Number > 10u)
    {
        App_FeatureDemo_At24Number = 0u;
    }

    Rte_Lcd1602ShowString(1u, 1u, App_FeatureDemo_AdcText);
    Rte_Lcd1602ShowString(2u, 1u, App_FeatureDemo_MemText);
    App_FeatureDemo_Show2Digits(3u, App_FeatureDemo_IapNumber);
    App_FeatureDemo_Show2Digits(6u, App_FeatureDemo_At24Number);

    App_FeatureDemo_Initialized = TRUE;
}

/* 整合演示周期任务：显示ADC电压，并按Timer2节拍更新两种EEPROM计数。 */
void App_FeatureDemo_MainFunction(void)
{
    uint8 adcValue;
    uint16 adcMv;

    if(App_FeatureDemo_Initialized == FALSE)
    {
        App_FeatureDemo_Init();
    }

    adcValue = Rte_ReadAdcValue();
    adcMv = Rte_AdcConvertToMv(adcValue);

    Rte_Lcd1602ShowChar(1u, 5u, (uint8)(adcValue / 100u + '0'));
    Rte_Lcd1602ShowChar(1u, 6u, (uint8)(adcValue / 10u % 10u + '0'));
    Rte_Lcd1602ShowChar(1u, 7u, (uint8)(adcValue % 10u + '0'));
    Rte_Lcd1602ShowChar(1u, 9u, (uint8)(adcMv / 1000u + '0'));
    Rte_Lcd1602ShowChar(1u, 10u, '.');
    Rte_Lcd1602ShowChar(1u, 11u, (uint8)(adcMv / 100u % 10u + '0'));
    Rte_Lcd1602ShowChar(1u, 12u, (uint8)(adcMv / 10u % 10u + '0'));
    Rte_Lcd1602ShowChar(1u, 13u, 'V');

    if(Rte_Timer2GetElapsedFlag() == TRUE)
    {
        Rte_Timer2ClearElapsedFlag();

        App_FeatureDemo_IapNumber++;
        App_FeatureDemo_At24Number++;
        if(App_FeatureDemo_IapNumber > 10u)
        {
            App_FeatureDemo_IapNumber = 0u;
        }
        if(App_FeatureDemo_At24Number > 10u)
        {
            App_FeatureDemo_At24Number = 0u;
        }

        Rte_IapEraseSector(IAP_DEMO_ADDRESS);
        Rte_IapProgramByte(IAP_DEMO_ADDRESS, App_FeatureDemo_IapNumber);
        Rte_At24c02WriteByte(AT24C02_DEMO_ADDRESS, App_FeatureDemo_At24Number);
        App_FeatureDemo_At24Number = Rte_At24c02ReadByte(AT24C02_DEMO_ADDRESS);

        App_FeatureDemo_Show2Digits(3u, App_FeatureDemo_IapNumber);
        App_FeatureDemo_Show2Digits(6u, App_FeatureDemo_At24Number);
    }
}
