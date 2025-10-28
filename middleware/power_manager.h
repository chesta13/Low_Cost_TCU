#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H


#include <stdint.h>
#include <stdbool.h>


void Power_Init(void);
bool Power_IsCharging(void);
void Power_EnableSwitch(bool enable);
uint32_t Power_ReadVbat_mV(void);
uint32_t Power_ReadVsup_mV(void);
void Power_RequestShutdown(void);


#endif // POWER_MANAGER_H