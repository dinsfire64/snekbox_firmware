#include "santroller.h"
#include "__handlers.h"

bool is_SANTROLLER(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == SANTROLLER_VID && pid == SANTROLLER_PID);
}

void processSANTROLLER(uint8_t const *report, uint16_t len)
{
    if (report[0] == SANTROLLER_REPORT_ID && len >= sizeof(SANTROLLER_report_t))
    {
        SANTROLLER_report_t new_SANTROLLER_state = {0};
        memcpy(&new_SANTROLLER_state, report, sizeof(new_SANTROLLER_state));

        reset_report();

        input_report.short_report.controller_type = SPECIAL_CONTROLLER_GUITAR;

        // whammy for some reason is zeros and has a dead zone then jumps...
        // but we want to idle at mid axis.
        if (new_SANTROLLER_state.whammy < 128)
        {
            new_SANTROLLER_state.whammy = 128;
        }

        // real gh guitar idles at 127 then goes down to zero.
        // santroller spikes to 127 then goes to 255.
        input_report.short_report.axis_ly = (255 - (new_SANTROLLER_state.whammy));

        // strum (down is down, up is up)
        encode_hat(new_SANTROLLER_state.GD_GamepadHatSwitch);

        // option buttons
        input_report.short_report.start = new_SANTROLLER_state.start;
        input_report.short_report.select = new_SANTROLLER_state.select;

        // green, red, yellow, blue, orange (ps2 mapping)
        // r2, circle, tri, cross, square
        input_report.short_report.r2 = new_SANTROLLER_state.green;
        input_report.short_report.btn_east = new_SANTROLLER_state.red;
        input_report.short_report.btn_north = new_SANTROLLER_state.yellow;
        input_report.short_report.btn_south = new_SANTROLLER_state.blue;
        input_report.short_report.btn_west = new_SANTROLLER_state.orange;

        // star, axis -> button press.
        input_report.short_report.l2 = (new_SANTROLLER_state.starpower_axis == 0xFF);
    }
}
