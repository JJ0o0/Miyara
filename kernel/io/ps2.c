#include <io/ps2.h>
#include <io/io.h>

#define PS2_IO_DATA                 0x60
#define PS2_IO_STATUS               0x64

#define PS2_STATUS_OUTPUT_FULL      0x01
#define PS2_STATUS_INPUT_FULL       0x02

#define PS2_CMD_READ_CONFIG         0x20
#define PS2_CMD_WRITE_CONFIG        0x60

#define PS2_CMD_ENABLE_SECOND_PORT  0xA8
#define PS2_CMD_WRITE_SECOND_PORT   0xD4

#define PS2_TIMEOUT_ITERATIONS      10000UL

bool ps2_wait_input(void) {
    for (u64 tries = 0; tries < PS2_TIMEOUT_ITERATIONS; tries++) {
        if (!(io_in8(PS2_IO_STATUS) & PS2_STATUS_INPUT_FULL)) {
            return true;
        }
    }

    return false;
}

bool ps2_wait_output(void) {
    for (u64 tries = 0; tries < PS2_TIMEOUT_ITERATIONS; tries++) {
        if (io_in8(PS2_IO_STATUS) & PS2_STATUS_OUTPUT_FULL) {
            return true;
        }
    }

    return false;
}

bool ps2_write_command(u8 command) {
    if (!ps2_wait_input()) {
        return false;
    }

    io_out8(PS2_IO_STATUS, command);
    return true;
}

bool ps2_write_data(u8 data) {
    if (!ps2_wait_input()) {
        return false;
    }

    io_out8(PS2_IO_DATA, data);
    return true;
}

bool ps2_read_data(u8* data) {
    if (!ps2_wait_output()) {
        return false;
    }

    *data = io_in8(PS2_IO_DATA);
    return true;
}

bool ps2_read_config(u8* config) {
    if (!ps2_write_command(PS2_CMD_READ_CONFIG)) {
        return false;
    }

    return ps2_read_data(config);
}

bool ps2_write_config(u8 config) {
    if (!ps2_write_command(PS2_CMD_WRITE_CONFIG)) {
        return false;
    }

    return ps2_write_data(config);
}

bool ps2_enable_second_port(void) {
    return ps2_write_command(PS2_CMD_ENABLE_SECOND_PORT);
}

bool ps2_write_second_port(u8 data) {
    if (!ps2_write_command(PS2_CMD_WRITE_SECOND_PORT)) {
        return false;
    }

    return ps2_write_data(data);
}