#include "uart_driver.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_uart.h>

void UART_Init(void)
{
    SYSCFG_DL_UART_0_init();
}

void UART_SendChar(char c)
{
    DL_UART_transmitDataBlocking(UART_0_INST, c);
}

void UART_SendString(const char *str)
{
    while (*str)
        UART_SendChar(*str++);
}