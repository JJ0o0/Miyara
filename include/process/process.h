#ifndef PROCESS_H
#define PROCESS_H

#include <cpu/context.h>
#include <types/types.h>

#define PROCESS_STACK_SIZE (16ULL * 1024)

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} ProcessState;

typedef enum {
    REASON_NONE,
    REASON_SLEEP,
    REASON_EVENT
} ProcessBlockReason;

typedef struct Process {
    u64 pid;
    ProcessState state;
    
    ProcessBlockReason block_reason;
    u64 wake_tick;

    void (*entry)(void);

    CPUContext* context;

    void* stack_base;
    u64 stack_size;

    struct Process* next;
} Process;

Process* process_create(void (*entry)(void));
void process_exit(void);
void process_block(void);
void process_wake(Process* process);
void process_sleep(u64 milliseconds);
void process_trampoline(void);

#endif