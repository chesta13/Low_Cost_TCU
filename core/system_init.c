#include "ti_msp_dl_config.h"
#include "config.h"

// #if USE_MOCK_DRIVERS
// #include "mock/mock_uart.h"
// #else
// #include "drivers/uart_driver.h"
// #include "drivers/gpio_driver.h"
// #include "drivers/can_driver.h"
// #endif

// void system_init(void)
// {
//     SYSCFG_DL_init(); // SysConfig auto-generated init

// #if USE_MOCK_DRIVERS
//     UART_Init();
// #else
//     GPIO_Init();
//     UART_Init(UART_BAUDRATE);
//     CAN_Init(CAN_BAUDRATE);
// #endif
// }


#include "system_init.h"
#include "ti_msp_dl_config.h"

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
}
