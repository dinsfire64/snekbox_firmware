#include "ds3.h"

#include "tusb.h"

ds3_report_t ds3_report = {0};
sixaxis_output_report ds3_output_report = {0};

bool is_ds3_init = false;

bool is_DS3(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return vid == 0x054c && pid == 0x0268;
}

void ds3_ctrl_done(tuh_xfer_t *xfer)
{
    (void)xfer;
    return;
}

void init_ds3(uint8_t dev_addr, uint8_t instance)
{
    // magic init bytes have to be sent over ctrl transfer
    uint8_t enable_ds3[] = {0x42, 0x0C, 0x00, 0x00};

    tusb_control_request_t const request =
        {
            .bmRequestType_bit =
                {
                    .recipient = TUSB_REQ_RCPT_INTERFACE,
                    .type = TUSB_REQ_TYPE_CLASS,
                    .direction = TUSB_DIR_OUT,
                },
            .bRequest = HID_REQ_CONTROL_SET_REPORT,
            // USB get report feature is 0x03, 0xF4 is...part of the ds3 magic |:<
            .wValue = (0x03 << 8) | 0xF4,
            .wIndex = 0,
            .wLength = sizeof(enable_ds3),
        };

    tuh_xfer_t xfer =
        {
            .daddr = dev_addr,
            .ep_addr = 0,
            .setup = &request,
            .buffer = &enable_ds3[0],
            // calling NULL makes this blocking
            .complete_cb = NULL,
        };

    // blocking call
    tuh_control_xfer(&xfer);

    DebugPrintf("DS3 Init.");

    is_ds3_init = true;

    // clear the rumble state and set the LED on the ds3.
    // doing this so early in the process is buggy sometimes, so skipping.
    // setDS3Rumble(dev_addr, instance, 0xff, 0xff);
}

void processDS3(uint8_t const *report, uint16_t len)
{
    if (report[0] == 0x01 && len >= sizeof(ds3_report_t))
    {
        memcpy(&ds3_report, report, sizeof(ds3_report));
        
        reset_report();

        input_report.short_report.dpad_up = ds3_report.dpad_up;
        input_report.short_report.dpad_down = ds3_report.dpad_down;
        input_report.short_report.dpad_left = ds3_report.dpad_left;
        input_report.short_report.dpad_right = ds3_report.dpad_right;

        input_report.short_report.btn_north = ds3_report.triangle;
        input_report.short_report.btn_south = ds3_report.cross;
        input_report.short_report.btn_east = ds3_report.circle;
        input_report.short_report.btn_west = ds3_report.square;

        input_report.short_report.start = ds3_report.start;
        input_report.short_report.select = ds3_report.select;

        input_report.short_report.l1 = ds3_report.l1;
        input_report.short_report.r1 = ds3_report.r1;
        input_report.short_report.l2 = ds3_report.l2;
        input_report.short_report.r2 = ds3_report.r2;
        input_report.short_report.l3 = ds3_report.l3;
        input_report.short_report.r3 = ds3_report.r3;

        input_report.short_report.guide = ds3_report.ps_button;

        input_report.short_report.axis_lx = ds3_report.lx;
        input_report.short_report.axis_ly = ds3_report.ly;

        input_report.short_report.axis_rx = ds3_report.rx;
        input_report.short_report.axis_ry = ds3_report.ry;

        // the dualshock 3 has analog face buttons, yay!
        input_report.enable_analog_facebuttons = true;

        input_report.analog_dpad_up = ds3_report.analog_up;
        input_report.analog_dpad_down = ds3_report.analog_down;
        input_report.analog_dpad_left = ds3_report.analog_left;
        input_report.analog_dpad_right = ds3_report.analog_right;

        input_report.analog_btn_north = ds3_report.analog_triangle;
        input_report.analog_btn_south = ds3_report.analog_cross;
        input_report.analog_btn_east = ds3_report.analog_circle;
        input_report.analog_btn_west = ds3_report.analog_square;

        input_report.analog_l1 = ds3_report.analog_l1;
        input_report.analog_r1 = ds3_report.analog_r1;
        input_report.analog_l2 = ds3_report.analog_l2;
        input_report.analog_r2 = ds3_report.analog_r2;
    }
}

void setDS3Rumble(uint8_t dev_addr, uint8_t instance, uint8_t rumbleLeft, uint8_t rumbleRight)
{
    (void)instance;

    // we need to send those magic bytes before we can send rumble commands.
    if (!is_ds3_init)
    {
        return;
    }

    // taken out the report id as the ctrl transfer request wValue sets the report ID.
    // ds3_output_report.report_id = 0x01;

    // use FE incase the controller disconnects and needs to stop rumbling.
    ds3_output_report.rumble.right_duration = 0xFE;
    // for some reason it only accepts 1 or 0 for the smaller rumble motor.
    ds3_output_report.rumble.right_motor_on = (rumbleRight > 0) ? 0x01 : 0x00;

    ds3_output_report.rumble.left_duration = 0xFE;
    ds3_output_report.rumble.left_motor_force = rumbleLeft;

    // enable all of the LEDs.
    ds3_output_report.leds_bitmap = 0xFF;

    // led[3] is the "1" led, so turn it on why not.
    ds3_output_report.led[3].time_enabled = 0xFF;
    ds3_output_report.led[3].duty_length = 0xFF;
    ds3_output_report.led[3].enabled = 0xFF;
    ds3_output_report.led[3].duty_off = 0x00;
    ds3_output_report.led[3].duty_on = 0xFF;

    // DebugOutputBuffer("DS3:", (uint8_t *)&ds3_output_report, sizeof(ds3_output_report));

    // we HAVE to call the ds3 over the control transfer request as opposed to the interrupt endpoint
    // because...I have no clue blame sony. The linux driver will change to sending it over
    // the interrupt endpoint if it detects a quirky device, otherwise it forces CTRL transfer.
    // leaving this here for later in case I need it.
    // FWIW the 0x01 parameter is the report ID which I've removed from the struct.
    // tuh_hid_send_report(dev_addr, instance, 0x01, &ds3_output_report, sizeof(ds3_output_report));

    tusb_control_request_t const request =
        {
            .bmRequestType_bit =
                {
                    .recipient = TUSB_REQ_RCPT_INTERFACE,
                    .type = TUSB_REQ_TYPE_CLASS,
                    .direction = TUSB_DIR_OUT,
                },
            .bRequest = HID_REQ_CONTROL_SET_REPORT,
            // 02 is HID_SET_REPORT_OUTPUT, 01 is the report ID
            .wValue = (0x02 << 8) | 0x01,
            .wIndex = 0,
            .wLength = sizeof(ds3_output_report),
        };

    tuh_xfer_t xfer =
        {
            .daddr = dev_addr,
            .ep_addr = 0,
            .setup = &request,
            .buffer = (uint8_t *)&ds3_output_report,
            // making a function return makes this call non-blocking.
            // which is good because we want a send and forget.
            .complete_cb = ds3_ctrl_done,
        };

    tuh_control_xfer(&xfer);
}