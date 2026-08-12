#ifndef _B2L_H_
#define _B2L_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define B2L_VID 0x1973
#define B2L_PID 0x1021

#define B2LV2_VID 0x0F0D
#define B2LV2_PID 0x0094

// this is USB test vid/pid...you can't ship that
#define B2LV3_VID 0xCAFE
#define B2LV3_PID 0x4004

#define B2L_REPORT_ID 0x01

#pragma pack(push, 1)

typedef struct
{
    // Report ID = 0x01 (1)
    uint8_t reportId : 8;

    uint8_t p1_up : 1;
    uint8_t p1_down : 1;
    uint8_t p1_left : 1;
    uint8_t p1_right : 1;
    uint8_t p1_pad : 4; // always zero

    uint8_t p2_up : 1;
    uint8_t p2_down : 1;
    uint8_t p2_left : 1;
    uint8_t p2_right : 1;
    uint8_t p2_pad : 4; // always zero

} B2L_report_t;

typedef struct
{
    // No REPORT ID byte
    // Collection: CA:Gamepad

    uint8_t p1_down : 1;
    uint8_t p1_right : 1;
    uint8_t p1_left : 1;
    uint8_t p1_up : 1;
    uint8_t BTN_GamepadButton05 : 1;
    uint8_t BTN_GamepadButton06 : 1;
    uint8_t BTN_GamepadButton07 : 1;
    uint8_t BTN_GamepadButton8 : 1;

    uint8_t BTN_GamepadButton09 : 1;
    uint8_t BTN_GamepadButton10 : 1;
    uint8_t BTN_GamepadButton11 : 1;
    uint8_t BTN_GamepadButton12 : 1;
    uint8_t BTN_GamepadButton13 : 1;
    uint8_t BTN_GamepadButton14 : 1;
    uint8_t BTN_GamepadButton15 : 1;
    uint8_t p2_down : 1;

    uint8_t p2_right : 1;
    uint8_t p2_up : 1;
    uint8_t p2_left : 1;
    uint8_t BTN_GamepadButton20 : 1;
    uint8_t BTN_GamepadButton21 : 1;
    uint8_t BTN_GamepadButton22 : 1;
    uint8_t BTN_GamepadButton23 : 1;
    uint8_t BTN_GamepadButton24 : 1;

    uint8_t GD_GamepadHatSwitch : 4;
    uint8_t _reserved : 4;
} B2LV2_report_t;

typedef struct
{
    // Report ID = 0x01 (1)
    uint8_t reportId;

    // Byte 1
    uint8_t up : 1;      // Bit 0
    uint8_t left : 1;    // Bit 1
    uint8_t button3 : 1; // Bit 2
    uint8_t button4 : 1; // Bit 3

    uint8_t button5 : 1; // Bit 4
    uint8_t right : 1;   // Bit 5
    uint8_t down : 1;    // Bit 6
    uint8_t button8 : 1; // Bit 7

    // Byte 2
    uint8_t button9 : 1;  // Bit 8
    uint8_t button10 : 1; // Bit 9
    uint8_t button11 : 1; // Bit 10
    uint8_t button12 : 1; // Bit 11

    uint8_t button13 : 1; // Bit 12
    uint8_t button14 : 1; // Bit 13
    uint8_t padding : 2;  // Bits 14–15 (constant)

    // Bytes 3–4
    uint8_t value1; // Bits 16–23
    uint8_t value2; // Bits 24–31

} B2LV3_report_t;

#pragma pack(pop)

bool is_B2L(uint8_t dev_addr);
bool is_B2LV2(uint8_t dev_addr);
bool is_B2LV3(uint8_t dev_addr);

void processB2L(uint8_t const *report, uint16_t len);
void processB2LV2(uint8_t const *report, uint16_t len);
void processB2LV3(uint8_t const *report, uint16_t len);

#endif
