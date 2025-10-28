// #ifndef CONFIG_H
// #define CONFIG_H

// // ====================================================
// // 🔧 Build Configuration
// // ====================================================

// // 1 = use software mocks (for simulation)
// // 0 = use real hardware drivers
// #define USE_MOCK_DRIVERS    1

// // ====================================================
// // ⚙️ System Configuration
// // ====================================================
// #define SYSTEM_TICK_MS      10
// #define UART_BAUDRATE       115200
// #define CAN_BAUDRATE        500000
// #define I2C_SPEED_HZ        100000

// #endif // CONFIG_H
#ifndef CONFIG_H_
#define CONFIG_H_

#define SYSTEM_CLOCK_HZ     32000000UL
#define UART_BAUDRATE       9600

#endif
