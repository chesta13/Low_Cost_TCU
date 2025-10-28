// mock_uart.c
// #include <stdio.h>
// #include <string.h>

// void UART_Init(void)
// {
//     printf("[MOCK] UART Initialized\n");
// }

// void UART_Send(const char *data)
// {
//     printf("[MOCK UART TX]: %s", data);
// }

// void UART_Read(char *buffer, int len)
// {
//     strcpy(buffer, "OK");
//     printf("[MOCK UART RX]: %s\n", buffer);
// }

#include "mock_uart.h"
#include <stdio.h>

void UART_Init(void)
{
    printf("[MOCK] UART Initialized\n");
}

void UART_SendString(const char *str)
{
    printf("[MOCK UART] %s", str);
}
