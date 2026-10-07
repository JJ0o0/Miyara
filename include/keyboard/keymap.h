#ifndef KEYMAP_H
#define KEYMAP_H

#include <keyboard/keyboard.h>
#include <types/types.h>

bool keymap_translate(KeyEvent event, char* character);

#endif