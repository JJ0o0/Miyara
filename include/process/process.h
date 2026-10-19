#ifndef PROCESS_H
#define PROCESS_H

#include <cpu/context.h>
#include <types/types.h>

/**
 * Size, in bytes, of the stack allocated for each new process by
 * process_create().
 */
#define PROCESS_STACK_SIZE        (16ULL * 1024)
#define USER_STACK_SIZE           (16ULL * 1024)
#define KERNEL_STACK_SIZE         (16ULL * 1024)

/**
 * Scheduling state of a process.
 */
typedef enum {
    /**
     * Eligible to run; waiting for the scheduler to pick it.
     */
    PROCESS_READY,

    /**
     * Currently running on the CPU.
     */
    PROCESS_RUNNING,

    /**
     * Not eligible to run until something wakes it; see
     * ProcessBlockReason for why.
     */
    PROCESS_BLOCKED,

    /**
     * Finished; process_exit() was called. The scheduler will not
     * run it again, but its Process struct is not freed.
     */
    PROCESS_TERMINATED
} ProcessState;

typedef enum {
    PROCESS_KERNEL,
    PROCESS_USER
} ProcessType;

/**
 * Why a PROCESS_BLOCKED process is blocked, so the right condition
 * wakes it back up.
 */
typedef enum {
    /**
     * Not blocked, or the reason no longer applies.
     */
    REASON_NONE,

    /**
     * Blocked in process_sleep(); woken by scheduler_tick() once
     * wake_tick is reached.
     */
    REASON_SLEEP,

    /**
     * Blocked in wait_event(); woken by add_event() when an event
     * arrives.
     */
    REASON_EVENT
} ProcessBlockReason;

/**
 * A single process (kernel-level task), including its CPU state and
 * the stack it runs on.
 *
 * Allocated and owned by process_create(); processes form a singly
 * linked list via next, maintained by the scheduler.
 */
typedef struct Process {
    /**
     * Process ID. Assigned sequentially by process_create(),
     * starting at 1 (PID 0 is reserved for the bootstrap process
     * that scheduler_init() creates for the code that was already
     * running before the scheduler started).
     */
    u64 pid;

    /**
     * Current scheduling state.
     */
    ProcessState state;
    ProcessType type;

    /**
     * Why this process is blocked. Only meaningful when
     * state == PROCESS_BLOCKED.
     */
    ProcessBlockReason block_reason;

    /**
     * Tick count at which this process should wake up. Only
     * meaningful when block_reason == REASON_SLEEP.
     */
    u64 wake_tick;

    /**
     * Entry point the process runs, via process_trampoline(). Not
     * used after the process starts running.
     */
    void (*entry)(void);

    /**
     * Saved CPU state, used by the scheduler to resume this process.
     * NULL for the bootstrap process, which has no process-owned
     * stack of its own.
     */
    CPUContext* context;

    /**
     * Base address of the heap-allocated stack given to this
     * process by process_create(). NULL for the bootstrap process.
     */
    void* stack_base;

    /**
     * Size of stack_base, in bytes (PROCESS_STACK_SIZE for every
     * process created by process_create()).
     */
    u64 stack_size;

    void* kernel_stack_base;
    u64 kernel_stack_size;

    /**
     * Next process in the scheduler's process list, or NULL if this
     * is the last one.
     */
    struct Process* next;
} Process;

/**
 * Creates a new process that will run entry when scheduled.
 *
 * Allocates the Process struct and its stack on the kernel heap, and
 * sets up an initial CPUContext so the process starts at
 * process_trampoline() (which calls entry and then process_exit()).
 * The new process is created in the PROCESS_READY state, but is not
 * added to the scheduler: pass it to scheduler_add_process()
 * afterwards.
 *
 * @param entry Function the process will run. Must not be NULL.
 * @return The new process, or NULL if entry is NULL or allocation
 *         failed (process or stack).
 */
Process* process_create(void (*entry)(void));

Process* process_create_user(void (*entry)(void));

/**
 * Terminates the currently running process.
 *
 * Marks it PROCESS_TERMINATED and yields to the scheduler, which
 * will never run it again. Never returns to the caller.
 *
 * Panics if there is no current process (should not happen once the
 * scheduler is initialized).
 */
void process_exit(void);

/**
 * Blocks the currently running process and yields to the scheduler.
 *
 * The process stays PROCESS_BLOCKED until something calls
 * process_wake() on it. Its block_reason is left as whatever the
 * caller set beforehand (REASON_NONE by default).
 *
 * Panics if there is no current process.
 */
void process_block(void);

/**
 * Makes a blocked process eligible to run again.
 *
 * Only has an effect if process is currently PROCESS_BLOCKED, in
 * which case it becomes PROCESS_READY. Does not itself clear
 * block_reason or yield the CPU.
 *
 * @param process Process to wake. Does nothing if NULL.
 */
void process_wake(Process* process);

/**
 * Blocks the currently running process for at least the given
 * duration, then yields to the scheduler.
 *
 * A request of 0 milliseconds just yields once, without blocking.
 * Otherwise, sets block_reason to REASON_SLEEP and wake_tick to the
 * tick count after which scheduler_tick() will wake the process.
 * Because ticks only advance while the timer IRQ fires, the actual
 * wait can run a bit long but is never shorter than requested.
 *
 * Panics if there is no current process, or if the timer has not
 * been initialized yet (pit_init() has not run).
 *
 * @param milliseconds Minimum time to sleep, in milliseconds.
 */
void process_sleep(u64 milliseconds);

/**
 * Entry point every process actually starts at.
 *
 * Looks up the current process and calls its entry function, then
 * calls process_exit() when it returns. Exists so process_create()
 * has a single, known address to put in the initial CPUContext's
 * RIP, rather than jumping to an arbitrary user-supplied function
 * directly.
 *
 * @param entry Unused parameter list — process_trampoline takes no
 *              arguments; the process it runs is found via
 *              scheduler_get_current().
 */
void process_trampoline(void);

#endif