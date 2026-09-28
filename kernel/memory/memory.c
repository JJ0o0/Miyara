#include <memory/memory.h>

void* mem_set(void* dest, u8 value, size_t quantity) {
    u8* p_dest_byte = (u8*)dest;
    for (size_t i = 0; i < quantity; i++) {
        p_dest_byte[i] = value;
    }

    return dest;
}