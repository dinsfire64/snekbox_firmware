#include "arduinokey.h"

static uint8_t prev_keys[MAX_KEY_NUM] = {0};

bool is_ARDUINOKEY(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == ARDUINOKEY_VID1 && pid == ARDUINOKEY_PID1);
}

static int contains(uint8_t key, const uint8_t *arr)
{
    for (int i = 0; i < MAX_KEY_NUM; i++)
        if (arr[i] == key)
            return 1;
    return 0;
}

void processARDUINOKEY(uint8_t const *report, uint16_t len)
{
    if (report[0] != ARDUINOKEY_REPORT_ID)
    {
        return;
    }

    // array is report, 2byte mods, then our keyboard presses.
    // so start at index 3.
    const uint8_t *keys = &report[3];
    int numKeys = len - 3;

    if (numKeys > MAX_KEY_NUM)
    {
        DebugPrintf("ERR: not enough keys.");
        return;
    }

    // --- presses ---
    for (int i = 0; i < numKeys; i++)
    {
        uint8_t k = keys[i];

        if (k == 0)
        {
            continue;
        }

        if (!contains(k, prev_keys))
        {
            switch (k)
            {
            case HID_KEY_A:
                input_report.short_report.dpad_right = 1;
                break;
            case HID_KEY_B:
                input_report.short_report.dpad_up = 1;
                break;
            case HID_KEY_C:
                input_report.short_report.dpad_left = 1;
                break;
            case HID_KEY_D:
                input_report.short_report.dpad_down = 1;
                break;
            }
        }
    }

    // --- releases ---
    for (int i = 0; i < numKeys; i++)
    {
        uint8_t k = prev_keys[i];

        if (k == 0)
        {
            continue;
        }

        if (!contains(k, keys))
        {
            switch (k)
            {
            case HID_KEY_A:
                input_report.short_report.dpad_right = 0;
                break;
            case HID_KEY_B:
                input_report.short_report.dpad_up = 0;
                break;
            case HID_KEY_C:
                input_report.short_report.dpad_left = 0;
                break;
            case HID_KEY_D:
                input_report.short_report.dpad_down = 0;
                break;
            }
        }
    }

    // save current state
    for (int i = 0; i < numKeys; i++)
    {
        prev_keys[i] = keys[i];
    }
}