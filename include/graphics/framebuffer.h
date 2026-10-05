#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <multiboot/multiboot.h>
#include <types/types.h>

typedef struct {
    u64 address;
    u32 width;
    u32 height;
    u32 pitch;
    u8  bpp;
} Framebuffer;

bool framebuffer_init(MBIHeader* mbi);

#endif