#ifndef MODEM_MANAGER_H_
#define MODEM_MANAGER_H_

void Modem_Init(void);

#endif
#ifndef MODEM_MANAGER_H_
#define MODEM_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MODEM_STATE_OFF,
    MODEM_STATE_POWERING_ON,
    MODEM_STATE_INITIALIZING,
    MODEM_STATE_READY,
    MODEM_STATE_CONNECTING,
    MODEM_STATE_CONNECTED,
    MODEM_STATE_TRANSMITTING,
    MODEM_STATE_ERROR
} ModemState_t;

typedef struct {
    double latitude;
    double longitude;
    float altitude;
    float speed_kmh;
    uint8_t satellites;
    bool fix_valid;
    uint32_t timestamp;
} GPS_Data_t;

typedef struct {
    ModemState_t state;
    int8_t rssi;              // Signal strength (dBm)
    uint8_t network_reg;      // Network registration status
    bool gps_active;
    uint32_t bytes_sent;
    uint32_t bytes_received;
} ModemStatus_t;

void Modem_Init(void);
void Modem_Update(void);
bool Modem_PowerOn(void);
bool Modem_PowerOff(void);
bool Modem_SendAT(const char *cmd, char *response, uint16_t max_len, uint32_t timeout_ms);
bool Modem_ConnectNetwork(void);
bool Modem_SendData(const uint8_t *data, uint16_t len);
bool Modem_GetGPS(GPS_Data_t *gps);
ModemStatus_t Modem_GetStatus(void);

#endif // MODEM_MANAGER_H_