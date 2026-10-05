#include <io/ps2.h>
#include <io/io.h>

#define PS2_IO_DATA                  0x60
#define PS2_IO_STATUS                0x64

#define PS2_STATUS_OUTPUT_FULL       0x01
#define PS2_STATUS_INPUT_FULL        0x02

#define PS2_CMD_READ_CONFIG          0x20
#define PS2_CMD_WRITE_CONFIG         0x60

#define PS2_CMD_ENABLE_FIRST_PORT    0xAE
#define PS2_CMD_DISABLE_FIRST_PORT   0xAD

#define PS2_CMD_ENABLE_SECOND_PORT   0xA8
#define PS2_CMD_DISABLE_SECOND_PORT  0xA7
#define PS2_CMD_WRITE_SECOND_PORT    0xD4

#define PS2_CMD_CONTROLLER_SELF_TEST 0xAA
#define PS2_CMD_FIRST_PORT_TEST      0xAB
#define PS2_CMD_SECOND_PORT_TEST     0xA9

#define PS2_SELF_TEST_OK             0x55
#define PS2_FIRST_PORT_TEST_OK       0x00
#define PS2_SECOND_PORT_TEST_OK      0x00

#define PS2_CONFIG_FIRST_PORT_INTERRUPT         0x01
#define PS2_CONFIG_SECOND_PORT_INTERRUPT        0x02
#define PS2_CONFIG_SYSTEM_FLAG                  0x04
#define PS2_CONFIG_FIRST_PORT_CLOCK_DISABLED    0x10
#define PS2_CONFIG_SECOND_PORT_CLOCK_DISABLED   0x20
#define PS2_CONFIG_FIRST_PORT_TRANSLATION       0x40

#define PS2_TIMEOUT_ITERATIONS       10000UL

static void ps2_flush(void);

void ps2_init(void) {
    // Disable PS/2 ports
    if (!ps2_disable_first_port()) {
        return;
    }

    if (!ps2_disable_second_port()) {
        return;
    }

    // Flush controller output buffer
    ps2_flush();

    // Configure controller initialization
    u8 config;
    if (!ps2_read_config(&config)) {
        return;
    }

    config &= ~(PS2_CONFIG_FIRST_PORT_INTERRUPT | PS2_CONFIG_SECOND_PORT_INTERRUPT);

    if (!ps2_write_config(config)) {
        return;
    }

    // Controller self test
    if (!ps2_controller_self_test()) {
        return;
    }

    // Restore configuration after self test
    if (!ps2_read_config(&config)) {
        return;
    }

    config &= ~(PS2_CONFIG_FIRST_PORT_INTERRUPT | PS2_CONFIG_SECOND_PORT_INTERRUPT);

    if (!ps2_write_config(config)) {
        return;
    }

    // Test first PS/2 port
    if (!ps2_test_first_port()) {
        return;
    }

    // Detect second PS/2 port
    if (!ps2_enable_second_port()) {
        return;
    }
    
    if (!ps2_read_config(&config)) {
        return;
    }

    bool has_second_port = (config & PS2_CONFIG_SECOND_PORT_CLOCK_DISABLED) == 0;
    if (!ps2_disable_second_port()) {
        return;
    }

    // Test second PS/2 port
    if (has_second_port) {
        if (!ps2_test_second_port()) {
            return;
        }
    }

    // Final controller configuration
    if (!ps2_enable_first_port()) {
        return;
    }

    if (has_second_port) {
        if (!ps2_enable_second_port()) {
            return;
        }
    }

    if (!ps2_read_config(&config)) {
        return;
    }

    config |= PS2_CONFIG_FIRST_PORT_INTERRUPT;

    if (has_second_port) {
        config |= PS2_CONFIG_SECOND_PORT_INTERRUPT;
    } else {
        config &= ~PS2_CONFIG_SECOND_PORT_INTERRUPT;
    }

    if (!ps2_write_config(config)) {
        return;
    }
}

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

bool ps2_data_available(void) {
    return (io_in8(PS2_IO_STATUS) & PS2_STATUS_OUTPUT_FULL) != 0;
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

bool ps2_enable_first_port(void) {
    return ps2_write_command(PS2_CMD_ENABLE_FIRST_PORT);
}

bool ps2_disable_first_port(void) {
    return ps2_write_command(PS2_CMD_DISABLE_FIRST_PORT);
}

bool ps2_write_first_port(u8 data) {
    return ps2_write_data(data);
}

bool ps2_enable_second_port(void) {
    return ps2_write_command(PS2_CMD_ENABLE_SECOND_PORT);
}

bool ps2_disable_second_port(void) {
    return ps2_write_command(PS2_CMD_DISABLE_SECOND_PORT);
}

bool ps2_write_second_port(u8 data) {
    if (!ps2_write_command(PS2_CMD_WRITE_SECOND_PORT)) {
        return false;
    }

    return ps2_write_data(data);
}

bool ps2_controller_self_test(void) {
    if (!ps2_write_command(PS2_CMD_CONTROLLER_SELF_TEST)) {
        return false;
    }

    u8 response;
    if (!ps2_read_data(&response)) {
        return false;
    }

    return response == PS2_SELF_TEST_OK;
}

bool ps2_test_first_port(void) {
    if (!ps2_write_command(PS2_CMD_FIRST_PORT_TEST)) {
        return false;
    }

    u8 response;
    if (!ps2_read_data(&response)) {
        return false;
    }

    return response == PS2_FIRST_PORT_TEST_OK;
}

bool ps2_test_second_port(void) {
    if (!ps2_write_command(PS2_CMD_SECOND_PORT_TEST)) {
        return false;
    }

    u8 response;
    if (!ps2_read_data(&response)) {
        return false;
    }

    return response == PS2_SECOND_PORT_TEST_OK;
}

static void ps2_flush(void) {
    while (ps2_data_available()) {
        u8 data;

        if (!ps2_read_data(&data)) {
            break;
        }
    }
}