#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>

void CAN_Init(uint32_t baudrate);
void CAN_Send(uint32_t id, const uint8_t *data, uint8_t len);
void CAN_Read(uint32_t *id, uint8_t *data, uint8_t *len);

#endif // CAN_DRIVER_H
