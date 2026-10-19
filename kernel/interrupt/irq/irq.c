#include <interrupt/idt.h>
#include <interrupt/pic.h>

#include <keyboard/keyboard.h>
#include <timer/timer.h>
#include <mouse/mouse.h>
#include <event/event.h>

#include <scheduler/scheduler.h>

static u64 cursor_tick_interval = 0;
static u64 next_cursor_tick = 0;

CPUContext* irq_dispatch(u64 irq, CPUContext* context) {
    switch (irq) {
        // TIMER
        case 0:
            timer_tick();

            u64 now = timer_get_ticks();
            if (next_cursor_tick == 0) {
                cursor_tick_interval = timer_ticks_from_ms(500);
                next_cursor_tick = now + cursor_tick_interval;
            }

            if (now >= next_cursor_tick) {
                Event event = {
                    .type = EVENT_CURSOR_TICK
                };

                add_event(&event);

                next_cursor_tick = now + cursor_tick_interval;
            }

            scheduler_tick();
            context = scheduler_schedule(context);
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
    return context;
}