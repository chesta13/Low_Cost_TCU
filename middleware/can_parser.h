#ifndef CAN_PARSER_H_
#define CAN_PARSER_H_

#include <stdint.h>
#include <stdbool.h>
#include "can_driver.h"

// Vehicle data structure
typedef struct {
    uint16_t vehicle_speed_kmh;
    uint16_t engine_rpm;
    uint8_t engine_load_percent;
    int16_t engine_coolant_temp_C;
    uint16_t fuel_level_percent;
    uint32_t odometer_km;
    bool ignition_on;
    bool brake_pressed;
    bool door_open;
    uint32_t last_update_ms;
} VehicleData_t;

void CANParser_Init(void);
void CANParser_ProcessMessage(const CAN_Message_t *msg);
VehicleData_t CANParser_GetVehicleData(void);

#endif // CAN_PARSER_H_