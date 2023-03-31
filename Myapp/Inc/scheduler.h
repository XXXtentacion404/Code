#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include "gpio.h"
typedef struct
{
void(*task_func)(void);
uint16_t rate_hz;
uint16_t interval_ticks;
uint32_t last_run;
}sched_task_t;


#define TICK_PER_SECOND	1000
#define TICK_US	(1000000/TICK_PER_SECOND)

void Scheduler_Setup(void);
void Scheduler_Run(void);

#endif
