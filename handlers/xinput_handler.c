#include "xinput_handler.h"

#include "settings.h"
#include "common_types.h"
#include "ws2812.h"
#include "i2c.h"

#define XINPUT_TRIGGER_THRESHOLD 0x05

bool prev_guide_button = false;

// for some reason on some controllers during startup it would ignore the first command
// so let's send it multiple times for good measure.
// for example if a wireless controller is booting while the system is booting
// we might send the command too early.
#define XINPUT_LED_SET_MAX 8
uint8_t set_led_xinput = 0;

// Since https://github.com/hathach/tinyusb/pull/2222, we can add in custom vendor drivers easily
usbh_class_driver_t const *usbh_app_driver_get_cb(uint8_t *driver_count)
{
    *driver_count = 1;
    return &usbh_xinput_driver;
}

static inline bool is_xplorer(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == 0x1430 && pid == 0x4748);
}

static inline bool is_tatacon(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    // HORI Taiko Drum Controller for PS5 (PC mode)
    return (vid == 0x0f0d && pid == 0x01b1); // || (...)
}

void convertXInputToLocal(uint8_t dev_addr, uint8_t instance, xinputh_interface_t const *xid_itf, uint16_t len)
{
    (void)len;
    (void)instance;

    const xinput_gamepad_t *p = &xid_itf->pad;

    reset_report();

    if (is_xplorer(dev_addr))
    {
        input_report.short_report.controller_type = SPECIAL_CONTROLLER_GUITAR;

        // whammy
        input_report.short_report.axis_ly = (-1 * ((uint8_t)(p->sThumbRX >> 8) - 0x7F));

        // strum
        input_report.short_report.dpad_up = (p->wButtons & XINPUT_GAMEPAD_DPAD_UP);
        input_report.short_report.dpad_down = (p->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);

        // option buttons
        input_report.short_report.start = (p->wButtons & XINPUT_GAMEPAD_START);
        input_report.short_report.select = (p->wButtons & XINPUT_GAMEPAD_BACK);

        // green, red, yellow, blue, orange (ps2 mapping)
        // r2, circle, tri, cross, square
        input_report.short_report.r2 = (p->wButtons & XINPUT_GAMEPAD_A);
        input_report.short_report.btn_east = (p->wButtons & XINPUT_GAMEPAD_B);
        input_report.short_report.btn_north = (p->wButtons & XINPUT_GAMEPAD_Y);
        input_report.short_report.btn_south = (p->wButtons & XINPUT_GAMEPAD_X);
        input_report.short_report.btn_west = (p->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
    }
    else if (is_tatacon(dev_addr))
    {
        // don
        input_report.short_report.dpad_left = (p->wButtons & (XINPUT_GAMEPAD_LEFT_THUMB | XINPUT_GAMEPAD_DPAD_LEFT));
        input_report.short_report.btn_east = (p->wButtons & (XINPUT_GAMEPAD_RIGHT_THUMB | XINPUT_GAMEPAD_B));

        // ka
        input_report.short_report.l1 = (p->bLeftTrigger >= XINPUT_TRIGGER_THRESHOLD) || (p->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
        input_report.short_report.r1 = (p->bRightTrigger >= XINPUT_TRIGGER_THRESHOLD) || (p->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

        // face buttons
        input_report.short_report.dpad_up = (p->wButtons & XINPUT_GAMEPAD_DPAD_UP);
        input_report.short_report.dpad_down = (p->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
        input_report.short_report.dpad_right = (p->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

        input_report.short_report.btn_north = (p->wButtons & XINPUT_GAMEPAD_Y);
        input_report.short_report.btn_south = (p->wButtons & XINPUT_GAMEPAD_A);
        input_report.short_report.btn_west = (p->wButtons & XINPUT_GAMEPAD_X);

        input_report.short_report.start = (p->wButtons & XINPUT_GAMEPAD_START);
        input_report.short_report.select = (p->wButtons & XINPUT_GAMEPAD_BACK);
    }
    else
    {
        input_report.short_report.dpad_up = (p->wButtons & XINPUT_GAMEPAD_DPAD_UP);
        input_report.short_report.dpad_down = (p->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
        input_report.short_report.dpad_left = (p->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
        input_report.short_report.dpad_right = (p->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

        input_report.short_report.btn_north = (p->wButtons & XINPUT_GAMEPAD_Y);
        input_report.short_report.btn_south = (p->wButtons & XINPUT_GAMEPAD_A);
        input_report.short_report.btn_east = (p->wButtons & XINPUT_GAMEPAD_B);
        input_report.short_report.btn_west = (p->wButtons & XINPUT_GAMEPAD_X);

        input_report.short_report.start = (p->wButtons & XINPUT_GAMEPAD_START);
        input_report.short_report.select = (p->wButtons & XINPUT_GAMEPAD_BACK);

        input_report.short_report.l1 = (p->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
        input_report.short_report.r1 = (p->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
        input_report.short_report.l2 = (p->bLeftTrigger >= XINPUT_TRIGGER_THRESHOLD);
        input_report.short_report.r2 = (p->bRightTrigger >= XINPUT_TRIGGER_THRESHOLD);
        input_report.short_report.l3 = (p->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
        input_report.short_report.r3 = (p->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);

        input_report.short_report.guide = (p->wButtons & XINPUT_GAMEPAD_GUIDE);

        input_report.short_report.axis_lx = (uint8_t)(p->sThumbLX >> 8) - 0x80;
        input_report.short_report.axis_ly = -1 * ((uint8_t)(p->sThumbLY >> 8) - 0x7F);

        input_report.short_report.axis_rx = (uint8_t)(p->sThumbRX >> 8) - 0x80;
        input_report.short_report.axis_ry = -1 * ((uint8_t)(p->sThumbRY >> 8) - 0x7F);

        input_report.analog_l2 = p->bLeftTrigger;
        input_report.analog_r2 = p->bRightTrigger;
    }
}

void tuh_xinput_report_received_cb(uint8_t dev_addr, uint8_t instance, xinputh_interface_t const *xid_itf, uint16_t len)
{
    if (xid_itf->last_xfer_result == XFER_RESULT_SUCCESS)
    {
        if (xid_itf->connected)
        {
            if (xid_itf->new_pad_data)
            {
                // const xinput_gamepad_t *p = &xid_itf->pad;
                // DebugPrintf("[%02x, %02x], Buttons %04x, LT: %02x RT: %02x, LX: %04x, LY: %04x, RX: %04x, RY: %04x", dev_addr, instance, p->wButtons, p->bLeftTrigger, p->bRightTrigger, p->sThumbLX, p->sThumbLY, p->sThumbRX, p->sThumbRY);

                convertXInputToLocal(dev_addr, instance, xid_itf, len);
            }

            if (set_led_xinput < XINPUT_LED_SET_MAX)
            {
                // DebugPrintf("Sending LED %d", set_led_xinput);

                tuh_xinput_set_led(dev_addr, instance, 1, true);
                set_led_xinput++;
            }
        }
        else
        {
            set_led_xinput = 0;
        }
    }

    tuh_xinput_receive_report(dev_addr, instance);
}

void tuh_xinput_mount_cb(uint8_t dev_addr, uint8_t instance, const xinputh_interface_t *xinput_itf)
{
    DebugPrintf("XINPUT MOUNTED %02x %d", dev_addr, instance);

    set_rgb1(0, 0xFF, 0);

    current_device.mounted = true;
    current_device.is_xinput = true;
    current_device.dev_addr = dev_addr;
    current_device.instance = instance;

    set_led_xinput = false;

    // If this is a Xbox 360 Wireless controller we need to wait for a connection packet
    // on the in pipe before setting LEDs etc. So just start getting data until a controller is connected.
    if (xinput_itf->type == XBOX360_WIRELESS && xinput_itf->connected == false)
    {
        tuh_xinput_receive_report(dev_addr, instance);
        return;
    }

    tuh_xinput_receive_report(dev_addr, instance);
}

void tuh_xinput_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    DebugPrintf("XINPUT UNMOUNTED %02x %d\n", dev_addr, instance);

    current_device.mounted = false;
    current_device.is_xinput = false;

    set_led_xinput = false;
}

void setXINPUTRumble(uint8_t dev_addr, uint8_t instance, uint8_t rumbleLeft, uint8_t rumbleRight)
{
    tuh_xinput_set_rumble(dev_addr,
                          instance,
                          rumbleLeft,
                          rumbleRight,
                          false);
}