#include "task_scheduler.h"
#include "config.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_timer.h>
#include <string.h>

static Task_t tasks[TASK_MAX];
static volatile uint32_t system_time_ms = 0;

// Timer interrupt for system tick
void TIMER_0_IRQHandler(void)
{
    system_time_ms += SYSTEM_TICK_MS;
}

void Scheduler_Init(void)
{
    memset(tasks, 0, sizeof(tasks));
    system_time_ms = 0;
    
    // Enable system tick timer (configured in SysConfig)
    DL_Timer_startCounter(TIMER_0_INST);
}

void Scheduler_RegisterTask(TaskID_t id, TaskFunction_t func, uint32_t period_ms)
{
    if (id >= TASK_MAX || func == NULL) {
        return;
    }
    
    tasks[id].function = func;
    tasks[id].period_ms = period_ms;
    tasks[id].last_run_ms = 0;
    tasks[id].enabled = true;
}

void Scheduler_EnableTask(TaskID_t id, bool enable)
{
    if (id >= TASK_MAX) {
        return;
    }
    
    tasks[id].enabled = enable;
}

void Scheduler_Run(void)
{
    uint32_t current_time = Scheduler_GetSystemTime_ms();
    
    for (int i = 0; i < TASK_MAX; i++) {
        if (tasks[i].function == NULL || !tasks[i].enabled) {
            continue;
        }
        
        // Check if it's time to run this task
        if ((current_time - tasks[i].last_run_ms) >= tasks[i].period_ms) {
            tasks[i].function();
            tasks[i].last_run_ms = current_time;
        }
    }
}

uint32_t Scheduler_GetSystemTime_ms(void)
{
    return system_time_ms;
}