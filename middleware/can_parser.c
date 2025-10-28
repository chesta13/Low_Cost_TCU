#include "can_parser.h"
#include "uart_driver.h"
#include <string.h>
#include <stdio.h>

static VehicleData_t vehicle_data;

// OBD-II/J1939 message IDs
#define CAN_ID_ENGINE_SPEED     0x0C
#define CAN_ID_VEHICLE_SPEED    0x0D
#define CAN_ID_COOLANT_TEMP     0x05
#define CAN_ID_ENGINE_LOAD      0x04
#define CAN_ID_FUEL_LEVEL       0x2F

void CANParser_Init(void)
{
    memset(&vehicle_data, 0, sizeof(VehicleData_t));
}

void CANParser_ProcessMessage(const CAN_Message_t *msg)
{
    if (msg == NULL) {
        return;
    }
    
    // Update timestamp
    vehicle_data.last_update_ms++;  // Should use actual system time
    
    // Parse based on CAN ID
    switch (msg->id) {
        case 0x100:  // Example: Vehicle speed
            if (msg->len >= 2) {
                vehicle_data.vehicle_speed_kmh = (msg->data[0] << 8) | msg->data[1];
            }
            break;
            
        case 0x200:  // Example: Engine RPM
            if (msg->len >= 2) {
                vehicle_data.engine_rpm = (msg->data[0] << 8) | msg->data[1];
            }
            break;
            
        case 0x300:  // Example: Engine parameters
            if (msg->len >= 4) {
                vehicle_data.engine_load_percent = msg->data[0];
                vehicle_data.engine_coolant_temp_C = (int16_t)msg->data[1] - 40;
                vehicle_data.fuel_level_percent = msg->data[2];
            }
            break;
            
        case 0x400:  // Example: Status bits
            if (msg->len >= 1) {
                vehicle_data.ignition_on = (msg->data[0] & 0x01) != 0;
                vehicle_data.brake_pressed = (msg->data[0] & 0x02) != 0;
                vehicle_data.door_open = (msg->data[0] & 0x04) != 0;
            }
            break;
            
        default:
            // Unknown message - log it
            break;
    }
}

VehicleData_t CANParser_GetVehicleData(void)
{
    return vehicle_data;
}