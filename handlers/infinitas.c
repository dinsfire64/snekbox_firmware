#include "infinitas.h"

bool is_INFINITAS(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == INFINITAS_VID && pid == INFINITAS_PID);
}

void processINFINITAS(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(INFINITAS_report_t) && report[0] == INFINITAS_REPORT_ID)
    {
        INFINITAS_report_t new_INFINITAS_state = {0};
        memcpy(&new_INFINITAS_state, report, sizeof(new_INFINITAS_state));

        reset_report();

        input_report.short_report.controller_type = SPECIAL_CONTROLLER_IIDX;

        // store the analog turn table value here
        // ps2 code will turn it into up/down.
        input_report.short_report.axis_rx = new_INFINITAS_state.iidx_turntable;

        input_report.short_report.btn_west = new_INFINITAS_state.iidx_btn_01;
        input_report.short_report.l1 = new_INFINITAS_state.iidx_btn_02;
        input_report.short_report.btn_south = new_INFINITAS_state.iidx_btn_03;
        input_report.short_report.r1 = new_INFINITAS_state.iidx_btn_04;
        input_report.short_report.btn_east = new_INFINITAS_state.iidx_btn_05;
        input_report.short_report.l2 = new_INFINITAS_state.iidx_btn_06;
        input_report.short_report.dpad_left = new_INFINITAS_state.iidx_btn_07;

        input_report.short_report.start = new_INFINITAS_state.iidx_btn_start;
        input_report.short_report.select = new_INFINITAS_state.iidx_btn_select;

        // debugging/testing
        // input_report.short_report.l2 = new_INFINITAS_state.iidx_turntable_cw;
        // input_report.short_report.l1 = new_INFINITAS_state.iidx_turntable_ccw;
    }
}
