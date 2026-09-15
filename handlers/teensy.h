#ifndef _TEENSY_JOYSTICK_H_
#define _TEENSY_JOYSTICK_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define TEENSY_VID 0x16C0
#define TEENSY_PID 0x0487

#pragma pack(push, 1)

typedef struct
{
    // No REPORT ID byte
    // Collection: CA:Joystick

    uint8_t BTN_JoystickButton00 : 1;
    uint8_t BTN_JoystickButton01 : 1;
    uint8_t BTN_JoystickButton02 : 1;
    uint8_t BTN_JoystickButton03 : 1;
    uint8_t BTN_JoystickButton04 : 1;
    uint8_t BTN_JoystickButton05 : 1;
    uint8_t BTN_JoystickButton06 : 1;
    uint8_t BTN_JoystickButton07 : 1;
    uint8_t BTN_JoystickButton08 : 1;
    uint8_t BTN_JoystickButton09 : 1;
    uint8_t BTN_JoystickButton10 : 1;
    uint8_t BTN_JoystickButton11 : 1;
    uint8_t BTN_JoystickButton12 : 1;
    uint8_t BTN_JoystickButton13 : 1;
    uint8_t BTN_JoystickButton14 : 1;
    uint8_t BTN_JoystickButton15 : 1;
    uint8_t BTN_JoystickButton16 : 1;
    uint8_t BTN_JoystickButton17 : 1;
    uint8_t BTN_JoystickButton18 : 1;
    uint8_t BTN_JoystickButton19 : 1;
    uint8_t BTN_JoystickButton20 : 1;
    uint8_t BTN_JoystickButton21 : 1;
    uint8_t BTN_JoystickButton22 : 1;
    uint8_t BTN_JoystickButton23 : 1;
    uint8_t BTN_JoystickButton24 : 1;
    uint8_t BTN_JoystickButton25 : 1;
    uint8_t BTN_JoystickButton26 : 1;
    uint8_t BTN_JoystickButton27 : 1;
    uint8_t BTN_JoystickButton28 : 1;
    uint8_t BTN_JoystickButton29 : 1;
    uint8_t BTN_JoystickButton30 : 1;
    uint8_t BTN_JoystickButton31 : 1;

    uint8_t GD_JoystickHatSwitch : 4; // Usage 0x00010039: Hat Switch, Value = 0 to 7, Physical = Value x 45 in
                                      // degrees Collection: CA:Joystick CP:Pointer

    uint16_t GD_JoystickPointerX
        : 10; // Usage 0x00010030: X, Value = 0 to 1023, Physical = Value x 105 / 341 in degrees
    uint16_t GD_JoystickPointerY
        : 10; // Usage 0x00010031: Y, Value = 0 to 1023, Physical = Value x 105 / 341 in degrees
    uint16_t GD_JoystickPointerZ
        : 10; // Usage 0x00010032: Z, Value = 0 to 1023, Physical = Value x 105 / 341 in degrees
    uint16_t GD_JoystickPointerRz : 10; // Usage 0x00010035: Rz, Value = 0 to 1023, Physical = Value x 105 / 341 in
                                        // degrees Collection: CA:Joystick

    uint16_t GD_JoystickSlider
        : 10; // Usage 0x00010036: Slider, Value = 0 to 1023, Physical = Value x 105 / 341 in degrees
    uint16_t GD_JoystickSlider_1
        : 10; // Usage 0x00010036: Slider, Value = 0 to 1023, Physical = Value x 105 / 341 in degrees
} TEENSY_report_t;

#pragma pack(pop)

_Static_assert(sizeof(TEENSY_report_t) == 12, "TEENSY_report_t size error");

bool is_TEENSY(uint8_t dev_addr);

void processTEENSY(uint8_t const *report, uint16_t len);

#endif
