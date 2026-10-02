#include <timer/pit.h>
#include <timer/timer.h>
#include <io/io.h>

#define PIT_BASE_FREQUENCY 1193182u
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

void pit_init(u16 frequency) {
    if (frequency == 0) {
        return;
    }

    timer_set_frequency(frequency);

    // Command Byte
    io_out8(PIT_COMMAND, 0x36);

    // Divisor
    u16 divisor = PIT_BASE_FREQUENCY / frequency;
    io_out8(PIT_CHANNEL0, divisor & 0xFF);
    io_out8(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}
