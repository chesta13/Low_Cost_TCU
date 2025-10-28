// #include "config.h"

// #if USE_MOCK_DRIVERS
// #include "mock/mock_uart.h"
// #include "mock/mock_modem.h"
// #else
// #include "drivers/uart_driver.h"
// #include "middleware/modem_manager.h"
// #endif

// void app_tasks_run(void)
// {
//     UART_Send("System Started\n");

//     Modem_SendCommand("AT");
//     Modem_ReadResponse();
// }


#include "app_tasks.h"
#include "gpio_driver.h"
#include "uart_driver.h"

void App_Task_Loop(void)
{
    static int ledState = 0;
    GPIO_SetLED(ledState);
    ledState = !ledState;

    UART_SendString("LED Toggled\r\n");

    for (volatile uint32_t i = 0; i < 100000; i++); // simple delay
}
