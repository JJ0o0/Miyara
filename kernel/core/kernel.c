#include <multiboot/multiboot.h>

#include <graphics/framebuffer.h>
#include <graphics/renderer.h>
#include <graphics/cursor.h>
#include <graphics/graphic_terminal.h>
#include <graphics/font_builtin.h>
#include <graphics/font.h>

#include <terminal/terminal.h>
#include <shell/shell.h>

#include <interrupt/idt.h>
#include <interrupt/pic.h>

#include <timer/timer.h>
#include <timer/pit.h>

#include <types/types.h>

#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/pmm.h>

#include <log/log.h>
#include <cpu/cpu.h>

#include <event/event.h>
#include <mouse/mouse.h>
#include <keyboard/keymap.h>
#include <io/ps2.h>
#include <io/io.h>

void kernel_main(u64 multiboot_info_address) {
    Terminal term = create_terminal();
    log_init(&term);

    log(LOG_SUCCESS, "Initialized terminal.");

    log(LOG_INFO, "Initializing PMM...");
        MBIHeader* mbi = (MBIHeader*)multiboot_info_address;
        pmm_init(mbi);
    log(LOG_SUCCESS, "Initialized PMM.");

    log(LOG_INFO, "Initializing Framebuffer...");
        framebuffer_init(mbi);
    log(LOG_SUCCESS, "Initialized Framebuffer.");

    log(LOG_INFO, "Initializing Paging...");
        paging_init();
    log(LOG_SUCCESS, "Initialized Paging.");

    log(LOG_INFO, "Mapping Framebuffer...");
        framebuffer_map();
    log(LOG_SUCCESS, "Mapped Framebuffer.");

    log(LOG_INFO, "Initializing Heap...");
        heap_init();
    log(LOG_SUCCESS, "Initialized Heap.");

    log(LOG_INFO, "Initializing Renderer...");
        if (!renderer_init()) {
            log(LOG_ERROR, "Error while initializing Renderer!");
            while (1) { }
        }
    log(LOG_SUCCESS, "Initialized Renderer.");

    log(LOG_INFO, "Initializing PIC...");
        pic_init();
    log(LOG_SUCCESS, "Initialized PIC.");

    log(LOG_INFO, "Initializing IDT...");
        idt_init();
    log(LOG_SUCCESS, "Initialized IDT.");

    log(LOG_INFO, "Initializing CPU...");
        cpu_init();
    log(LOG_SUCCESS, "Initialized CPU.");

    log(LOG_INFO, "Initializing PIT...");
        pit_init(100);
    log(LOG_SUCCESS, "Initialized PIT.");

    log(LOG_INFO, "Initializing PS/2 ports...");
        ps2_init();
    log(LOG_SUCCESS, "Initialized PS/2 ports.");

    log(LOG_INFO, "Initializing Keyboard...");
        if (!keyboard_init()) {
            log(LOG_ERROR, "Error while initializing Keyboard!");
            while (1) { }
        }
    log(LOG_SUCCESS, "Initialized Keyboard.");

    log(LOG_INFO, "Initializing Mouse...");
        mouse_init();
    log(LOG_SUCCESS, "Initialized Mouse.");

    log(LOG_INFO, "Enabling Interrupts...");
        enable_interrupts();
    log(LOG_SUCCESS, "Enabled Interrupts.");

    renderer_clear((Color){23, 23, 23});
        GraphicTerminal graphic_terminal;
        GraphicTerminalConfig graphic_terminal_config = {
            .origin = {
                (renderer_get_width() - 600) / 2,
                (renderer_get_height() - 400) / 2
            },
            .size = {600, 400},
            .font = &font8x8_basic_font,
            .scale = 2,
            .foreground = {255, 255, 255},
            .background = {13, 13, 13}
        };

        graphic_terminal_init(&graphic_terminal, &graphic_terminal_config);
        graphic_terminal_clear(&graphic_terminal);

        Shell shell;
        shell_init(&shell, &graphic_terminal);
    renderer_present();

    log(LOG_INFO, "Initializing Cursor...");
        cursor_init();
    log(LOG_SUCCESS, "Initialized Cursor.");

    term_write(&term, "\nWelcome to Miyara\n");

    while (1) {
        Event event;

        if (!get_event(&event)) {
            continue;
        }

        switch (event.type) {
            case EVENT_MOUSE:
                cursor_handle_mouse(event.data.mouse);
                break;
            case EVENT_KEYBOARD: {
                shell_handle_key(&shell, event.data.keyboard);
                renderer_present_rect(graphic_terminal.origin, graphic_terminal.size);
                break;
            }
        }
    }
}