#ifndef TASK_SCHEDULER_H_
#define TASK_SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>

/* ---------------- Task Configuration Constants ---------------- */
#define SYSTEM_TICK_MS          10      // Base scheduler tick in milliseconds

#define TASK_PERIOD_CAN         100     // CAN processing interval (ms)
#define TASK_PERIOD_IMU         50      // IMU reading interval (ms)
#define TASK_PERIOD_GPS         1000    // GPS update interval (ms)
#define TASK_PERIOD_POWER       500     // Power monitor interval (ms)
#define TASK_PERIOD_TELEMETRY   2000    // Telemetry logging interval (ms)
#define TASK_PERIOD_DATA_TX     1000    // Data transmit interval (ms)
#define TASK_PERIOD_MODEM       500     // Modem update interval (ms)

/* ---------------- Task Function Type Definition ---------------- */
typedef void (*TaskFunction_t)(void);

/* ---------------- Task Identifiers ---------------- */
typedef enum {
    TASK_CAN_PROCESS = 0,
    TASK_IMU_READ,
    TASK_GPS_UPDATE,
    TASK_POWER_MONITOR,
    TASK_TELEMETRY_LOG,
    TASK_DATA_TRANSMIT,
    TASK_MODEM_UPDATE,
    TASK_MAX
} TaskID_t;

/* ---------------- Task Structure Definition ---------------- */
typedef struct {
    TaskFunction_t function;   // Task function pointer
    uint32_t period_ms;        // Task period in milliseconds
    uint32_t last_run_ms;      // Timestamp of last run
    bool enabled;              // Task enable flag
} Task_t;

/* ---------------- Function Prototypes ---------------- */
void Scheduler_Init(void);
void Scheduler_RegisterTask(TaskID_t id, TaskFunction_t func, uint32_t period_ms);
void Scheduler_EnableTask(TaskID_t id, bool enable);
void Scheduler_Run(void);
uint32_t Scheduler_GetSystemTime_ms(void);

#endif // TASK_SCHEDULER_H_
