#ifndef IMU_DRIVER_H_
#define IMU_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

// ICM-42670-P Register addresses
#define ICM42670_WHO_AM_I           0x75
#define ICM42670_PWR_MGMT0          0x4E
#define ICM42670_ACCEL_CONFIG0      0x50
#define ICM42670_GYRO_CONFIG0       0x4F
#define ICM42670_TEMP_DATA1         0x1D
#define ICM42670_ACCEL_DATA_X1      0x1F
#define ICM42670_GYRO_DATA_X1       0x25

// Expected WHO_AM_I value
#define ICM42670_DEVICE_ID          0x67

// IMU data structure
typedef struct {
    int16_t accel_x;      // Acceleration X (raw)
    int16_t accel_y;      // Acceleration Y (raw)
    int16_t accel_z;      // Acceleration Z (raw)
    int16_t gyro_x;       // Gyroscope X (raw)
    int16_t gyro_y;       // Gyroscope Y (raw)
    int16_t gyro_z;       // Gyroscope Z (raw)
    int16_t temperature;  // Temperature (raw)
} IMU_Data_t;

// Scaled IMU data (SI units)
typedef struct {
    float accel_x_g;      // Acceleration X (g)
    float accel_y_g;      // Acceleration Y (g)
    float accel_z_g;      // Acceleration Z (g)
    float gyro_x_dps;     // Gyroscope X (deg/s)
    float gyro_y_dps;     // Gyroscope Y (deg/s)
    float gyro_z_dps;     // Gyroscope Z (deg/s)
    float temp_C;         // Temperature (°C)
} IMU_DataScaled_t;

bool IMU_Init(void);
bool IMU_ReadRaw(IMU_Data_t *data);
bool IMU_ReadScaled(IMU_DataScaled_t *data);
bool IMU_SelfTest(void);

#endif // IMU_DRIVER_H_