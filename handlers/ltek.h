#ifndef _LTEK_H_
#define _LTEK_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define LTEK_VID1 0x6667
#define LTEK_PID1 0xc006

#define LTEK_VID2 0x03EB
#define LTEK_PID2 0x8041

#define LTEK_VID3 0x37F0
#define LTEK_PID3 0x1001

#define LTEK_REPORT_ID 0x01

#pragma pack(push, 1)

typedef struct
{
    uint8_t reportId;

    // buttons 1-8
    uint8_t left : 1;
    uint8_t right : 1;
    uint8_t up : 1;
    uint8_t down : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
    uint8_t b8 : 1;

    // buttons 9-16
    uint8_t b9 : 1;
    uint8_t b10 : 1;
    uint8_t start : 1;
    uint8_t select : 1;
    uint8_t b13 : 1;
    uint8_t b14 : 1;
    uint8_t b15 : 1;
    uint8_t b16 : 1;
} ltek_report_t;

#pragma pack(pop)

bool is_LTEK(uint8_t dev_addr);

void processLTEK(uint8_t const *report, uint16_t len);

#endif
