#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

// ADC Channel definitions
typedef enum {
    ADC_CH_VSUP_SENSE = 0,    // Vehicle voltage (12V/24V)
    ADC_CH_VBAT_SENSE = 1,    // Backup battery voltage
    ADC_CH_TEMP_SENSE = 2,    // TMP235 temperature sensor
    ADC_CH_COUNT
} ADC_Channel_t;

// ADC result structure
typedef struct {
    uint16_t raw;             // Raw ADC value
    uint32_t voltage_mV;      // Converted voltage in mV
} ADC_Result_t;

/**
 * @brief Initialize ADC peripheral
 */
void ADC_Init(void);

/**
 * @brief Read raw ADC value from specified channel
 * @param channel ADC channel to read
 * @return Raw ADC value (0-4095 for 12-bit)
 */
uint16_t ADC_ReadRaw(ADC_Channel_t channel);

/**
 * @brief Read voltage from specified channel
 * @param channel ADC channel to read
 * @return Voltage in millivolts
 */
uint32_t ADC_ReadVoltage_mV(ADC_Channel_t channel);

/**
 * @brief Read all ADC channels
 * @param results Array to store results (must be ADC_CH_COUNT size)
 */
void ADC_ReadAll(ADC_Result_t *results);

/**
 * @brief Read temperature from TMP235 sensor
 * @return Temperature in degrees Celsius (x10 for one decimal place)
 */
int16_t ADC_ReadTemperature_C(void);

/**
 * @brief Read vehicle supply voltage (Vsup)
 * @return Voltage in millivolts
 */
uint32_t ADC_ReadVsup_mV(void);

/**
 * @brief Read backup battery voltage (Vbat)
 * @return Voltage in millivolts
 */
uint32_t ADC_ReadVbat_mV(void);

#endif // ADC_DRIVER_H_