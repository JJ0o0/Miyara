#include <memory/memory.h>

void* mem_set(void* dest, u8 value, size_t quantity) {
    u8* p_dest_byte = (u8*)dest;
    for (size_t i = 0; i < quantity; i++) {
        p_dest_byte[i] = value;
    }

    return dest;
}

void* mem_copy(void* dest, const void* src, size_t quantity) {
    u8* p_dest_byte = (u8*)dest;
    const u8* p_src_byte = (const u8*)src;
    for (size_t i = 0; i < quantity; i++) {
        p_dest_byte[i] = p_src_byte[i];
    }

    return dest;
}