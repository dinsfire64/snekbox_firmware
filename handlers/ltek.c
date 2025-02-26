#include "ltek.h"

bool is_LTEK(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == LTEK_VID1 && pid == LTEK_PID1) ||
           (vid == LTEK_VID2 && pid == LTEK_PID2);
}

void processLTEK(uint8_t const *report, uint16_t len)
{
    if (report[0] == LTEK_REPORT_ID && len >= sizeof(ltek_report_t))
    {
        ltek_report_t new_LTEK_state = {0};
        memcpy(&new_LTEK_state, report, sizeof(new_LTEK_state));
        
        reset_report();

        input_report.short_report.dpad_up = new_LTEK_state.up;
        input_report.short_report.dpad_down = new_LTEK_state.down;
        input_report.short_report.dpad_left = new_LTEK_state.left;
        input_report.short_report.dpad_right = new_LTEK_state.right;
    }
}
