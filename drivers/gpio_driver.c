// // gpio_driver.c
// #include "gpio_driver.h"
// #include "ti_msp_dl_config.h"

// void GPIO_Init(void)
// {
//     // SysConfig handles setup, maybe extra init here
// }

// void GPIO_SetLED(uint8_t state)
// {
//     if (state)
//         DL_GPIO_setPins(GPIO_GRP_0, GPIO_PIN_0);
//     else
//         DL_GPIO_clearPins(GPIO_GRP_0, GPIO_PIN_0);
// }

#include "gpio_driver.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_gpio.h>

void GPIO_Init(void)
{
    SYSCFG_DL_GPIO_init();
}

void GPIO_SetLED(uint8_t state)
{
    if (state)
        DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);
    else
        DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);
}

void GPIO_ToggleLED(void)
{
    DL_GPIO_togglePins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);
}
