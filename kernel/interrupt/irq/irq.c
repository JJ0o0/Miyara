#include <interrupt/idt.h>
#include <interrupt/pic.h>
#include <log/log.h>

void irq_dispatch(u64 irq) {
    log_hex(LOG_DEBUG, "IRQ", irq);
    pic_send_eoi(irq);
}