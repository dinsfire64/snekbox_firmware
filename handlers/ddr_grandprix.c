#include "ddr_grandprix.h"
#include <string.h>
#include "input_report.h"

bool is_DDR_GRANDPRIX(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    return (vid == DDR_GRANDPRIX_VID && pid == DDR_GRANDPRIX_PID);
}

void processDDR_GRANDPRIX(uint8_t const *report, uint16_t len)
{
    if (len >= sizeof(ddr_grandprix_report_t))
    {
        ddr_grandprix_report_t new_ddr_state = {0};
        memcpy(&new_ddr_state, report, sizeof(new_ddr_state));
        
        reset_report();

        // Button mapping according to the specification:
        // L       -> Button 1  (bit 0)  -> select
        // R       -> Button 2  (bit 1)  -> start
        // Up      -> Button 3  (bit 2)  -> dpad_up
        // Right   -> Button 4  (bit 3)  -> dpad_right
        // Down    -> Button 5  (bit 4)  -> dpad_down
        // Left    -> Button 6  (bit 5)  -> dpad_left
        // Circle  -> Button 9  (bit 8)  -> btn_east
        // Cross   -> Button 10 (bit 9)  -> btn_south

        input_report.short_report.select = (new_ddr_state.buttons & (1 << 0)) != 0;      // Button 1 (L)
        input_report.short_report.start = (new_ddr_state.buttons & (1 << 1)) != 0;       // Button 2 (R)
        input_report.short_report.dpad_up = (new_ddr_state.buttons & (1 << 2)) != 0;     // Button 3 (Up)
        input_report.short_report.dpad_right = (new_ddr_state.buttons & (1 << 3)) != 0;  // Button 4 (Right)
        input_report.short_report.dpad_down = (new_ddr_state.buttons & (1 << 4)) != 0;   // Button 5 (Down)
        input_report.short_report.dpad_left = (new_ddr_state.buttons & (1 << 5)) != 0;   // Button 6 (Left)
        input_report.short_report.btn_east = (new_ddr_state.buttons & (1 << 8)) != 0;    // Button 9 (Circle)
        input_report.short_report.btn_south = (new_ddr_state.buttons & (1 << 9)) != 0;   // Button 10 (Cross)
    }
}