#include "icedragon.h"

#include "tusb.h"

char fusion_prod_string[] = "fusion-gamepad";

bool is_STAC(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == ICEDRAGON_VID_STAC &&
            (pid == ICEDRAGON_PID_STAC_P1 || pid == ICEDRAGON_PID_STAC_P2));
}

void processSTAC(uint8_t const *report, uint16_t len)
{
    if (len > sizeof(stac_state))
    {
        return;
    }

    stac_state new_stac_state = {0};
    memcpy(&new_stac_state, report, sizeof(stac_state));

    reset_report();

    input_report.short_report.dpad_up = new_stac_state.up;
    input_report.short_report.dpad_down = new_stac_state.down;
    input_report.short_report.dpad_left = new_stac_state.left;
    input_report.short_report.dpad_right = new_stac_state.right;

    input_report.short_report.btn_south = new_stac_state.center;
}

bool is_SNEK(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return vid == ICEDRAGON_VID_SNEK && (pid == ICEDRAGON_PID_SNEK);
}

void processSNEK(uint8_t const *report, uint16_t len)
{
    if (len > sizeof(snek_hid_report_t))
    {
        return;
    }

    if (report[0] == ICEDRAGON_SNEK_REPORT_ID_BUTTONS)
    {
        snek_hid_report_t new_snek_state = {0};
        memcpy(&new_snek_state, report, sizeof(snek_hid_report_t));

        reset_report();

        // p1
        input_report.short_report.dpad_up = new_snek_state.btns.btn_p1_u;
        input_report.short_report.dpad_down = new_snek_state.btns.btn_p1_d;
        input_report.short_report.dpad_left = new_snek_state.btns.btn_p1_l;
        input_report.short_report.dpad_right = new_snek_state.btns.btn_p1_r;

        // menu right is b3, menu left is b2.
        input_report.short_report.btn_south = new_snek_state.btns.btn_p1_b2;
        input_report.short_report.btn_east = new_snek_state.btns.btn_p1_b3;

        // menu up is b4, menu down is b5
        input_report.short_report.btn_north = new_snek_state.btns.btn_p1_b4;
        input_report.short_report.btn_west = new_snek_state.btns.btn_p1_b5;

        input_report.short_report.start = new_snek_state.btns.btn_p1_s;
        input_report.short_report.select = new_snek_state.btns.btn_test;

        // p2
        helper_short_report.dpad_up = new_snek_state.btns.btn_p2_u;
        helper_short_report.dpad_down = new_snek_state.btns.btn_p2_d;
        helper_short_report.dpad_left = new_snek_state.btns.btn_p2_l;
        helper_short_report.dpad_right = new_snek_state.btns.btn_p2_r;

        // menu right is b3, menu left is b2.
        helper_short_report.btn_south = new_snek_state.btns.btn_p2_b2;
        helper_short_report.btn_east = new_snek_state.btns.btn_p2_b3;

        // menu up is b4, menu down is b5
        helper_short_report.btn_north = new_snek_state.btns.btn_p2_b4;
        helper_short_report.btn_west = new_snek_state.btns.btn_p2_b5;

        helper_short_report.start = new_snek_state.btns.btn_p2_s;
        helper_short_report.select = new_snek_state.btns.btn_service;
    }
}

bool is_FUSION_HID(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    if ((vid == ICEDRAGON_VID_FUSION_HID && pid == ICEDRAGON_PID_FUSION_HID))
    {
        // used to not confused with a real piuio
        if (memcmp(current_device.product_str, fusion_prod_string, sizeof(fusion_prod_string)) == 0)
        {
            return true;
        }
        else
        {
            DebugPrintf("String cmp error on FUSION_HID");
        }
    }

    return false;
}

void processFUSION_HID(uint8_t const *report, uint16_t len)
{
    if (len > sizeof(fusion_hid_report_t))
    {
        return;
    }

    if (report[0] == ICEDRAGON_FUSION_HID_REPORT_ID_BUTTONS)
    {
        fusion_hid_report_t new_fusion_state = {0};
        memcpy(&new_fusion_state, report, sizeof(snek_hid_report_t));

        reset_report();

        // p1
        input_report.short_report.dpad_up = new_fusion_state.btns.btn_P1_UL_U;
        input_report.short_report.dpad_down = new_fusion_state.btns.btn_P1_UR_D;
        input_report.short_report.dpad_left = new_fusion_state.btns.btn_P1_CN_L;
        input_report.short_report.dpad_right = new_fusion_state.btns.btn_P1_LL_R;

        input_report.short_report.btn_south = new_fusion_state.btns.btn_P1_MENU_LEFT;
        input_report.short_report.btn_east = new_fusion_state.btns.btn_P1_MENU_RIGHT;
        //input_report.short_report.btn_north = new_fusion_state.btns.;
        //input_report.short_report.btn_west = new_fusion_state.btns.;

        input_report.short_report.start = new_fusion_state.btns.btn_P1_LR_START;
        input_report.short_report.select = new_fusion_state.btns.btn_P1_SELECT;

        // p2
        helper_short_report.dpad_up = new_fusion_state.btns.btn_P2_UL_U;
        helper_short_report.dpad_down = new_fusion_state.btns.btn_P2_UR_D;
        helper_short_report.dpad_left = new_fusion_state.btns.btn_P2_CN_L;
        helper_short_report.dpad_right = new_fusion_state.btns.btn_P2_LL_R;

        helper_short_report.btn_south = new_fusion_state.btns.btn_P2_MENU_LEFT;
        helper_short_report.btn_east = new_fusion_state.btns.btn_P2_MENU_RIGHT;
        //helper_short_report.btn_north = new_fusion_state.btns.;
        //helper_short_report.btn_west = new_fusion_state.btns.;

        helper_short_report.start = new_fusion_state.btns.btn_P2_LR_START;
        helper_short_report.select = new_fusion_state.btns.btn_P2_SELECT;
    }
}