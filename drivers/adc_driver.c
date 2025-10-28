#include "adc_driver.h"
#include "ti_msp_dl_config.h"
#include "config.h"
#include <ti/driverlib/dl_adc12.h>

// ADC channel mapping to hardware pins
static const uint32_t adc_channels[ADC_CH_COUNT] = {
    DL_ADC12_INPUT_CHAN_0,  // Vsup sense
    DL_ADC12_INPUT_CHAN_1,  // Vbat sense
    DL_ADC12_INPUT_CHAN_2   // TMP235 temperature
};

void ADC_Init(void)
{
    // ADC is initialized via SysConfig
    // Additional runtime configuration can go here
    
    // Enable ADC
    DL_ADC12_enableConversions(ADC12_0_INST);
}

uint16_t ADC_ReadRaw(ADC_Channel_t channel)
{
    if (channel >= ADC_CH_COUNT) {
        return 0;
    }
    
    // Select channel
    DL_ADC12_configConversionMem(ADC12_0_INST, DL_ADC12_MEM_IDX_0,
                                  adc_channels[channel],
                                  DL_ADC12_REFERENCE_VOLTAGE_VDDA,
                                  DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0,
                                  DL_ADC12_AVERAGING_MODE_DISABLED,
                                  DL_ADC12_BURN_OUT_SOURCE_DISABLED,
                                  DL_ADC12_TRIGGER_MODE_AUTO_NEXT,
                                  DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    
    // Start conversion
    DL_ADC12_startConversion(ADC12_0_INST);
    
    // Wait for conversion to complete
    while (DL_ADC12_isBusy(ADC12_0_INST));
    
    // Read result
    return DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
}

uint32_t ADC_ReadVoltage_mV(ADC_Channel_t channel)
{
    uint16_t raw = ADC_ReadRaw(channel);
    
    // Convert to millivolts: (raw / 4095) * 3300
    uint32_t voltage_mV = ((uint32_t)raw * ADC_VREF_MV) / ADC_MAX_VALUE;
    
    return voltage_mV;
}

void ADC_ReadAll(ADC_Result_t *results)
{
    for (int i = 0; i < ADC_CH_COUNT; i++) {
        results[i].raw = ADC_ReadRaw(i);
        results[i].voltage_mV = ADC_ReadVoltage_mV(i);
    }
}

int16_t ADC_ReadTemperature_C(void)
{
    uint32_t voltage_mV = ADC_ReadVoltage_mV(ADC_CH_TEMP_SENSE);
    
    // TMP235: Vout = 500mV + (10mV/°C × T)
    // T = (Vout - 500mV) / 10mV
    int16_t temp_C = (int16_t)((voltage_mV - TMP235_OFFSET_MV) / TMP235_SCALE_MVC);
    
    return temp_C;
}

uint32_t ADC_ReadVsup_mV(void)
{
    uint32_t adc_voltage = ADC_ReadVoltage_mV(ADC_CH_VSUP_SENSE);
    
    // Apply voltage divider ratio
    uint32_t vsup_mV = (uint32_t)(adc_voltage * VSUP_DIVIDER_RATIO);
    
    return vsup_mV;
}

uint32_t ADC_ReadVbat_mV(void)
{
    uint32_t adc_voltage = ADC_ReadVoltage_mV(ADC_CH_VBAT_SENSE);
    
    // Apply voltage divider ratio
    uint32_t vbat_mV = (uint32_t)(adc_voltage * VBAT_DIVIDER_RATIO);
    
    return vbat_mV;
}