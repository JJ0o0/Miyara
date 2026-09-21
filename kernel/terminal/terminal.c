#include <terminal/terminal.h>

Terminal create_terminal() {
    Terminal term;
    term.video = (volatile char*)0xB8000;
    term.cursor_x = 0;
    term.cursor_y = 0;

    return term;
}

void term_scroll(Terminal* term);

void term_putc(Terminal* term, char c) {
    if (c == '\n') {
        term->cursor_x = 0;

        if (term->cursor_y + 1 >= TERMINAL_MAX_HEIGHT) {
            term_scroll(term);
        } else {
            term->cursor_y++;
        }

        return;
    }

    int index = (term->cursor_y * TERMINAL_MAX_WIDTH + term->cursor_x) * 2;
    term->video[index] = c;
    term->video[index + 1] = 0x07;

    if (term->cursor_x + 1 >= TERMINAL_MAX_WIDTH) {
        term->cursor_x = 0;

        if (term->cursor_y + 1 >= TERMINAL_MAX_HEIGHT) {
            term_scroll(term);
        } else {
            term->cursor_y++;
        }
    } else {
        term->cursor_x++;
    }
}

void term_write(Terminal* term, const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        term_putc(term, str[i]);
        i++;
    }
}

void term_write_int(Terminal* term, int value) {
    if (value == 0) {
        term_putc(term, '0');
        return;
    }

    if (value < 0) {
        term_putc(term, '-');
        value *= -1;
    }

    char buffer[10];
    int pos = 0;

    while (value != 0) {
        int digit = value % 10;

        char digitChar = '0' + digit;
        buffer[pos] = digitChar;

        value = value / 10;
        pos++;
    }

    for (int i = pos - 1; i >= 0; i--) {
        term_putc(term, buffer[i]);
    }
}

void term_write_hex(Terminal* term, u64 value) {
    if (value == 0) {
        term_putc(term, '0');
        return;
    }

    const char* table = "0123456789ABCDEF";
    char buffer[16];
    int pos = 0;

    while (value != 0) {
        int digit = value & 0xF;

        char digitChar = table[digit];
        buffer[pos] = digitChar;

        value >>= 4;
        pos++;
    }
    
    for (int i = pos - 1; i >= 0; i--) {
        term_putc(term, buffer[i]);
    }
}

void term_scroll(Terminal *term) {
    int bytes_per_line = TERMINAL_MAX_WIDTH * 2;
    for (int l = 1; l < TERMINAL_MAX_HEIGHT; l++) {
        volatile char* origin = term->video + l * bytes_per_line;
        volatile char* dest = term->video + (l - 1) * bytes_per_line;

        for (int b = 0; b < bytes_per_line; b++) {
            dest[b] = origin[b];
        }
    }

    int last_line = (TERMINAL_MAX_HEIGHT - 1) * bytes_per_line;
    for (int b = 0; b < bytes_per_line; b++) {
        char c = ' ';
        if (b % 2) c = 0x07;

        term->video[last_line + b] = c;
    }
}
