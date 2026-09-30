#include <cpu/cpu.h>

void cpu_init(void) {
    // Enabling CR0.WP
    asm volatile (
        "mov %%cr0, %%rax\n"
        "or $0x10000, %%rax\n"
        "mov %%rax, %%cr0\n"
        :
        :
        : "rax", "memory"
    );
}