// #ifndef UART_DRIVER_H
// #define UART_DRIVER_H

// #include <stdint.h>

// void UART_Init(uint32_t baudrate);
// void UART_Send(const char *data);
// void UART_Read(char *buffer, uint16_t len);

// #endif // UART_DRIVER_H


#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

void UART_Init(void);
void UART_SendChar(char c);
void UART_SendString(const char *str);

#endif
