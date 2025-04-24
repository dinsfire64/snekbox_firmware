#include "switch_pro.h"

#include "tusb.h"

#include "debug.h"

bool is_switch_pro_init1 = false;
bool is_switch_pro_fully_init = false;

joycon_subcmd_request_t outgoing_request = {0};

static uint8_t default_rumble_data[] = {0x80, 0x00, 0x00, 0x00,
                                        0x80, 0x00, 0x00, 0x00};

bool is_SWITCH_PRO(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return vid == 0x057E && (pid == 0x2009);
}

void setSWITCH_PRORumble(uint8_t dev_addr, uint8_t instance, uint8_t rumbleLeft, uint8_t rumbleRight)
{
    if (!is_switch_pro_fully_init || (rumbleLeft == 0 && rumbleRight == 0))
    {
        return;
    }

    uint8_t report_size = JC_MIN_SUBCOMMAND_SIZE;
    outgoing_request.output_id = JC_OUTPUT_RUMBLE_ONLY;

    memcmp(&default_rumble_data, &outgoing_request.rumble_data, sizeof(outgoing_request.rumble_data));

    // taken from https://github.com/ToadKing/switch-pro-x/blob/master/switch-pro-x/ProControllerDevice.cpp#L568
    // strong motor
    if (rumbleLeft > 0)
    {
        outgoing_request.rumble_data[0] = 0x80;
        outgoing_request.rumble_data[1] = 0x20;
        outgoing_request.rumble_data[2] = 0x62;
        outgoing_request.rumble_data[3] = rumbleLeft >> 2;
    }

    // weak motor
    if (rumbleRight > 0)
    {
        outgoing_request.rumble_data[4] = 0x98;
        outgoing_request.rumble_data[5] = 0x20;
        outgoing_request.rumble_data[6] = 0x62;
        outgoing_request.rumble_data[7] = rumbleRight >> 2;
    }

    outgoing_request.packet_num = (outgoing_request.packet_num++) & 0x0F;
    tuh_hid_send_report(dev_addr, instance, 0, &outgoing_request, report_size);
}

void init_switch_pro(uint8_t dev_addr, uint8_t instance)
{
    if (is_switch_pro_fully_init)
    {
        return;
    }

    // see https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/USB-HID-Notes.md
    if (!is_switch_pro_init1 && !is_switch_pro_fully_init)
    {
        // this enables the USB commands
        uint8_t buff[2] = {0x80, 0x02};
        tuh_hid_send_report(dev_addr, instance, 0, buff, sizeof(buff));

        is_switch_pro_init1 = true;
    }
    else if (is_switch_pro_init1 && !is_switch_pro_fully_init)
    {
        // this disables the timeout over USB.
        uint8_t buff2[2] = {0x80, 0x04};
        tuh_hid_send_report(dev_addr, instance, 0, buff2, sizeof(buff2));

        is_switch_pro_fully_init = true;
    }
}

uint8_t switch_pro_axis_fix(uint16_t input)
{
    // TODO: Grab config/cal data from the controller to get the proper scale factor.

    // convert to signed uint16 values and scale the axis into a uint8 barrier
    int16_t rtn_val = (input - 0x07FF) / 11;

    // use decimal for easy clamping.
    if (rtn_val >= 128)
    {
        rtn_val = 128;
    }
    else if (rtn_val <= -127)
    {
        rtn_val = -127;
    }

    return (uint8_t)(rtn_val + 127);
}

void processSWITCH_PRO(uint8_t const *report, uint16_t len)
{
    if (report[0] == 0x30 && len >= sizeof(switch_pro_report_t))
    {
        switch_pro_report_t pro_report;
        memcpy(&pro_report, report, sizeof(pro_report));
        
        reset_report();

        input_report.short_report.dpad_up = pro_report.up;
        input_report.short_report.dpad_down = pro_report.down;
        input_report.short_report.dpad_left = pro_report.left;
        input_report.short_report.dpad_right = pro_report.right;

        input_report.short_report.btn_north = pro_report.x;
        input_report.short_report.btn_south = pro_report.b;
        input_report.short_report.btn_east = pro_report.a;
        input_report.short_report.btn_west = pro_report.y;

        input_report.short_report.start = pro_report.plus;
        input_report.short_report.select = pro_report.minus;

        input_report.short_report.l1 = pro_report.l;
        input_report.short_report.r1 = pro_report.r;
        input_report.short_report.l2 = pro_report.zl;
        input_report.short_report.r2 = pro_report.zr;
        input_report.short_report.l3 = pro_report.l3;
        input_report.short_report.r3 = pro_report.r3;

        input_report.short_report.guide = pro_report.home;

        input_report.short_report.axis_lx = switch_pro_axis_fix(pro_report.lx);
        input_report.short_report.axis_ly = ~switch_pro_axis_fix(pro_report.ly);

        input_report.short_report.axis_rx = switch_pro_axis_fix(pro_report.rx);
        input_report.short_report.axis_ry = ~switch_pro_axis_fix(pro_report.ry);

        // no analog triggers on this controller...
        input_report.analog_l2 = input_report.short_report.l2 ? 0xFF : 0x00;
        input_report.analog_r2 = input_report.short_report.r2 ? 0xFF : 0x00;
    }
}
