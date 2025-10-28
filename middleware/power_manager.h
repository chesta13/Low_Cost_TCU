#ifndef POWER_MANAGER_H_
#define POWER_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    POWER_STATE_STARTUP,
    POWER_STATE_NORMAL,
    POWER_STATE_LOW_BATTERY,
    POWER_STATE_CHARGING,
    POWER_STATE_SHUTDOWN,
    POWER_STATE_ERROR
} PowerState_t;

typedef struct {
    uint32_t vsup_mV;           // Vehicle supply voltage
    uint32_t vbat_mV;           // Backup battery voltage
    int16_t temperature_C;      // System temperature
    bool ignition_on;           // IGN signal status
    bool charging;              // Battery charging status
    bool modem_enabled;         // Modem power state
    PowerState_t state;         // Current power state
    uint32_t uptime_sec;        // System uptime in seconds
} PowerStatus_t;

void Power_Init(void);
void Power_Update(void);
PowerStatus_t Power_GetStatus(void);
void Power_EnableModem(bool enable);
void Power_EnableSwitch(bool enable);
void Power_RequestShutdown(void);
bool Power_IsVoltageOK(void);
bool Power_IsTemperatureOK(void);
const char* Power_GetStateString(void);

#endif // POWER_MANAGER_H_