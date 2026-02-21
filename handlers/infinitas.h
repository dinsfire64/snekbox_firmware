#ifndef _INFINITAS_H_
#define _INFINITAS_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define INFINITAS_VID 0x1ccf
#define INFINITAS_PID 0x8048

#define INFINITAS_REPORT_ID 0x01

#pragma pack(push, 1)

// decoded using hidrdd
typedef struct
{
    uint8_t reportId;

    uint8_t iidx_btn_01 : 1;
    uint8_t iidx_btn_02 : 1;
    uint8_t iidx_btn_03 : 1;
    uint8_t iidx_btn_04 : 1;
    uint8_t iidx_btn_05 : 1;
    uint8_t iidx_btn_06 : 1;
    uint8_t iidx_btn_07 : 1;
    uint8_t : 1;
    uint8_t iidx_btn_start : 1;
    uint8_t iidx_btn_select : 1;
    uint8_t : 1;
    uint8_t : 1;
    uint8_t iidx_turntable_cw : 1;  // arcin feature
    uint8_t iidx_turntable_ccw : 1; // arcin feature
    uint8_t : 1;
    uint8_t : 1;
    uint8_t iidx_turntable;
    uint8_t GD_GamepadY;

} INFINITAS_report_t;

#pragma pack(pop)

bool is_INFINITAS(uint8_t dev_addr);

void processINFINITAS(uint8_t const *report, uint16_t len);

#endif