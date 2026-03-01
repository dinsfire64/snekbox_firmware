#include "b2l.h"
#include "__handlers.h"

bool is_B2L(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == B2L_VID && pid == B2L_PID);
}

bool is_B2LV2(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == B2LV2_VID && pid == B2LV2_PID);
}

bool is_B2LV3(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    // todo: strcmp the product name as they are using CAFE/4004
    // since other devices will use that from a tinyusb example.
    // it should be "B2L Pad Joy"

    return (vid == B2LV3_VID && pid == B2LV3_PID);
}

void processB2L(uint8_t const *report, uint16_t len)
{
    if (report[0] == B2L_REPORT_ID && len >= sizeof(B2L_report_t))
    {
        B2L_report_t new_B2L_state = {0};
        memcpy(&new_B2L_state, report, sizeof(new_B2L_state));

        reset_report();

        // these pads throw each pad state into a different byte
        // but since we can only have one connected at a time...just OR them together.
        input_report.short_report.dpad_up = new_B2L_state.p1_up || new_B2L_state.p2_up;
        input_report.short_report.dpad_down = new_B2L_state.p1_down || new_B2L_state.p2_down;
        input_report.short_report.dpad_left = new_B2L_state.p1_left || new_B2L_state.p2_left;
        input_report.short_report.dpad_right = new_B2L_state.p1_right || new_B2L_state.p2_right;
    }
}

void processB2LV2(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(B2LV2_report_t))
    {
        B2LV2_report_t new_B2L_state = {0};
        memcpy(&new_B2L_state, report, sizeof(new_B2L_state));

        reset_report();

        // these pads throw each pad state into a different byte
        // but since we can only have one connected at a time...just OR them together.
        input_report.short_report.dpad_up = new_B2L_state.p2_up || new_B2L_state.p1_up;
        input_report.short_report.dpad_down = new_B2L_state.p2_down || new_B2L_state.p1_down;
        input_report.short_report.dpad_left = new_B2L_state.p2_left || new_B2L_state.p1_left;
        input_report.short_report.dpad_right = new_B2L_state.p2_right || new_B2L_state.p1_right;
    }
}

void processB2LV3(uint8_t const *report, uint16_t len)
{
    if (report[0] == B2L_REPORT_ID && len >= sizeof(B2LV3_report_t))
    {
        B2LV3_report_t new_B2L_state = {0};
        memcpy(&new_B2L_state, report, sizeof(new_B2L_state));

        reset_report();

        // there doesn't seem to be sides/player anymore? need more info.

        input_report.short_report.dpad_up = new_B2L_state.up;
        input_report.short_report.dpad_down = new_B2L_state.down;
        input_report.short_report.dpad_left = new_B2L_state.left;
        input_report.short_report.dpad_right = new_B2L_state.right;
    }
}