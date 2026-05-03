#include "input_report.h"

#include <stdio.h>
#include <string.h>

input_report_long_t input_report;

bool en_helper_report = false;
input_report_short_t helper_short_report;

input_report_long_t final_input_report;

void reset_report()
{
    // all active low.
    memset(&input_report, 0x00, sizeof(input_report));

    input_report.short_report.axis_lx = INPUT_STATE_AXIS_MIDDLE;
    input_report.short_report.axis_ly = INPUT_STATE_AXIS_MIDDLE;
    input_report.short_report.axis_rx = INPUT_STATE_AXIS_MIDDLE;
    input_report.short_report.axis_ry = INPUT_STATE_AXIS_MIDDLE;

    memset(&helper_short_report, 0x00, sizeof(helper_short_report));

    helper_short_report.axis_lx = INPUT_STATE_AXIS_MIDDLE;
    helper_short_report.axis_ly = INPUT_STATE_AXIS_MIDDLE;
    helper_short_report.axis_rx = INPUT_STATE_AXIS_MIDDLE;
    helper_short_report.axis_ry = INPUT_STATE_AXIS_MIDDLE;
}

void mux_report(input_report_short_t short_rpt)
{
    memcpy(&final_input_report, &input_report, sizeof(input_report_long_t));

    // TODO add more here but I'm only using the front face buttons for the PCB.
    final_input_report.short_report.btn_east = input_report.short_report.btn_east | short_rpt.btn_east;
    final_input_report.short_report.btn_south = input_report.short_report.btn_south | short_rpt.btn_south;

    final_input_report.short_report.start = input_report.short_report.start | short_rpt.start;
    final_input_report.short_report.select = input_report.short_report.select | short_rpt.select;

    final_input_report.short_report.guide = input_report.short_report.guide | short_rpt.guide;
};