#include "ds5.h"
#include "__handlers.h"

bool is_DS5(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == DS5_VID && pid == DS5_PID);
}

void processDS5(uint8_t const *report, uint16_t len)
{
    if (report[0] == DS5_REPORT_ID && len >= sizeof(DS5_report_t))
    {
        DS5_report_t ds5_report = {0};
        memcpy(&ds5_report, report, sizeof(DS5_report_t));

        reset_report();

        encode_hat((hid_hat_t)ds5_report.dpad);

        input_report.short_report.btn_north = ds5_report.triangle;
        input_report.short_report.btn_south = ds5_report.cross;
        input_report.short_report.btn_east = ds5_report.circle;
        input_report.short_report.btn_west = ds5_report.square;

        input_report.short_report.start = ds5_report.options;
        input_report.short_report.select = ds5_report.create;

        input_report.short_report.l1 = ds5_report.l1;
        input_report.short_report.r1 = ds5_report.r1;
        input_report.short_report.l2 = ds5_report.l2;
        input_report.short_report.r2 = ds5_report.r2;
        input_report.short_report.l3 = ds5_report.l3;
        input_report.short_report.r3 = ds5_report.r3;

        input_report.short_report.guide = ds5_report.ps;

        input_report.short_report.axis_lx = ds5_report.lx;
        input_report.short_report.axis_ly = ds5_report.ly;

        input_report.short_report.axis_rx = ds5_report.rx;
        input_report.short_report.axis_ry = ds5_report.ry;

        // we do have analog triggers here.
        input_report.analog_l2 = ds5_report.l2_trigger;
        input_report.analog_r2 = ds5_report.r2_trigger;
    }
}
