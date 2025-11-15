#ifndef _DDR_GRANDPRIX_H_
#define _DDR_GRANDPRIX_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define DDR_GRANDPRIX_VID 0x1CCF
#define DDR_GRANDPRIX_PID 0x1020

#define DDR_GRANDPRIX_REPORT_ID 0x00

#pragma pack(push, 1)

typedef struct
{
    int8_t x;        // X axis (-127 to 127)
    int8_t y;        // Y axis (-127 to 127)
    uint16_t buttons; // 16 buttons (bits 0-15)
    uint8_t padding;  // Constant padding
} ddr_grandprix_report_t;

#pragma pack(pop)

bool is_DDR_GRANDPRIX(uint8_t dev_addr);

void processDDR_GRANDPRIX(uint8_t const *report, uint16_t len);

#endif