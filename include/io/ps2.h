#ifndef PS2_H
#define PS2_H

#include <types/types.h>

bool ps2_wait_input(void);
bool ps2_wait_output(void);

void ps2_write_command(u8 command);

void ps2_write_data(u8 data);
bool ps2_read_data(u8* data);

#endif