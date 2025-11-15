#ifndef _DDR_GRANDPRIX_H_
#define _DDR_GRANDPRIX_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define DDR_GRANDPRIX_VID 0x1CCF
#define DDR_GRANDPRIX_PID 0x1020

#define DDR_GRANDPRIX_REPORT_ID 0x00

#pragma pack(push, 1)

typedef struct
{
    int8_t x; // X axis (-127 to 127)
    int8_t y; // Y axis (-127 to 127)

    // Buttons as bitfields
    uint16_t select : 1;     // Button 1 (L button)
    uint16_t start : 1;      // Button 2 (R button)
    uint16_t dpad_up : 1;    // Button 3 (Up)
    uint16_t dpad_right : 1; // Button 4 (Right)
    uint16_t dpad_down : 1;  // Button 5 (Down)
    uint16_t dpad_left : 1;  // Button 6 (Left)
    uint16_t _reserved1 : 2; // padding

    uint16_t btn_circle : 1; // Button 9 (Circle)
    uint16_t btn_cross : 1;  // Button 10 (Cross)
    uint16_t _reserved2 : 6; // padding to

    uint8_t padding; // extra padding
} ddr_grandprix_report_t;

#pragma pack(pop)

bool is_DDR_GRANDPRIX(uint8_t dev_addr);

void processDDR_GRANDPRIX(uint8_t const *report, uint16_t len);

#endif