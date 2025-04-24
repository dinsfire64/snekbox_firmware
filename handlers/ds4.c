#include "ds4.h"

#include <string.h>
#include "__handlers.h"

sony_ds4_report_t ds4_report = {0};

bool is_DS4(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return ((vid == 0x054c && (pid == 0x09cc || pid == 0x05c4)) // Sony DualShock4
            || (vid == 0x0f0d && pid == 0x005e)                 // Hori FC4
            || (vid == 0x0f0d && pid == 0x00ee)                 // Hori PS4 Mini (PS4-099U)
            || (vid == 0x1f4f && pid == 0x1002)                 // ASW GG xrd controller
    );
}

void setDS4Rumble(uint8_t dev_addr, uint8_t instance, uint8_t rumbleLeft, uint8_t rumbleRight)
{
    sony_ds4_output_report_t output_report = {0};
    output_report.set_rumble = 1;
    output_report.motor_left = rumbleLeft;
    output_report.motor_right = rumbleRight;
    tuh_hid_send_report(dev_addr, instance, 5, &output_report, sizeof(output_report));
}

// for some reason I had to add __not_in_flash_func to the ds4 processing...
void __not_in_flash_func(processDS4)(uint8_t const *report, uint16_t len)
{
    if (report[0] == 0x01 && len >= sizeof(sony_ds4_report_t))
    {
        memcpy(&ds4_report, report, sizeof(ds4_report));

        reset_report();

        encode_hat((hid_hat_t)ds4_report.dpad);

        input_report.short_report.btn_north = ds4_report.triangle;
        input_report.short_report.btn_south = ds4_report.cross;
        input_report.short_report.btn_east = ds4_report.circle;
        input_report.short_report.btn_west = ds4_report.square;

        input_report.short_report.start = ds4_report.option;
        input_report.short_report.select = ds4_report.share;

        input_report.short_report.l1 = ds4_report.l1;
        input_report.short_report.r1 = ds4_report.r1;
        input_report.short_report.l2 = ds4_report.l2;
        input_report.short_report.r2 = ds4_report.r2;
        input_report.short_report.l3 = ds4_report.l3;
        input_report.short_report.r3 = ds4_report.r3;

        input_report.short_report.guide = ds4_report.ps;

        input_report.short_report.axis_lx = ds4_report.lx;
        input_report.short_report.axis_ly = ds4_report.ly;

        input_report.short_report.axis_rx = ds4_report.rx;
        input_report.short_report.axis_ry = ds4_report.ry;

        // we do have analog triggers here.
        input_report.analog_l2 = ds4_report.l2_trigger;
        input_report.analog_r2 = ds4_report.r2_trigger;
    }
}
