/*
 * TCU Firmware - Complete Implementation
 * Texas Instruments MSPM0G3507 based Telematics Control Unit
 */
 #include "ti_msp_dl_config.h"
#include "system_init.h"
#include "task_scheduler.h"
#include "config.h"

// Drivers
#include "drivers/gpio_driver.h"
#include "drivers/uart_driver.h"
#include "drivers/adc_driver.h"
#include "drivers/can_driver.h"
#include "drivers/imu_driver.h"
#include "drivers/spi_driver.h"

// Middleware
#include "middleware/power_manager.h"
#include "middleware/modem_manager.h"
#include "middleware/can_parser.h"
#include "middleware/data_logger.h"

#include <stdio.h>
#include <string.h>

typedef struct
{
    bool fix_valid;        // GPS fix valid or not
    float latitude;        // Latitude value
    float longitude;       // Longitude value
    uint8_t satellites;    // Number of satellites
    float speed_kmh;       // Speed in km/h
} GPS_Data_t;

// ====================================================
// Task Function Declarations
// ====================================================
void Task_CANProcess(void);
void Task_IMURead(void);
void Task_GPSUpdate(void);
void Task_PowerMonitor(void);
void Task_TelemetryLog(void);
void Task_DataTransmit(void);
void Task_ModemUpdate(void);

// ====================================================
// Global Variables
// ====================================================
static GPS_Data_t current_gps;
static IMU_DataScaled_t current_imu;
static VehicleData_t current_vehicle;
static PowerStatus_t current_power;

// ====================================================
// Main Function
// ====================================================
int main(void)
{
    // ================================================
    // 1. System Initialization
    // ================================================
    System_Init();
    
    UART_SendString("\r\n");
    UART_SendString("========================================\r\n");
    UART_SendString("  TCU Firmware v1.0\r\n");
    UART_SendString("  Low-Cost Telematics Control Unit\r\n");
    UART_SendString("  (c) 2025 - MSPM0G3507\r\n");
    UART_SendString("========================================\r\n\r\n");
    
    // ================================================
    // 2. Driver Initialization
    // ================================================
    UART_SendString("[INIT] Initializing drivers...\r\n");
    
    GPIO_Init();
    UART_SendString("  [OK] GPIO\r\n");
    
    ADC_Init();
    UART_SendString("  [OK] ADC\r\n");
    
    SPI_Init();
    UART_SendString("  [OK] SPI\r\n");
    
    if (CAN_Init()) {
        UART_SendString("  [OK] CAN-FD\r\n");
    } else {
        UART_SendString("  [WARN] CAN-FD init failed\r\n");
    }
    
    if (IMU_Init()) {
        UART_SendString("  [OK] IMU (ICM-42670-P)\r\n");
    } else {
        UART_SendString("  [WARN] IMU init failed\r\n");
    }
    
    // ================================================
    // 3. Middleware Initialization
    // ================================================
    UART_SendString("\r\n[INIT] Initializing middleware...\r\n");
    
    Power_Init();
    UART_SendString("  [OK] Power Manager\r\n");
    
    Modem_Init();
    UART_SendString("  [OK] Modem Manager\r\n");
    
    CANParser_Init();
    UART_SendString("  [OK] CAN Parser\r\n");
    
    DataLogger_Init();
    UART_SendString("  [OK] Data Logger\r\n");
    
    // ================================================
    // 4. Check Initial Power Status
    // ================================================
    UART_SendString("\r\n[INIT] Checking power status...\r\n");
    current_power = Power_GetStatus();
    
    char buffer[128];
    sprintf(buffer, "  Vsup: %u mV\r\n", current_power.vsup_mV);
    UART_SendString(buffer);
    sprintf(buffer, "  Vbat: %u mV\r\n", current_power.vbat_mV);
    UART_SendString(buffer);
    sprintf(buffer, "  Temp: %d C\r\n", current_power.temperature_C);
    UART_SendString(buffer);
    
    if (!Power_IsVoltageOK()) {
        UART_SendString("  [ERROR] Voltage out of range!\r\n");
        UART_SendString("  System halted for safety.\r\n");
        while (1) {
            GPIO_ToggleLED();
            delay_cycles(SYSTEM_CLOCK_HZ / 4);  // 250ms
        }
    }
    
    UART_SendString("  [OK] Power status nominal\r\n");
    
    // ================================================
    // 5. Power On Modem
    // ================================================
    UART_SendString("\r\n[INIT] Starting modem...\r\n");
    Modem_PowerOn();
    
    // ================================================
    // 6. Task Scheduler Setup
    // ================================================
    UART_SendString("\r\n[INIT] Setting up task scheduler...\r\n");
    Scheduler_Init();
    
    Scheduler_RegisterTask(TASK_CAN_PROCESS,    Task_CANProcess,    TASK_PERIOD_CAN);
    UART_SendString("  [OK] CAN Process Task (10ms)\r\n");
    
    Scheduler_RegisterTask(TASK_IMU_READ,       Task_IMURead,       TASK_PERIOD_IMU);
    UART_SendString("  [OK] IMU Read Task (50ms)\r\n");
    
    Scheduler_RegisterTask(TASK_GPS_UPDATE,     Task_GPSUpdate,     TASK_PERIOD_GPS);
    UART_SendString("  [OK] GPS Update Task (1s)\r\n");
    
    Scheduler_RegisterTask(TASK_POWER_MONITOR,  Task_PowerMonitor,  TASK_PERIOD_POWER);
    UART_SendString("  [OK] Power Monitor Task (500ms)\r\n");
    
    Scheduler_RegisterTask(TASK_TELEMETRY_LOG,  Task_TelemetryLog,  TASK_PERIOD_TELEMETRY);
    UART_SendString("  [OK] Telemetry Log Task (5s)\r\n");
    
    Scheduler_RegisterTask(TASK_DATA_TRANSMIT,  Task_DataTransmit,  TASK_PERIOD_TELEMETRY);
    UART_SendString("  [OK] Data Transmit Task (5s)\r\n");
    
    Scheduler_RegisterTask(TASK_MODEM_UPDATE,   Task_ModemUpdate,   TASK_PERIOD_POWER);
    UART_SendString("  [OK] Modem Update Task (500ms)\r\n");
    
    // ================================================
    // 7. Initialization Complete
    // ================================================
    UART_SendString("\r\n========================================\r\n");
    UART_SendString("  INITIALIZATION COMPLETE\r\n");
    UART_SendString("  System entering normal operation...\r\n");
    UART_SendString("========================================\r\n\r\n");
    
    GPIO_SetLED(1);  // Turn on LED to indicate running
    
    // ================================================
    // 8. Main Loop - Task Scheduler
    // ================================================
    while (1)
    {
        // Run task scheduler
        Scheduler_Run();
        
        // Critical safety check
        current_power = Power_GetStatus();
        if (current_power.state == POWER_STATE_ERROR) {
            UART_SendString("[CRITICAL] Power error detected - initiating shutdown\r\n");
            Power_RequestShutdown();
        }
        
        // Optional: Enter low power mode between tasks
        // __WFI();  // Wait for interrupt
    }
}

// ====================================================
// Task Implementations
// ====================================================

/**
 * @brief CAN message processing task
 * Period: 10ms
 */
void Task_CANProcess(void)
{
    CAN_Message_t msg;
    
    // Process all pending CAN messages
    while (CAN_Receive(&msg)) {
        // Log raw message
        DataLogger_LogCAN(&msg);
        
        // Parse message
        CANParser_ProcessMessage(&msg);
    }
    
    // Update current vehicle data
    current_vehicle = CANParser_GetVehicleData();
}

/**
 * @brief IMU data reading task
 * Period: 50ms
 */
void Task_IMURead(void)
{
    static uint32_t read_count = 0;
    
    if (IMU_ReadScaled(&current_imu)) {
        read_count++;
        
        // Optional: Print IMU data every 20 reads (1 second)
        if (read_count % 20 == 0) {
            char buffer[128];
            sprintf(buffer, "[IMU] Accel: X=%.2f Y=%.2f Z=%.2f g\r\n",
                    current_imu.accel_x_g,
                    current_imu.accel_y_g,
                    current_imu.accel_z_g);
            UART_SendString(buffer);
        }
        
        // Detect harsh acceleration/braking (>0.5g)
        if (current_imu.accel_x_g > 0.5f || current_imu.accel_x_g < -0.5f) {
            UART_SendString("[EVENT] Harsh acceleration detected!\r\n");
        }
        
        // Detect collision (>3g)
        if (current_imu.accel_x_g > 3.0f || current_imu.accel_y_g > 3.0f || current_imu.accel_z_g > 3.0f) {
            UART_SendString("[CRITICAL] Collision detected!\r\n");
            // Trigger emergency response
        }
    }
}

/**
 * @brief GPS update task
 * Period: 1000ms
 */
void Task_GPSUpdate(void)
{
    static uint32_t gps_fail_count = 0;
    
    if (Modem_GetGPS(&current_gps)) {
        if (current_gps.fix_valid) {
            gps_fail_count = 0;
            
            char buffer[128];
            sprintf(buffer, "[GPS] Lat: %.6f, Lon: %.6f, Sats: %d, Speed: %.1f km/h\r\n",
                    current_gps.latitude,
                    current_gps.longitude,
                    current_gps.satellites,
                    current_gps.speed_kmh);
            UART_SendString(buffer);
        } else {
            gps_fail_count++;
            if (gps_fail_count % 10 == 0) {
                UART_SendString("[GPS] No valid fix\r\n");
            }
        }
    }
}

/**
 * @brief Power monitoring task
 * Period: 500ms
 */
void Task_PowerMonitor(void)
{
    static uint32_t monitor_count = 0;
    
    Power_Update();
    current_power = Power_GetStatus();
    
    monitor_count++;
    
    // Print power status every 10 cycles (5 seconds)
    if (monitor_count % 10 == 0) {
        char buffer[256];
        sprintf(buffer, "[POWER] State: %s | Vsup: %u mV | Vbat: %u mV | Temp: %d C | Uptime: %u s\r\n",
                Power_GetStateString(),
                current_power.vsup_mV,
                current_power.vbat_mV,
                current_power.temperature_C,
                current_power.uptime_sec);
        UART_SendString(buffer);
    }
    
    // Check for low voltage warning
    if (current_power.vsup_mV < VSUP_MIN_THRESHOLD_MV + 1000) {  // Within 1V of minimum
        UART_SendString("[WARN] Low vehicle voltage!\r\n");
    }
    
    // Check for high temperature warning
    if (current_power.temperature_C > TEMP_MAX_THRESHOLD_C - 10) {  // Within 10°C of max
        UART_SendString("[WARN] High temperature!\r\n");
    }
}

/**
 * @brief Telemetry logging task
 * Period: 5000ms
 */
void Task_TelemetryLog(void)
{
    TelemetryEntry_t entry;
    
    // Populate telemetry entry
    entry.timestamp_ms = Scheduler_GetSystemTime_ms();
    entry.latitude = current_gps.latitude;
    entry.longitude = current_gps.longitude;
    entry.speed_kmh = current_gps.speed_kmh;
    entry.accel_x = (int16_t)(current_imu.accel_x_g * 1000);  // Convert to mg
    entry.accel_y = (int16_t)(current_imu.accel_y_g * 1000);
    entry.accel_z = (int16_t)(current_imu.accel_z_g * 1000);
    entry.vehicle_speed = current_vehicle.vehicle_speed_kmh;
    entry.engine_rpm = current_vehicle.engine_rpm;
    entry.vsup_mV = current_power.vsup_mV;
    entry.vbat_mV = current_power.vbat_mV;
    entry.temperature_C = current_power.temperature_C;
    
    // Log entry
    DataLogger_LogTelemetry(&entry);
    
    UART_SendString("[LOGGER] Telemetry entry logged\r\n");
}

/**
 * @brief Data transmission task
 * Period: 5000ms
 */
void Task_DataTransmit(void)
{
    ModemStatus_t modem_status = Modem_GetStatus();
    
    if (modem_status.state == MODEM_STATE_CONNECTED) {
        // Send data to cloud
        if (DataLogger_SendToCloud()) {
            GPIO_ToggleLED();  // Visual feedback
        }
    } else {
        static uint32_t not_connected_count = 0;
        not_connected_count++;
        
        if (not_connected_count % 6 == 0) {  // Every 30 seconds
            UART_SendString("[MODEM] Not connected - cannot transmit\r\n");
        }
    }
}

/**
 * @brief Modem state machine update task
 * Period: 500ms
 */
void Task_ModemUpdate(void)
{
    Modem_Update();
    
    static ModemState_t last_state = MODEM_STATE_OFF;
    ModemStatus_t status = Modem_GetStatus();
    
    // Print state changes
    if (status.state != last_state) {
        char buffer[64];
        sprintf(buffer, "[MODEM] State changed to: %d\r\n", status.state);
        UART_SendString(buffer);
        last_state = status.state;
    }
    
    // If ready but not connected, try to connect
    static uint32_t connect_attempt_timer = 0;
    if (status.state == MODEM_STATE_READY) {
        connect_attempt_timer++;
        if (connect_attempt_timer > 10) {  // Wait 5 seconds
            UART_SendString("[MODEM] Attempting network connection...\r\n");
            Modem_ConnectNetwork();
            connect_attempt_timer = 0;
        }
    }
}