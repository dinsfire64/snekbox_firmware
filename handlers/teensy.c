#include "teensy.h"

bool is_TEENSY(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == TEENSY_VID && pid == TEENSY_PID);
}

void processTEENSY(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(TEENSY_report_t))
    {
        TEENSY_report_t new_TEENSY_state = {0};
        memcpy(&new_TEENSY_state, report, sizeof(new_TEENSY_state));

        reset_report();

        /*
            The up arrow is B3
            Down arrow is B5
            Right arrow B7
            Left arrow B1

            Center panel is b4

            Lower left is B2 -> triangle -> north
            Upper left is b0 -> cross -> south
            Upper right is B6 -> circle -> east
            Lower right is b8 -> square -> west
        */

        input_report.short_report.dpad_up = new_TEENSY_state.BTN_JoystickButton03;
        input_report.short_report.dpad_down = new_TEENSY_state.BTN_JoystickButton05;
        input_report.short_report.dpad_right = new_TEENSY_state.BTN_JoystickButton07;
        input_report.short_report.dpad_left = new_TEENSY_state.BTN_JoystickButton01;

        input_report.short_report.btn_north = new_TEENSY_state.BTN_JoystickButton02;
        input_report.short_report.btn_south = new_TEENSY_state.BTN_JoystickButton00;
        input_report.short_report.btn_east = new_TEENSY_state.BTN_JoystickButton06;
        input_report.short_report.btn_west = new_TEENSY_state.BTN_JoystickButton08;
    }
}
