#include <interrupt/idt.h>
#include <interrupt/pic.h>

#include <keyboard/keyboard.h>
#include <timer/timer.h>
#include <mouse/mouse.h>

void irq_dispatch(u64 irq) {
    switch (irq) {
        // TIMER
        case 0:
            timer_tick();
            break;
        // KEYBOARD
        case 1:
            keyboard_handle();
            break;
        // MOUSE
        case 12:
            mouse_handle();
            break;
    }
    
    pic_send_eoi(irq);
}