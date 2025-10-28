#include "imu_driver.h"
#include "spi_driver.h"
#include "config.h"
#include <ti/driverlib/driverlib.h>

// Helper functions
static void IMU_WriteReg(uint8_t reg, uint8_t value);
static uint8_t IMU_ReadReg(uint8_t reg);
static void IMU_ReadRegs(uint8_t reg, uint8_t *buffer, uint8_t len);

bool IMU_Init(void)
{
    // Delay for power-on
    delay_cycles(SYSTEM_CLOCK_HZ / 100);  // 10ms
    
    // Check WHO_AM_I
    uint8_t who_am_i = IMU_ReadReg(ICM42670_WHO_AM_I);
    if (who_am_i != ICM42670_DEVICE_ID) {
        return false;
    }
    
    // Reset device
    IMU_WriteReg(ICM42670_PWR_MGMT0, 0x00);
    delay_cycles(SYSTEM_CLOCK_HZ / 100);  // 10ms
    
    // Configure power management: Enable accel and gyro in low noise mode
    IMU_WriteReg(ICM42670_PWR_MGMT0, 0x0F);
    delay_cycles(SYSTEM_CLOCK_HZ / 100);  // 10ms
    
    // Configure accelerometer: ±16g, ODR = 1kHz
    IMU_WriteReg(ICM42670_ACCEL_CONFIG0, 0x06);
    
    // Configure gyroscope: ±2000dps, ODR = 1kHz
    IMU_WriteReg(ICM42670_GYRO_CONFIG0, 0x06);
    
    delay_cycles(SYSTEM_CLOCK_HZ / 50);  // 20ms settling time
    
    return true;
}

bool IMU_ReadRaw(IMU_Data_t *data)
{
    uint8_t buffer[14];
    
    // Read temperature + accel + gyro data (14 bytes total)
    IMU_ReadRegs(ICM42670_TEMP_DATA1, buffer, 14);
    
    // Parse data
    data->temperature = (int16_t)((buffer[0] << 8) | buffer[1]);
    data->accel_x = (int16_t)((buffer[2] << 8) | buffer[3]);
    data->accel_y = (int16_t)((buffer[4] << 8) | buffer[5]);
    data->accel_z = (int16_t)((buffer[6] << 8) | buffer[7]);
    data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]);
    data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]);
    data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]);
    
    return true;
}

bool IMU_ReadScaled(IMU_DataScaled_t *data)
{
    IMU_Data_t raw;
    if (!IMU_ReadRaw(&raw)) {
        return false;
    }
    
    // Scale factors (adjust based on selected range)
    const float accel_scale = 16.0f / 32768.0f;  // ±16g range
    const float gyro_scale = 2000.0f / 32768.0f;  // ±2000dps range
    const float temp_scale = 1.0f / 132.48f;
    const float temp_offset = 25.0f;
    
    data->accel_x_g = raw.accel_x * accel_scale;
    data->accel_y_g = raw.accel_y * accel_scale;
    data->accel_z_g = raw.accel_z * accel_scale;
    data->gyro_x_dps = raw.gyro_x * gyro_scale;
    data->gyro_y_dps = raw.gyro_y * gyro_scale;
    data->gyro_z_dps = raw.gyro_z * gyro_scale;
    data->temp_C = (raw.temperature * temp_scale) + temp_offset;
    
    return true;
}

bool IMU_SelfTest(void)
{
    // Read WHO_AM_I to verify communication
    uint8_t who_am_i = IMU_ReadReg(ICM42670_WHO_AM_I);
    return (who_am_i == ICM42670_DEVICE_ID);
}

// Helper functions
static void IMU_WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = {reg & 0x7F, value};  // MSB=0 for write
    
    SPI_ChipSelect(true);
    SPI_Transfer(tx, NULL, 2);
    SPI_ChipSelect(false);
    
    delay_cycles(100);  // Small delay between transactions
}

static uint8_t IMU_ReadReg(uint8_t reg)
{
    uint8_t tx[2] = {reg | 0x80, 0x00};  // MSB=1 for read
    uint8_t rx[2];
    
    SPI_ChipSelect(true);
    SPI_Transfer(tx, rx, 2);
    SPI_ChipSelect(false);
    
    return rx[1];
}

static void IMU_ReadRegs(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    uint8_t tx_buffer[32];
    tx_buffer[0] = reg | 0x80;  // MSB=1 for read
    
    SPI_ChipSelect(true);
    SPI_Transfer(tx_buffer, buffer, len + 1);
    SPI_ChipSelect(false);
    
    // Shift data (first byte is dummy)
    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = buffer[i + 1];
    }
}