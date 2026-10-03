#ifndef PS2_H
#define PS2_H

#include <types/types.h>

bool ps2_wait_input(void);
bool ps2_wait_output(void);

bool ps2_write_command(u8 command);

bool ps2_write_data(u8 data);
bool ps2_read_data(u8* data);

bool ps2_read_config(u8* config);
bool ps2_write_config(u8 config);

bool ps2_enable_second_port(void);
bool ps2_write_second_port(u8 data);

#endif