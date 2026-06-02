#include "ps3.h"

#include "device/usbd_pvt.h"

#include "handlers/ds3.h"

ps3_report_t ps3_state;

void ps3_setup(void)
{
    memset(&ps3_state, 0x00, sizeof(ps3_state));
}

uint8_t encodeHat()
{
    uint8_t hatVal = PS3_DPAD_NONE;

    if (final_input_report.short_report.dpad_up)
    {
        hatVal = PS3_DPAD_UP;

        if (final_input_report.short_report.dpad_right)
        {
            hatVal = PS3_DPAD_UP_RIGHT;
        }
        else if (final_input_report.short_report.dpad_left)
        {
            hatVal = PS3_DPAD_UP_LEFT;
        }
        else if (final_input_report.short_report.dpad_down)
        {
#if PS3_ENABLEJUMPFIX
            // press X to correspond to an UP DOWN jump.
            ps3_state.cross = true;
#endif
        }
    }
    else
    {
        if (final_input_report.short_report.dpad_down)
        {
            hatVal = PS3_DPAD_DOWN;

            if (final_input_report.short_report.dpad_right)
            {
                hatVal = PS3_DPAD_DOWN_RIGHT;
            }
            else if (final_input_report.short_report.dpad_left)
            {
                hatVal = PS3_DPAD_DOWN_LEFT;
            }
        }
        else
        {
            if (final_input_report.short_report.dpad_right && final_input_report.short_report.dpad_left)
            {
                hatVal = PS3_DPAD_LEFT;

#if PS3_ENABLEJUMPFIX
                // press O to correspond to an LEFT RIGHT jump.
                ps3_state.circle = true;
#endif
            }
            else if (final_input_report.short_report.dpad_right)
            {
                hatVal = PS3_DPAD_RIGHT;
            }
            else if (final_input_report.short_report.dpad_left)
            {
                hatVal = PS3_DPAD_LEFT;
            }
        }
    }

    return hatVal;
}

void ps3_makeReport()
{
    ps3_state.square = final_input_report.short_report.btn_west;
    ps3_state.cross = final_input_report.short_report.btn_south;
    ps3_state.circle = final_input_report.short_report.btn_east;
    ps3_state.triangle = final_input_report.short_report.btn_north;

    ps3_state.l1 = final_input_report.short_report.l1;
    ps3_state.l2 = final_input_report.short_report.l2;
    ps3_state.r1 = final_input_report.short_report.r1;
    ps3_state.r2 = final_input_report.short_report.r2;

    ps3_state.select = final_input_report.short_report.select;
    ps3_state.start = final_input_report.short_report.start;

    ps3_state.l3 = final_input_report.short_report.l3;
    ps3_state.r3 = final_input_report.short_report.r3;

    ps3_state.ps_button = final_input_report.short_report.guide;

    ps3_state.dpad_hat = encodeHat();

    ps3_state.lx = final_input_report.short_report.axis_lx;
    ps3_state.ly = final_input_report.short_report.axis_ly;
    ps3_state.rx = final_input_report.short_report.axis_rx;
    ps3_state.ry = final_input_report.short_report.axis_ry;

    if (final_input_report.enable_analog_facebuttons)
    {
        ps3_state.analog_dpad_right = final_input_report.analog_dpad_right;
        ps3_state.analog_dpad_left = final_input_report.analog_dpad_left;
        ps3_state.analog_dpad_up = final_input_report.analog_dpad_up;
        ps3_state.analog_dpad_down = final_input_report.analog_dpad_down;

        ps3_state.analog_triangle = final_input_report.analog_btn_north;
        ps3_state.analog_cross = final_input_report.analog_btn_south;
        ps3_state.analog_circle = final_input_report.analog_btn_east;
        ps3_state.analog_square = final_input_report.analog_btn_west;

        ps3_state.analog_l1 = final_input_report.analog_l1;
        ps3_state.analog_r1 = final_input_report.analog_r1;
        ps3_state.analog_l2 = final_input_report.analog_l2;
        ps3_state.analog_r2 = final_input_report.analog_r2;
    }
    else
    {
        ps3_state.analog_dpad_right = final_input_report.short_report.dpad_right ? 0xFF : 0x00;
        ps3_state.analog_dpad_left = final_input_report.short_report.dpad_left ? 0xFF : 0x00;
        ps3_state.analog_dpad_up = final_input_report.short_report.dpad_up ? 0xFF : 0x00;
        ps3_state.analog_dpad_down = final_input_report.short_report.dpad_down ? 0xFF : 0x00;

        ps3_state.analog_triangle = final_input_report.short_report.btn_north ? 0xFF : 0x00;
        ps3_state.analog_cross = final_input_report.short_report.btn_south ? 0xFF : 0x00;
        ps3_state.analog_circle = final_input_report.short_report.btn_east ? 0xFF : 0x00;
        ps3_state.analog_square = final_input_report.short_report.btn_west ? 0xFF : 0x00;

        ps3_state.analog_l1 = final_input_report.short_report.l1 ? 0xFF : 0x00;
        ps3_state.analog_r1 = final_input_report.short_report.r1 ? 0xFF : 0x00;
        ps3_state.analog_l2 = final_input_report.short_report.l2 ? 0xFF : 0x00;
        ps3_state.analog_r2 = final_input_report.short_report.r2 ? 0xFF : 0x00;
    }
}

void ps3_task(void)
{
    if (tud_ready())
    {
        if (!usbd_edpt_busy(0, PS3_IN_EPADDR))
        {

            ps3_makeReport();

            usbd_edpt_xfer(0, PS3_IN_EPADDR, (void *)&ps3_state, sizeof(ps3_state));
        }
    }
}