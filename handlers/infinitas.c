#include "infinitas.h"

char phoenixwan_prod_string[] = "Controller INF&BMS";

bool isPhoenixWan = false;

bool is_INFINITAS(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    bool isInf = (vid == INFINITAS_VID && pid == INFINITAS_PID);

    if (isInf)
    {
        // since a lot of controllers use the konami vid/pid for this controller
        // use string comparison to find specific ones.
        isPhoenixWan = (memcmp(current_device.product_str, phoenixwan_prod_string, sizeof(phoenixwan_prod_string)) == 0);
    }

    return isInf;
}

void processINFINITAS(uint8_t const *report, uint16_t len)
{
    if (isPhoenixWan && len == sizeof(PHOENIXWAN_report_t))
    {
        // no report id.
        PHOENIXWAN_report_t new_PHOENIXWAN_state = {0};
        memcpy(&new_PHOENIXWAN_state, report, sizeof(new_PHOENIXWAN_state));

        reset_report();

        input_report.short_report.controller_type = SPECIAL_CONTROLLER_IIDX;

        // store the analog turn table value here
        // ps2 code will turn it into up/down.
        input_report.short_report.axis_rx = new_PHOENIXWAN_state.turntable;

        input_report.short_report.btn_west = new_PHOENIXWAN_state.B1;
        input_report.short_report.l1 = new_PHOENIXWAN_state.B2;
        input_report.short_report.btn_south = new_PHOENIXWAN_state.B3;
        input_report.short_report.r1 = new_PHOENIXWAN_state.B4;
        input_report.short_report.btn_east = new_PHOENIXWAN_state.B5;
        input_report.short_report.l2 = new_PHOENIXWAN_state.B6;
        input_report.short_report.dpad_left = new_PHOENIXWAN_state.B7;

        input_report.short_report.start = new_PHOENIXWAN_state.E1;
        input_report.short_report.select = new_PHOENIXWAN_state.E2;
    }
    else if (len == sizeof(INFINITAS_report_t) && report[0] == INFINITAS_REPORT_ID)
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
