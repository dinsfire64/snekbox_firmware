#ifndef _PS3_H_
#define _PS3_H_

#include "ps3_descriptors.h"

#define PS3_REPORT_SIZE 19

#define PS3_ENABLEJUMPFIX false

enum PS3DpadDirs
{
    PS3_DPAD_UP = 0x00,
    PS3_DPAD_UP_RIGHT = 0x01,
    PS3_DPAD_RIGHT = 0x02,
    PS3_DPAD_DOWN_RIGHT = 0x03,
    PS3_DPAD_DOWN = 0x04,
    PS3_DPAD_DOWN_LEFT = 0x05,
    PS3_DPAD_LEFT = 0x06,
    PS3_DPAD_UP_LEFT = 0x07,
    PS3_DPAD_NONE = 0x0f,
};

#pragma pack(push, 1)

typedef struct
{
    // byte 0
    uint8_t square : 1;
    uint8_t cross : 1;
    uint8_t circle : 1;
    uint8_t triangle : 1;
    uint8_t l1 : 1;
    uint8_t r1 : 1;
    uint8_t l2 : 1;
    uint8_t r2 : 1;

    // byte 1
    uint8_t select : 1;
    uint8_t start : 1;
    uint8_t l3 : 1;
    uint8_t r3 : 1;
    uint8_t ps_button : 1;
    uint8_t paddd : 3;

    uint8_t dpad_hat;

    uint8_t lx;
    uint8_t ly;
    uint8_t rx;
    uint8_t ry;

    // confirmed indicies with ps2 hen and ps2 pad tester for analog values.
    uint8_t analog_dpad_right;
    uint8_t analog_dpad_left;
    uint8_t analog_dpad_up;
    uint8_t analog_dpad_down;

    uint8_t analog_triangle;
    uint8_t analog_circle;
    uint8_t analog_cross;
    uint8_t analog_square;

    uint8_t analog_l1;
    uint8_t analog_r1;
    uint8_t analog_l2;
    uint8_t analog_r2;

} ps3_report_t;

#pragma pack(pop)

_Static_assert(sizeof(ps3_report_t) == PS3_REPORT_SIZE, "ps3_report_t size error");

void ps3_setup(void);
void ps3_task(void);

// void ps3_process_output(uint8_t report_size);

void ps3_makeReport(void);

#endif