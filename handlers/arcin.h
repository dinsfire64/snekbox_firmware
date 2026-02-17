#ifndef _ARCIN_H_
#define _ARCIN_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define ARCIN_VID 0x1ccf
#define ARCIN_PID 0x8048

#define ARCIN_REPORT_ID 0x01

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
    uint8_t : 1;
    uint8_t : 1;
    uint8_t : 1;
    uint8_t : 1;
    uint8_t iidx_turntable;
    uint8_t GD_GamepadY;

} ARCIN_report_t;

#pragma pack(pop)

bool is_ARCIN(uint8_t dev_addr);

void processARCIN(uint8_t const *report, uint16_t len);

#endif