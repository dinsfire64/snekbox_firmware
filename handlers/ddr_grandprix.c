#include "ddr_grandprix.h"
#include <string.h>
#include "input_report.h"

bool is_DDR_GRANDPRIX(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == DDR_GRANDPRIX_VID && pid == DDR_GRANDPRIX_PID);
}

void processDDR_GRANDPRIX(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(ddr_grandprix_report_t))
    {
        ddr_grandprix_report_t new_ddr_state = {0};
        memcpy(&new_ddr_state, report, sizeof(new_ddr_state));

        reset_report();

        input_report.short_report.select = new_ddr_state.select;
        input_report.short_report.start = new_ddr_state.start;

        input_report.short_report.dpad_up = new_ddr_state.dpad_up;
        input_report.short_report.dpad_right = new_ddr_state.dpad_right;
        input_report.short_report.dpad_down = new_ddr_state.dpad_down;
        input_report.short_report.dpad_left = new_ddr_state.dpad_left;

        input_report.short_report.btn_east = new_ddr_state.btn_circle;
        input_report.short_report.btn_south = new_ddr_state.btn_cross;
    }
}