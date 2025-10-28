#include "system_init.h"
#include "ti_msp_dl_config.h"
#include "config.h"

void System_Init(void)
{
    // Initialize system power and clocks
    SYSCFG_DL_initPower();
    SYSCFG_DL_SYSCTL_init();

    // Initialize all peripherals configured in SysConfig
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_UART_0_init();
    SYSCFG_DL_SPI_0_init();
    SYSCFG_DL_I2C_0_init();
    SYSCFG_DL_TIMER_0_init();
    
    // Note: Add these if you configure ADC and CAN in SysConfig
    // SYSCFG_DL_ADC_0_init();
    // SYSCFG_DL_MCAN_0_init();
    
    // Enable global interrupts
    __enable_irq();
}