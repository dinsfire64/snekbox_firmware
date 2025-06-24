#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "tusb.h"
#include "debug.h"
#include "common_types.h"

#define MAX_KEYBOARD_REPORT_SIZE 64

typedef struct
{
    uint8_t keycode;
    bool *button;
} keyboard_mapping_t;

typedef struct
{
    bool dpad_up;
    bool dpad_down;
    bool dpad_left;
    bool dpad_right;

    bool btn_south;
    bool btn_east;
    bool btn_west;
    bool btn_north;

    bool guide;

    bool start;
    bool select;
} keyboard_input_report_t;

void set_key_mapping(uint8_t dev_addr);
void process_kbd_report(uint8_t dev_addr, hid_keyboard_report_t const *report, uint16_t len);

#endif