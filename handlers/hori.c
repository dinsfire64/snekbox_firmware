#include "hori.h"
#include "__handlers.h"
#include "common_types.h"
#include "targets/xinput_descriptors.h"

bool is_HORI_POKKENWIIU(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == HORI_VID_POKKENWIIU && pid == HORI_PID_POKKENWIIU);
}

void processHORI_POKKENWIIU(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(HORI_POKKENWIIU_report_t))
    {
        HORI_POKKENWIIU_report_t new_HORI_state = {0};
        memcpy(&new_HORI_state, report, sizeof(new_HORI_state));

        reset_report();

        hat_to_local(new_HORI_state.GD_GamepadHatSwitch);

        input_report.short_report.btn_north = new_HORI_state.X;
        input_report.short_report.btn_south = new_HORI_state.B;
        input_report.short_report.btn_east = new_HORI_state.A;
        input_report.short_report.btn_west = new_HORI_state.Y;

        // note: this controller is primarly used for the pokken arcade, which uses the bumpers of xinput for the
        // triggers.
        // so we are mapping the triggers to l1/2 for our internal state as a result.
        input_report.short_report.l1 = new_HORI_state.L;
        input_report.short_report.r1 = new_HORI_state.R;

#if (POKKEN_CONTROLLER)
        // if we are the arcade pokken controller, map any of these buttons to start
        // since the consumer controller has multiple buttons while the arcade has just one.
        // just make it easy for "anyone to press any button" to start.

        input_report.short_report.start |= new_HORI_state.start;
        input_report.short_report.start |= new_HORI_state.select;
        input_report.short_report.start |= new_HORI_state.ZL;
        input_report.short_report.start |= new_HORI_state.ZR;

#else
        input_report.short_report.start = new_HORI_state.start;
        input_report.short_report.select = new_HORI_state.select;

        // these are face buttons on the pokken controller, not actual triggers.
        input_report.short_report.l2 = new_HORI_state.ZL;
        input_report.short_report.r2 = new_HORI_state.ZR;

        // this gamepad only had digital triggers, so fake the analog support.
        input_report.analog_l2 = new_HORI_state.ZL ? 0xFF : 0x00;
        input_report.analog_r2 = new_HORI_state.ZR ? 0xFF : 0x00;
#endif
    }
}
