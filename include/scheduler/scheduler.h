#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <process/process.h>
#include <cpu/context.h>

/**
 * Initializes the scheduler.
 *
 * Sets up the bootstrap process (PID 0, representing the code
 * already running before the scheduler started — it has no
 * process-owned stack or saved context) as the current process, and
 * creates the idle process (which just halts in a loop, run only
 * when nothing else is PROCESS_READY).
 *
 * Must be called once, after heap_init() (process_create() uses
 * k_malloc()) and before any other scheduler process call.
 * Panics if the idle process cannot be created.
 */
void scheduler_init(void);

/**
 * Wakes any process whose sleep has expired.
 *
 * Meant to be called once per timer tick (from irq_dispatch(), IRQ
 * 0), after timer_tick(). Scans every process and, for any that is
 * PROCESS_BLOCKED with block_reason == REASON_SLEEP and whose
 * wake_tick has been reached, marks it PROCESS_READY and clears
 * block_reason.
 */
void scheduler_tick(void);

/**
 * Gives up the CPU, letting the scheduler run another process.
 *
 * Triggers a context switch (scheduler_yield_asm, in assembly) which
 * eventually calls scheduler_schedule() with the current CPU state.
 * Returns once this process is scheduled to run again.
 */
void scheduler_yield(void);

/**
 * Adds a process to the scheduler's run list.
 *
 * Appends it at the end of the (singly linked) process list. The
 * process must have been created with process_create() and not
 * already be in the list.
 *
 * @param process Process to add. Does nothing if NULL.
 */
void scheduler_add_process(Process* process);

void scheduler_reap_terminated(void);

/**
 * Picks the next process to run and returns the CPU state to resume.
 *
 * Called from the interrupt path (via scheduler_yield_asm or a timer
 * IRQ) with the CPU state that was just saved. Saves that state into
 * the outgoing process (demoting it from PROCESS_RUNNING to
 * PROCESS_READY unless it already changed state, e.g. blocked or
 * terminated), then picks the next PROCESS_READY process in the
 * list, round-robin from the one after the current, falling back to
 * the idle process if nothing else is ready.
 *
 * @param context CPU state of the process being switched away from.
 *                Returned unchanged if it is NULL, if there is no
 *                other process to switch to, or if the chosen
 *                process has no saved context yet.
 * @return CPU state to resume: the incoming process's saved context,
 *         or the given context if no switch happened.
 */
CPUContext* scheduler_schedule(CPUContext* context);

/**
 * Returns the process currently running on the CPU.
 *
 * @return The current process, or NULL if the scheduler has not
 *         been initialized yet (scheduler_init() has not run).
 */
Process* scheduler_get_current(void);

#endif