#include "dual_ps2.h"
#include "__handlers.h"

bool is_DUAL_PS2(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == DUAL_PS2_VID && pid == DUAL_PS2_PID);
}

void processDUAL_PS2(uint8_t const *report, uint16_t len)
{
    if (report[0] == DUAL_PS2_REPORT_ID && len >= sizeof(dual_ps2_report_t))
    {
        dual_ps2_report_t new_dual_ps2_state = {0};
        memcpy(&new_dual_ps2_state, report, sizeof(new_dual_ps2_state));
        
        reset_report();

        // DebugOutputBuffer("DUAL", report, len);

        hat_to_local(new_dual_ps2_state.dpad_hat);

        input_report.short_report.axis_lx = new_dual_ps2_state.lx;
        input_report.short_report.axis_ly = new_dual_ps2_state.ly;

        input_report.short_report.axis_rx = new_dual_ps2_state.rx;
        input_report.short_report.axis_ry = new_dual_ps2_state.ry;

        input_report.short_report.btn_north = new_dual_ps2_state.triangle;
        input_report.short_report.btn_south = new_dual_ps2_state.cross;
        input_report.short_report.btn_east = new_dual_ps2_state.circle;
        input_report.short_report.btn_west = new_dual_ps2_state.square;

        input_report.short_report.start = new_dual_ps2_state.start;
        input_report.short_report.select = new_dual_ps2_state.select;

        input_report.short_report.l1 = new_dual_ps2_state.l1;
        input_report.short_report.r1 = new_dual_ps2_state.r1;
        input_report.short_report.l2 = new_dual_ps2_state.l2;
        input_report.short_report.r2 = new_dual_ps2_state.r2;
        input_report.short_report.l3 = new_dual_ps2_state.l3;
        input_report.short_report.r3 = new_dual_ps2_state.r3;

        // no analog triggers on this controller...
        input_report.analog_l2 = input_report.short_report.l2 ? 0xFF : 0x00;
        input_report.analog_r2 = input_report.short_report.r2 ? 0xFF : 0x00;
    }
}
