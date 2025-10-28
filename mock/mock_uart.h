// mock_uart.h
// #ifndef MOCK_UART_H
// #define MOCK_UART_H

// void UART_Init(void);
// void UART_Send(const char *data);
// void UART_Read(char *buffer, int len);

// #endif


#ifndef MOCK_UART_H_
#define MOCK_UART_H_

void UART_Init(void);
void UART_SendString(const char *str);

#endif
