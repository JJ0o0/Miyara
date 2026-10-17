#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <process/process.h>
#include <cpu/context.h>

void scheduler_init(void);
void scheduler_tick(void);
void scheduler_yield(void);
void scheduler_add_process(Process* process);

CPUContext* scheduler_schedule(CPUContext* context);
Process* scheduler_get_current(void);

#endif