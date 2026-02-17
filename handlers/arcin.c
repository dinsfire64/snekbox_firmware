#include "arcin.h"

char arcin_prod_string[] = "arcin";

bool is_ARCIN(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    if ((vid == ARCIN_VID && pid == ARCIN_PID))
    {
        if (memcmp(current_device.product_str, arcin_prod_string, sizeof(arcin_prod_string)) == 0)
        {
            return true;
        }
        else
        {
            DebugPrintf("String cmp error on arcin");
        }
    }

    return false;
}

void processARCIN(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(ARCIN_report_t) && report[0] == ARCIN_REPORT_ID)
    {
        ARCIN_report_t new_ARCIN_state = {0};
        memcpy(&new_ARCIN_state, report, sizeof(new_ARCIN_state));

        reset_report();

        input_report.short_report.controller_type = SPECIAL_CONTROLLER_IIDX;

        // store the analog turn table value here
        // ps2 code will turn it into up/down.
        input_report.short_report.axis_rx = new_ARCIN_state.iidx_turntable;

        input_report.short_report.btn_west = new_ARCIN_state.iidx_btn_01;
        input_report.short_report.l1 = new_ARCIN_state.iidx_btn_02;
        input_report.short_report.btn_south = new_ARCIN_state.iidx_btn_03;
        input_report.short_report.r1 = new_ARCIN_state.iidx_btn_04;
        input_report.short_report.btn_east = new_ARCIN_state.iidx_btn_05;
        input_report.short_report.l2 = new_ARCIN_state.iidx_btn_06;
        input_report.short_report.dpad_left = new_ARCIN_state.iidx_btn_07;

        input_report.short_report.start = new_ARCIN_state.iidx_btn_start;
        input_report.short_report.select = new_ARCIN_state.iidx_btn_select;
    }
}
