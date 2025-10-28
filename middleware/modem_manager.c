#include "modem_manager.h"
#include "drivers/uart_driver.h"

void Modem_Init(void)
{
    UART_SendString("Modem Manager Init\r\n");
}
