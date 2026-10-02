#include <io/ps2.h>
#include <io/io.h>

#include <timer/timer.h>

#define PS2_IO_DATA 0x60
#define PS2_IO_STATUS_COMMAND 0x64

#define PS2_TIMEOUT_MS 10UL

bool ps2_wait_input(void) {
    u64 start = timer_get_ticks();
    u64 timeout = timer_ticks_from_ms(PS2_TIMEOUT_MS);

    while (io_in8(PS2_IO_STATUS_COMMAND) & 0x02) {
        u64 elapsed = timer_get_ticks() - start;
        if (elapsed >= timeout) {
            return false;
        }
    }
    
    return true;
}

bool ps2_wait_output(void) {
    u64 start = timer_get_ticks();
    u64 timeout = timer_ticks_from_ms(PS2_TIMEOUT_MS);

    while (!(io_in8(PS2_IO_STATUS_COMMAND) & 0x01)) {
        u64 elapsed = timer_get_ticks() - start;
        if (elapsed >= timeout) {
            return false;
        }
    }

    return true;
}

void ps2_write_command(u8 command) {
    if (!ps2_wait_input()) {
        return;
    }

    io_out8(PS2_IO_STATUS_COMMAND, command);
}

void ps2_write_data(u8 data) {
    if (!ps2_wait_input()) {
        return;
    }

    io_out8(PS2_IO_DATA, data);
}

bool ps2_read_data(u8* data) {
    if (!ps2_wait_output()) {
        return false;
    }

    *data = io_in8(PS2_IO_DATA);
    return true;
}
