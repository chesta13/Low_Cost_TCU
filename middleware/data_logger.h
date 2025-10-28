#ifndef DATA_LOGGER_H_
#define DATA_LOGGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "can_driver.h"

#define MAX_LOG_ENTRIES  100

typedef struct {
    uint32_t timestamp_ms;
    double latitude;
    double longitude;
    uint16_t speed_kmh;
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    uint16_t vehicle_speed;
    uint16_t engine_rpm;
    uint32_t vsup_mV;
    uint32_t vbat_mV;
    int16_t temperature_C;
} TelemetryEntry_t;

void DataLogger_Init(void);
void DataLogger_LogCAN(const CAN_Message_t *msg);
void DataLogger_LogTelemetry(const TelemetryEntry_t *entry);
bool DataLogger_FlushToFlash(void);
bool DataLogger_SendToCloud(void);
uint32_t DataLogger_GetEntryCount(void);

#endif // DATA_LOGGER_H_