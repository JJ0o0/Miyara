#include <mouse/mouse.h>
#include <event/event.h>
#include <io/ps2.h>

#define MOUSE_CMD_RESET                     0xFF
#define MOUSE_CMD_ENABLE_DATA_REPORTING     0xF4

#define MOUSE_RESPONSE_ACK                  0xFA
#define MOUSE_RESPONSE_SELF_TEST_OK         0xAA

#define MOUSE_ID_STANDARD                   0x00

#define MOUSE_PACKET_SIZE                   3
#define MOUSE_PACKET_LEFT_BUTTON            0x01
#define MOUSE_PACKET_RIGHT_BUTTON           0x02
#define MOUSE_PACKET_MIDDLE_BUTTON          0x04
#define MOUSE_PACKET_X_SIGN                 0x10
#define MOUSE_PACKET_Y_SIGN                 0x20
#define MOUSE_PACKET_X_OVERFLOW             0x40
#define MOUSE_PACKET_Y_OVERFLOW             0x80
#define MOUSE_PACKET_SIGN_OFFSET            256
#define MOUSE_PACKET_ALWAYS_ONE             0x08

static u8 packet[MOUSE_PACKET_SIZE];
static u8 packet_index = 0;

static bool mouse_send_command(u8 command);
static bool mouse_reset(void);

void mouse_init(void) {
    if (!mouse_reset()) {
        return;
    }

    if (!mouse_send_command(MOUSE_CMD_ENABLE_DATA_REPORTING)) {
        return;
    }
}

void mouse_handle(void) {
    u8 data = ps2_read_data_now();
    if (packet_index == 0 && !(data & MOUSE_PACKET_ALWAYS_ONE)) {
        return;
    }
    
    packet[packet_index] = data;
    packet_index++;

    if (packet_index < MOUSE_PACKET_SIZE) {
        return;
    }

    if (packet[0] & (MOUSE_PACKET_X_OVERFLOW | MOUSE_PACKET_Y_OVERFLOW)) {
        packet_index = 0;
        return;
    }

    MouseEvent event;
    event.buttons.left = (packet[0] & MOUSE_PACKET_LEFT_BUTTON) != 0;
    event.buttons.right = (packet[0] & MOUSE_PACKET_RIGHT_BUTTON) != 0;
    event.buttons.middle = (packet[0] & MOUSE_PACKET_MIDDLE_BUTTON) != 0;
    
    event.dx = packet[1];
    if (packet[0] & MOUSE_PACKET_X_SIGN) {
        event.dx -= MOUSE_PACKET_SIGN_OFFSET;
    }

    event.dy = packet[2];
    if (packet[0] & MOUSE_PACKET_Y_SIGN) {
        event.dy -= MOUSE_PACKET_SIGN_OFFSET;
    }

    Event system_event = {
        .type = EVENT_MOUSE,
        .data.mouse = event
    };
    
    add_event(&system_event);

    packet_index = 0;
}

static bool mouse_send_command(u8 command) {
    if (!ps2_write_second_port(command)) {
        return false;
    }

    u8 response;
    if (!ps2_read_data(&response)) {
        return false;
    }

    return response == MOUSE_RESPONSE_ACK;
}

static bool mouse_reset(void) {
    if (!mouse_send_command(MOUSE_CMD_RESET)) {
        return false;
    }

    // Self Test Byte
    u8 byte;
    if (!ps2_read_data(&byte)) {
        return false;
    }

    if (byte != MOUSE_RESPONSE_SELF_TEST_OK) {
        return false;
    }

    // Device ID Byte
    if (!ps2_read_data(&byte)) {
        return false;
    }

    if (byte != MOUSE_ID_STANDARD) {
        return false;
    }

    return true;
}