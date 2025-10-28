#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#define CAN_MAX_DATA_LEN  64  // CAN-FD supports up to 64 bytes

typedef struct {
    uint32_t id;                      // CAN message ID
    uint8_t data[CAN_MAX_DATA_LEN];  // Data payload
    uint8_t len;                      // Data length (0-64)
    bool extended;                    // Extended ID (29-bit)
    bool fd_frame;                    // CAN-FD frame
    bool brs;                         // Bit rate switch
} CAN_Message_t;

typedef void (*CAN_RxCallback_t)(const CAN_Message_t *msg);

bool CAN_Init(void);
bool CAN_Send(const CAN_Message_t *msg);
bool CAN_Receive(CAN_Message_t *msg);
void CAN_SetRxCallback(CAN_RxCallback_t callback);
uint32_t CAN_GetRxCount(void);
uint32_t CAN_GetTxCount(void);
uint32_t CAN_GetErrorCount(void);

#endif // CAN_DRIVER_H_