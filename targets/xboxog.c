#include "xboxog.h"

#include "tusb.h"
#include "device/usbd_pvt.h"

#include "common_types.h"

uint8_t xbox_input_data[XBOX_EPINSIZE];

USB_XboxGamepad_OutReport_t prev_xbox_output_state;
USB_XboxGamepad_Data_t xbox_controller_state;

void xboxog_setup(void)
{
    // Setup the first report.
    memset(&xbox_controller_state, 0x00, sizeof(xbox_controller_state));

    // start byte is always zero
    xbox_controller_state.startByte = 0x00;

    // second byte is always the length of the packet (which is static in this case.)
    xbox_controller_state.bLength = sizeof(xbox_controller_state);

    memset(&prev_xbox_output_state, 0x00, sizeof(prev_xbox_output_state));
    memset(&xbox_input_data, 0x00, sizeof(xbox_input_data));
}

void xboxog_makeReport(void)
{
    xbox_controller_state.startByte = 0x00;
    xbox_controller_state.bLength = sizeof(xbox_controller_state);

    xbox_controller_state.btn_dpad_up = final_input_report.short_report.dpad_up;
    xbox_controller_state.btn_dpad_down = final_input_report.short_report.dpad_down;
    xbox_controller_state.btn_dpad_left = final_input_report.short_report.dpad_left;
    xbox_controller_state.btn_dpad_right = final_input_report.short_report.dpad_right;

    xbox_controller_state.btn_start = final_input_report.short_report.start;
    xbox_controller_state.btn_back = final_input_report.short_report.select;
    xbox_controller_state.btn_l3 = final_input_report.short_report.l3;
    xbox_controller_state.btn_r3 = final_input_report.short_report.r3;

    if (final_input_report.enable_analog_facebuttons)
    {
        xbox_controller_state.A = final_input_report.analog_btn_south;
        xbox_controller_state.B = final_input_report.analog_btn_east;
        xbox_controller_state.X = final_input_report.analog_btn_west;
        xbox_controller_state.Y = final_input_report.analog_btn_north;

        xbox_controller_state.BLACK = final_input_report.analog_r1;
        xbox_controller_state.WHITE = final_input_report.analog_l1;
    }
    else
    {
        xbox_controller_state.A = final_input_report.short_report.btn_south ? 0xFF : 0x00;
        xbox_controller_state.B = final_input_report.short_report.btn_east ? 0xFF : 0x00;
        xbox_controller_state.X = final_input_report.short_report.btn_west ? 0xFF : 0x00;
        xbox_controller_state.Y = final_input_report.short_report.btn_north ? 0xFF : 0x00;

        xbox_controller_state.BLACK = final_input_report.short_report.r1 ? 0xFF : 0x00;
        xbox_controller_state.WHITE = final_input_report.short_report.l1 ? 0xFF : 0x00;
    }

    xbox_controller_state.L = final_input_report.analog_l2;
    xbox_controller_state.R = final_input_report.analog_r2;

    // convert our local uint8_t sick values to int16_t values for XID.
    xbox_controller_state.leftStickX = ((final_input_report.short_report.axis_lx - 0x80) << 8);
    xbox_controller_state.leftStickY = -1 * ((final_input_report.short_report.axis_ly - 0x7F) << 8);
    xbox_controller_state.rightStickX = ((final_input_report.short_report.axis_rx - 0x80) << 8);
    xbox_controller_state.rightStickY = -1 * ((final_input_report.short_report.axis_ry - 0x7F) << 8);

    // DebugOutputBuffer("XBX", (void*)&xbox_controller_state, sizeof(xbox_controller_state));
}

void xboxog_process_output(uint8_t report_size)
{
    // this is the rumble output.
    if (xbox_input_data[0] == 0x00 &&
        xbox_input_data[1] == XID_REPORTSIZE_OUT)
    {
        USB_XboxGamepad_OutReport_t new_report;
        memcpy(&new_report, xbox_input_data, XID_REPORTSIZE_OUT);

        if (memcmp(&new_report, &prev_xbox_output_state, XID_REPORTSIZE_OUT))
        {
            // DebugOutputBuffer("RBL", xbox_input_data, XID_REPORTSIZE_OUT);

            output_report.new_output_report = true;
            output_report.rumbleLarge = new_report.lValue >> 8;
            output_report.rumbleSmall = new_report.rValue >> 8;

            memcpy(&prev_xbox_output_state, &new_report, XID_REPORTSIZE_OUT);
        }
    }
}

void xboxog_task(void)
{
    if (tud_ready())
    {
        if (!usbd_edpt_busy(0, XBOX_IN_EPADDR))
        {
            xboxog_makeReport();
            usbd_edpt_xfer(0, XBOX_IN_EPADDR, (void *)&xbox_controller_state, sizeof(xbox_controller_state), false);
        }

        if (!usbd_edpt_busy(0, XBOX_OUT_EPADDR))
        {
            usbd_edpt_xfer(0, XBOX_OUT_EPADDR, (void *)&xbox_input_data, sizeof(xbox_input_data), false);
        }
    }
}