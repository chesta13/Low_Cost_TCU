#include "power_manager.h"
#include "adc_driver.h"
#include "gpio_driver.h"
#include "uart_driver.h"
#include "config.h"
#include <ti/driverlib/dl_gpio.h>
#include <stdio.h>
#include <string.h>

static PowerStatus_t power_status;
static uint32_t tick_counter = 0;

// GPIO definitions for power control
#define MODEM_POWER_PORT    GPIO_GRP_0_PORT
#define MODEM_POWER_PIN     GPIO_GRP_0_PIN_1_PIN
#define SWITCH_ENABLE_PORT  GPIO_GRP_0_PORT
#define SWITCH_ENABLE_PIN   GPIO_GRP_0_PIN_2_PIN

void Power_Init(void)
{
    memset(&power_status, 0, sizeof(PowerStatus_t));
    power_status.state = POWER_STATE_STARTUP;
    
    // Initialize ADC
    ADC_Init();
    
    // Default: Modem off, switch disabled
    Power_EnableModem(false);
    Power_EnableSwitch(false);
    
    // Read initial voltages
    Power_Update();
    
    // Check if voltages are OK
    if (Power_IsVoltageOK()) {
        power_status.state = POWER_STATE_NORMAL;
        UART_SendString("[POWER] System power OK\r\n");
    } else {
        power_status.state = POWER_STATE_ERROR;
        UART_SendString("[POWER] Voltage out of range!\r\n");
    }
}

void Power_Update(void)
{
    // Read voltages
    power_status.vsup_mV = ADC_ReadVsup_mV();
    power_status.vbat_mV = ADC_ReadVbat_mV();
    power_status.temperature_C = ADC_ReadTemperature_C();
    
    // Update uptime (called every TASK_PERIOD_POWER ms)
    tick_counter++;
    power_status.uptime_sec = (tick_counter * TASK_PERIOD_POWER) / 1000;
    
    // Check charging status (Vbat increasing)
    static uint32_t last_vbat = 0;
    if (power_status.vbat_mV > last_vbat + 50) {  // 50mV threshold
        power_status.charging = true;
    } else {
        power_status.charging = false;
    }
    last_vbat = power_status.vbat_mV;
    
    // State machine
    switch (power_status.state) {
        case POWER_STATE_STARTUP:
            if (Power_IsVoltageOK()) {
                power_status.state = POWER_STATE_NORMAL;
            }
            break;
            
        case POWER_STATE_NORMAL:
            if (!Power_IsVoltageOK()) {
                power_status.state = POWER_STATE_ERROR;
                UART_SendString("[POWER] ERROR: Voltage fault\r\n");
            }
            if (!Power_IsTemperatureOK()) {
                power_status.state = POWER_STATE_ERROR;
                UART_SendString("[POWER] ERROR: Temperature fault\r\n");
            }
            if (power_status.vbat_mV < VBAT_MIN_THRESHOLD_MV) {
                power_status.state = POWER_STATE_LOW_BATTERY;
                UART_SendString("[POWER] WARNING: Low battery\r\n");
            }
            break;
            
        case POWER_STATE_LOW_BATTERY:
            if (power_status.charging && power_status.vbat_mV > VBAT_MIN_THRESHOLD_MV + 200) {
                power_status.state = POWER_STATE_NORMAL;
                UART_SendString("[POWER] Battery recovered\r\n");
            }
            break;
            
        case POWER_STATE_ERROR:
            // Stay in error state until manual reset
            break;
            
        case POWER_STATE_SHUTDOWN:
            // Prepare for shutdown
            Power_EnableModem(false);
            Power_EnableSwitch(false);
            break;
            
        default:
            break;
    }
}

PowerStatus_t Power_GetStatus(void)
{
    return power_status;
}

void Power_EnableModem(bool enable)
{
    if (enable) {
        DL_GPIO_setPins(MODEM_POWER_PORT, MODEM_POWER_PIN);
        power_status.modem_enabled = true;
    } else {
        DL_GPIO_clearPins(MODEM_POWER_PORT, MODEM_POWER_PIN);
        power_status.modem_enabled = false;
    }
}

void Power_EnableSwitch(bool enable)
{
    if (enable) {
        DL_GPIO_setPins(SWITCH_ENABLE_PORT, SWITCH_ENABLE_PIN);
    } else {
        DL_GPIO_clearPins(SWITCH_ENABLE_PORT, SWITCH_ENABLE_PIN);
    }
}

void Power_RequestShutdown(void)
{
    UART_SendString("[POWER] Shutdown requested\r\n");
    power_status.state = POWER_STATE_SHUTDOWN;
    
    // Save critical data to flash
    // ...
    
    // Disable all peripherals
    Power_EnableModem(false);
    Power_EnableSwitch(false);
    
    // Enter low power mode
    __WFI();  // Wait for interrupt
}

bool Power_IsVoltageOK(void)
{
    return (power_status.vsup_mV >= VSUP_MIN_THRESHOLD_MV &&
            power_status.vsup_mV <= VSUP_MAX_THRESHOLD_MV &&
            power_status.vbat_mV >= VBAT_MIN_THRESHOLD_MV &&
            power_status.vbat_mV <= VBAT_MAX_THRESHOLD_MV);
}

bool Power_IsTemperatureOK(void)
{
    return (power_status.temperature_C < TEMP_MAX_THRESHOLD_C);
}

const char* Power_GetStateString(void)
{
    switch (power_status.state) {
        case POWER_STATE_STARTUP:       return "STARTUP";
        case POWER_STATE_NORMAL:        return "NORMAL";
        case POWER_STATE_LOW_BATTERY:   return "LOW_BATTERY";
        case POWER_STATE_CHARGING:      return "CHARGING";
        case POWER_STATE_SHUTDOWN:      return "SHUTDOWN";
        case POWER_STATE_ERROR:         return "ERROR";
        default:                        return "UNKNOWN";
    }
}