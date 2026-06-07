#include "__handlers.h"

#include "common_types.h"

#include "helpers.h"
#include "settings.h"

uint64_t prev_output_report_time = 0;
uint64_t prev_btn_sampling_time = 0;

input_report_short_t new_rpt;

handler_type determine_handler(uint8_t dev_addr)
{
    handler_type rtn = HANDLER_NONE;

    if (current_device.is_xinput)
    {
        rtn = HANDLER_XINPUT;
    }
    else if (is_DS3(dev_addr))
    {
        rtn = HANDLER_DS3;
    }
    else if (is_DS4(dev_addr))
    {
        rtn = HANDLER_DS4;
    }
    else if (is_DS5(dev_addr))
    {
        rtn = HANDLER_DS5;
    }
    else if (is_SMX(dev_addr))
    {
        rtn = HANDLER_SMX;
    }
    else if (is_SNEK(dev_addr))
    {
        rtn = HANDLER_SNEK;
    }
    else if (is_STAC(dev_addr))
    {
        rtn = HANDLER_STAC;
    }
    else if (is_STAC2(dev_addr))
    {
        rtn = HANDLER_STAC2;
    }
    else if (is_SWITCH_PRO(dev_addr))
    {
        rtn = HANDLER_SWITCH_PRO;
    }
    else if (is_LTEK(dev_addr))
    {
        rtn = HANDLER_LTEK;
    }
    else if (is_DFORCE(dev_addr))
    {
        rtn = HANDLER_DFORCE;
    }
    else if (is_PHOENIXWAN(dev_addr))
    {
        rtn = HANDLER_PHOENIXWAN;
    }
    else if (is_SOFTMAT(dev_addr))
    {
        rtn = HANDLER_SOFTMAT;
    }
    else if (is_DUAL_PS2(dev_addr))
    {
        rtn = HANDLER_DUAL_PS2;
    }
    else if (is_ZUIKI(dev_addr))
    {
        rtn = HANDLER_ZUIKI;
    }
    else if (is_FUSION_HID(dev_addr))
    {
        rtn = HANDLER_FUSION_HID;
    }
    else if (is_SANTROLLER(dev_addr))
    {
        rtn = HANDLER_SANTROLLER;
    }
    else if (is_B2L(dev_addr))
    {
        rtn = HANDLER_B2L;
    }
    else if (is_B2LV2(dev_addr))
    {
        rtn = HANDLER_B2LV2;
    }
    else if (is_B2LV3(dev_addr))
    {
        rtn = HANDLER_B2LV3;
    }
    else if (is_DDR_GRANDPRIX(dev_addr))
    {
        rtn = HANDLER_DDR_GRANDPRIX;
    }
    else if (is_ARDUINOKEY(dev_addr))
    {
        rtn = HANDLER_ARDUINOKEY;
    }

    return rtn;
}

void handlers_task()
{
    uint64_t curr_time = time_us_64();

    if (current_device.mounted)
    {
        // even if the report hasn't changed, there's a chance that the vibration needs keep being sent
        // since some controllers will timeout of a rumble operation.
        if (output_report.new_output_report ||
            curr_time - prev_output_report_time >= HANDLER_OUTPUTREPORT_REPEAT_TIME_US)
        {
            switch (determine_handler(current_device.dev_addr))
            {
                // DISPATCH_NEW_RUMBLE(DS3)
                // DISPATCH_NEW_RUMBLE(DS4)
                // DISPATCH_NEW_RUMBLE(SWITCH_PRO)
                // DISPATCH_NEW_RUMBLE(XINPUT)

            case HANDLER_ZUIKI:
                setZuikiLights(current_device.dev_addr, current_device.instance);
                break;

            default:
                break;
            }

            output_report.new_output_report = false;
            prev_output_report_time = time_us_64();
        }
    }

    if (curr_time - prev_btn_sampling_time > 5 * 1000)
    {
        // Buttons are active-low.
        bool btn_south = !gpio_get(PIN_SNEKBOX_BTN1);
        bool btn_start = !gpio_get(PIN_SNEKBOX_BTN2);
        bool btn_east = !gpio_get(PIN_SNEKBOX_BTN3);
        bool btn_select = !gpio_get(PIN_SNEKBOX_BTN4);

        // hold select and east (b/circle) to open guide.
        bool guide_pressed = btn_start && btn_east;

        new_rpt.guide = guide_pressed;
        new_rpt.start = guide_pressed ? false : btn_start;
        new_rpt.select = btn_select;
        new_rpt.btn_south = btn_south;
        new_rpt.btn_east = guide_pressed ? false : btn_east;

        prev_btn_sampling_time = time_us_64();
    }

    mux_report(new_rpt);
}

void encode_hat(hid_hat_t hat)
{
    // reset at start
    input_report.short_report.dpad_up = false;
    input_report.short_report.dpad_down = false;
    input_report.short_report.dpad_left = false;
    input_report.short_report.dpad_right = false;

    switch (hat)
    {
    case HID_HAT_NONE:
        // do nothing as it was just reset.
        break;

    case HID_HAT_UP:
        input_report.short_report.dpad_up = true;
        break;

    case HID_HAT_UP_RIGHT:
        input_report.short_report.dpad_up = true;
        input_report.short_report.dpad_right = true;
        break;

    case HID_HAT_RIGHT:
        input_report.short_report.dpad_right = true;
        break;

    case HID_HAT_RIGHT_DOWN:
        input_report.short_report.dpad_right = true;
        input_report.short_report.dpad_down = true;
        break;

    case HID_HAT_DOWN:
        input_report.short_report.dpad_down = true;
        break;

    case HID_HAT_DOWN_LEFT:
        input_report.short_report.dpad_down = true;
        input_report.short_report.dpad_left = true;
        break;

    case HID_HAT_LEFT:
        input_report.short_report.dpad_left = true;
        break;

    case HID_HAT_UP_LEFT:
        input_report.short_report.dpad_up = true;
        input_report.short_report.dpad_left = true;
        break;

    default:
        break;
    }
}