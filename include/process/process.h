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

typedef struct Process {
    u64 pid;
    ProcessState state;

    void (*entry)(void);

    CPUContext* context;

    void* stack_base;
    u64 stack_size;

    struct Process* next;
} Process;

Process* process_create(void (*entry)(void));
void process_exit(void);
void process_trampoline(void);

#endif