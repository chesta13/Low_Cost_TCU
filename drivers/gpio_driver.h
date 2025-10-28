// // gpio_driver.h
// #ifndef GPIO_DRIVER_H
// #define GPIO_DRIVER_H

// #include <stdint.h>    
// #include <stdbool.h>   // optional for future use

// void GPIO_Init(void);
// void GPIO_SetLED(uint8_t state);

// #endif // GPIO_DRIVER_H


#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

#include <stdint.h>

void GPIO_Init(void);
void GPIO_SetLED(uint8_t state);
void GPIO_ToggleLED(void);

#endif
