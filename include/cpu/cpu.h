#ifndef CPU_H
#define CPU_H

/**
 * Applies early per-CPU configuration that must be set before the
 * kernel relies on it.
 *
 * Currently sets CR0.WP (Write Protect, bit 16), so the CPU enforces
 * read-only page mappings even while running in ring 0. Without this
 * bit, the kernel could silently write through a page marked
 * non-writable instead of faulting.
 */
void cpu_init(void);

#endif