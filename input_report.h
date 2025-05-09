#ifndef _INPUT_REPORT_H_
#define _INPUT_REPORT_H_

#include "common_types.h"
#include "debug.h"

#pragma pack(push, 1)

typedef enum
{
    SPECIAL_CONTROLLER_NONE = 0x00,

    SPECIAL_CONTROLLER_GUITAR,
    SPECIAL_CONTROLLER_POPN,
    SPECIAL_CONTROLLER_GF_GUITAR,
    SPECIAL_CONTROLLER_DM_DRUMS,
    SPECIAL_CONTROLLER_TATACON,

    NUM_OF_SPECIAL_CONTROLLERS
} special_controller_t;

typedef struct
{
    struct
    {
        bool dpad_up : 1;
        bool dpad_down : 1;
        bool dpad_left : 1;
        bool dpad_right : 1;
        bool btn_north : 1;
        bool btn_south : 1;
        bool btn_east : 1;
        bool btn_west : 1;

        bool start : 1;
        bool select : 1;
        bool l1 : 1;
        bool r1 : 1;
        bool l2 : 1;
        bool r2 : 1;
        bool l3 : 1;
        bool r3 : 1;

        bool guide : 1;
        uint8_t gf_effector : 2;
        special_controller_t controller_type : 5;
    };

    uint8_t axis_lx;
    uint8_t axis_ly;

    uint8_t axis_rx;
    uint8_t axis_ry;

} input_report_short_t;

typedef struct
{
    input_report_short_t short_report;

    bool enable_analog_facebuttons;

    uint8_t analog_dpad_up;
    uint8_t analog_dpad_down;
    uint8_t analog_dpad_left;
    uint8_t analog_dpad_right;

    uint8_t analog_btn_north;
    uint8_t analog_btn_south;
    uint8_t analog_btn_east;
    uint8_t analog_btn_west;

    uint8_t analog_l1;
    uint8_t analog_r1;
    uint8_t analog_l2;
    uint8_t analog_r2;

} input_report_long_t;

#pragma pack(pop)

extern input_report_long_t input_report;
extern input_report_long_t final_input_report;

extern bool en_helper_report;
extern input_report_short_t helper_short_report;

void reset_report();

void mux_report(input_report_short_t short_rpt);

#endif