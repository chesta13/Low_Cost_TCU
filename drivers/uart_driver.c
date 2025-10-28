// #include "uart_driver.h"
// #include "ti_msp_dl_config.h"

// void UART_Init(uint32_t baudrate)
// {
//     // Configure UART using TI DriverLib APIs
//     // (Use SysConfig-generated UART instance)
//     DL_UART_init(UART_INST, &UART_config);
//     DL_UART_enable(UART_INST);
// }

// void UART_Send(const char *data)
// {
//     while (*data) {
//         DL_UART_transmitData(UART_INST, *data++);
//         while (!DL_UART_isTXEmpty(UART_INST));
//     }
// }

// void UART_Read(char *buffer, uint16_t len)
// {
//     for (uint16_t i = 0; i < len; i++) {
//         while (!DL_UART_isRXReady(UART_INST));
//         buffer[i] = DL_UART_receiveData(UART_INST);
//     }
// }

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
