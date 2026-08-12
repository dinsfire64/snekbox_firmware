#ifndef _HORI_H_
#define _HORI_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define HORI_VID_POKKENWIIU 0x0F0D
#define HORI_PID_POKKENWIIU 0x0092

#pragma pack(push, 1)

typedef struct
{
    // No REPORT ID byte
    // Collection: CA:Gamepad
    uint8_t Y : 1;  // Usage 0x00090001: Button 1 Primary/trigger, Value = 0 to 1, Physical = Value
    uint8_t B : 1;  // Usage 0x00090002: Button 2 Secondary, Value = 0 to 1, Physical = Value
    uint8_t A : 1;  // Usage 0x00090003: Button 3 Tertiary, Value = 0 to 1, Physical = Value
    uint8_t X : 1;  // Usage 0x00090004: Button 4, Value = 0 to 1, Physical = Value
    uint8_t L : 1;  // Usage 0x00090005: Button 5, Value = 0 to 1, Physical = Value
    uint8_t R : 1;  // Usage 0x00090006: Button 6, Value = 0 to 1, Physical = Value
    uint8_t ZL : 1; // Usage 0x00090007: Button 7, Value = 0 to 1, Physical = Value

    uint8_t ZR : 1;                  // Usage 0x00090008: Button 8, Value = 0 to 1, Physical = Value
    uint8_t select : 1;              // Usage 0x00090009: Button 9, Value = 0 to 1, Physical = Value
    uint8_t start : 1;               // Usage 0x0009000A: Button 10, Value = 0 to 1, Physical = Value
    uint8_t BTN_GamepadButton11 : 1; // Usage 0x0009000B: Button 11, Value = 0 to 1, Physical = Value
    uint8_t BTN_GamepadButton12 : 1; // Usage 0x0009000C: Button 12, Value = 0 to 1, Physical = Value
    uint8_t BTN_GamepadButton13 : 1; // Usage 0x0009000D: Button 13, Value = 0 to 1, Physical = Value
    uint8_t : 1;                     // Pad
    uint8_t : 1;                     // Pad

    uint8_t : 1;                     // Pad
    uint8_t GD_GamepadHatSwitch : 4; // Usage 0x00010039: Hat Switch, Value = 0 to 7, Physical = Value x 45 in degrees
    uint8_t : 1;                     // Pad
    uint8_t : 1;                     // Pad
    uint8_t : 1;                     // Pad
    uint8_t : 1;                     // Pad

    uint8_t GD_GamepadX;     // Usage 0x00010030: X, Value = 0 to -1, Physical = Value x --1
    uint8_t GD_GamepadY;     // Usage 0x00010031: Y, Value = 0 to -1, Physical = Value x --1
    uint8_t GD_GamepadZ;     // Usage 0x00010032: Z, Value = 0 to -1, Physical = Value x --1
    uint8_t GD_GamepadRz;    // Usage 0x00010035: Rz, Value = 0 to -1, Physical = Value x --1
    uint8_t VEN_Gamepad0020; // Usage 0xFF000020: , Value = 0 to -1, Physical = Value x --1

} HORI_POKKENWIIU_report_t;

#pragma pack(pop)

bool is_HORI_POKKENWIIU(uint8_t dev_addr);

void processHORI_POKKENWIIU(uint8_t const *report, uint16_t len);

#endif
