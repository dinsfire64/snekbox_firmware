#include "smx.h"

char smx_prod_string[] = "StepManiaX";

bool is_SMX(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    if ((vid == SMX_VID && pid == SMX_PID))
    {
        // Step Revolution reused the Arduino vid/pid combo and expects to check the product string
        // for the device in question...horray
        if (memcmp(current_device.product_str, smx_prod_string, sizeof(smx_prod_string)) == 0)
        {
            return true;
        }
        else
        {
            // DebugPrintf("String cmp error on SMX");
        }
    }

    return false;
}

void processSMX(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(smx_state) && report[0] == SMX_REPORT_GAMEPAD_STATE)
    {
        smx_state new_smx_state = {0};
        memcpy(&new_smx_state, report, sizeof(new_smx_state));

        reset_report();

        input_report.short_report.dpad_up = new_smx_state.buttons.val.uc;
        input_report.short_report.dpad_down = new_smx_state.buttons.val.dc;
        input_report.short_report.dpad_left = new_smx_state.buttons.val.cl;
        input_report.short_report.dpad_right = new_smx_state.buttons.val.cr;

        input_report.short_report.btn_north = new_smx_state.buttons.val.dl;
        input_report.short_report.btn_south = new_smx_state.buttons.val.ul;
        input_report.short_report.btn_east = new_smx_state.buttons.val.ur;
        input_report.short_report.btn_west = new_smx_state.buttons.val.dr;
    }
}
