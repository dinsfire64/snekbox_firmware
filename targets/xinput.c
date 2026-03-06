#include "xinput.h"
#include "xinput_descriptors.h"

#include "tusb.h"
#include "device/usbd_pvt.h"

#include "common_types.h"

XInputReport_t gamepad_state = {0};
uint8_t xinput_fromgame[32];

void xinput_make_report()
{
    gamepad_state.rid = 0;
    gamepad_state.rsize = sizeof(gamepad_state);

    gamepad_state.dpad_up = final_input_report.short_report.dpad_up;
    gamepad_state.dpad_down = final_input_report.short_report.dpad_down;
    gamepad_state.dpad_left = final_input_report.short_report.dpad_left;
    gamepad_state.dpad_right = final_input_report.short_report.dpad_right;
    gamepad_state.start = final_input_report.short_report.start;
    gamepad_state.back = final_input_report.short_report.select;
    gamepad_state.left_stick = final_input_report.short_report.l3;
    gamepad_state.right_stick = final_input_report.short_report.r3;

    gamepad_state.left_bumper = final_input_report.short_report.l1;
    gamepad_state.right_bumper = final_input_report.short_report.r1;
    gamepad_state.home = final_input_report.short_report.guide;
    // unused
    gamepad_state.a = final_input_report.short_report.btn_south;
    gamepad_state.b = final_input_report.short_report.btn_east;
    gamepad_state.x = final_input_report.short_report.btn_west;
    gamepad_state.y = final_input_report.short_report.btn_north;

    gamepad_state.lt = final_input_report.analog_l2;
    gamepad_state.rt = final_input_report.analog_r2;

    gamepad_state.l_x = ((final_input_report.short_report.axis_lx - 0x80) << 8);
    gamepad_state.l_y = -1 * ((final_input_report.short_report.axis_ly - 0x7F) << 8);

    gamepad_state.r_x = ((final_input_report.short_report.axis_rx - 0x80) << 8);
    gamepad_state.r_y = -1 * ((final_input_report.short_report.axis_ry - 0x7F) << 8);
}

void xinput_task()
{
    if (tud_ready())
    {
        if (!usbd_edpt_busy(0, XINPUT_EPADDR_IN))
        {
            xinput_make_report();
            usbd_edpt_xfer(0, XINPUT_EPADDR_IN, (void *)&gamepad_state, sizeof(gamepad_state));
        }

        if (!usbd_edpt_busy(0, XINPUT_EPADDR_OUT))
        {
            usbd_edpt_xfer(0, XINPUT_EPADDR_OUT, (void *)&xinput_fromgame, sizeof(xinput_fromgame));
        }
    }
}