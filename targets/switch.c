#include "switch.h"
#include "input_report.h"
#include "handlers/__handlers.h"

switch_report_t switchReport;
switch_report_t last_switchReport;

uint64_t prev_sent_us = 0;

void switch_setup(void)
{
    memset(&switchReport, 0x00, sizeof(switchReport));
    memset(&last_switchReport, 0x00, sizeof(last_switchReport));

    switch_makeReport();
}

void switch_task(void)
{
    if (tud_suspended())
    {
        tud_remote_wakeup();
    }

    if (tud_hid_n_ready(HID_GAMEPAD_ITF_NUM))
    {
        switch_makeReport();

        // always send to switch
        if (tud_hid_n_report(HID_GAMEPAD_ITF_NUM, 0, &switchReport, sizeof(switchReport)))
        {
            // TODO: figure out why sending on change/idle causes glitches?
            memcpy(&last_switchReport, &switchReport, sizeof(switchReport));
            prev_sent_us = time_us_64();
        }
        else
        {
            DebugPrintf("Fail send");
        }
    }
}

void switch_makeReport(void)
{
    switchReport.Y = final_input_report.short_report.btn_west;
    switchReport.A = final_input_report.short_report.btn_east;
    switchReport.B = final_input_report.short_report.btn_south;
    switchReport.X = final_input_report.short_report.btn_north;

    switchReport.L = final_input_report.short_report.l1;
    switchReport.R = final_input_report.short_report.r1;
    switchReport.ZL = final_input_report.short_report.l2;
    switchReport.ZR = final_input_report.short_report.r2;

    switchReport.Minus = final_input_report.short_report.select;
    switchReport.Plus = final_input_report.short_report.start;
    switchReport.L3 = final_input_report.short_report.l3;
    switchReport.R3 = final_input_report.short_report.r3;
    switchReport.Home = final_input_report.short_report.guide;
    switchReport.Capture = 0;

    switchReport.leftXAxis = final_input_report.short_report.axis_lx;
    switchReport.leftYAxis = final_input_report.short_report.axis_ly;
    switchReport.rightXAxis = final_input_report.short_report.axis_rx;
    switchReport.rightYAxis = final_input_report.short_report.axis_ry;

    switchReport.dPad = (uint8_t)local_to_hat();

    switchReport.filler = 0;
}
