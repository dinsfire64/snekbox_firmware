#ifndef _PS2_CONTROLLER_
#define _PS2_CONTROLLER_

#include "debug.h"

#include "common_types.h"

#pragma pack(push, 1)

typedef union
{
    struct
    {
        bool select : 1;
        bool l3 : 1;
        bool r3 : 1;
        bool start : 1;
        bool up : 1;
        bool right : 1;
        bool down : 1;
        bool left : 1;
    } val;
    uint8_t raw;
} ps2_buttons1_t;

typedef union
{
    struct
    {
        bool l2 : 1;
        bool r2 : 1;
        bool l1 : 1;
        bool r1 : 1;
        bool tri : 1;
        bool cir : 1;
        bool cross : 1;
        bool squ : 1;
    } val;
    uint8_t raw;
} ps2_buttons2_t;

typedef struct
{
    ps2_buttons1_t buttons1;
    ps2_buttons2_t buttons2;
} ps2_button_state_t;

#pragma pack(pop)

#define MODE_DIGITAL 0x41
#define MODE_ANALOG 0x73
#define MODE_ANALOG_PRESSURE 0x79
#define MODE_CONFIG 0xF3

#define CMD_PRES_CONFIG 0x40
#define CMD_POLL_CONFIG_STATUS 0x41
#define CMD_POLL 0x42
#define CMD_CONFIG 0x43
#define CMD_STATUS 0x45
#define CMD_CONST_46 0x46
#define CMD_CONST_47 0x47
#define CMD_CONST_4C 0x4C
#define CMD_ENABLE_RUMBLE 0x4D
#define CMD_POLL_CONFIG 0x4F
#define CMD_ANALOG_SWITCH 0x44

#define PS2_DEFAULT_STICK_POS 0
#define MODE_PS2_STARTUP MODE_DIGITAL

// how many polling cycles to hold select for an effector knob change.
#define GF_EFFECTOR_HOLD_COUNT 10

void initPS2ControllerState();

void processPoll();
void processConfig();
void processStatus();
void processConst46();
void processConst47();
void processConst4c();
void processPollConfigStatus();
void processEnableRumble();
void processPollConfig();
void processPresConfig();
void processAnalogSwitch();
void process_joy_req();

void force_ps2_to_analog();

extern ps2_button_state_t inputState;

#endif