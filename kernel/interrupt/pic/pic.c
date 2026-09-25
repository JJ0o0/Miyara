#include <interrupt/pic.h>
#include <io/io.h>

void pic_init(void) {
    // ICW1
    io_out8(PIC_MASTER_COMMAND, 0x11);
    io_out8(PIC_SLAVE_COMMAND, 0x11);

    // ICW2
    io_out8(PIC_MASTER_DATA, 0x20);
    io_out8(PIC_SLAVE_DATA, 0x28);

    // ICW3
    io_out8(PIC_MASTER_DATA, 0x04);
    io_out8(PIC_SLAVE_DATA, 0x02);

    // ICW4
    io_out8(PIC_MASTER_DATA, 0x01);
    io_out8(PIC_SLAVE_DATA, 0x01);

    // IMR Setup
    io_out8(PIC_MASTER_DATA, 0xF8); // Timer, Keyboand and Slave Bridge.
    io_out8(PIC_SLAVE_DATA, 0xFF);  // Nothing.
}

void pic_send_eoi(u8 irq) {
    io_out8(PIC_MASTER_COMMAND, 0x20);
    if (irq >= 8) io_out8(PIC_SLAVE_COMMAND, 0x20);
}
