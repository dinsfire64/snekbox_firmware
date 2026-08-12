#ifndef _SWITCH_TARGET_H_
#define _SWITCH_TARGET_H_

#include "switch_descriptors.h"

#pragma pack(push, 1)

typedef struct
{
    uint16_t Y : 1;
    uint16_t B : 1;
    uint16_t A : 1;
    uint16_t X : 1;
    uint16_t L : 1;
    uint16_t R : 1;
    uint16_t ZL : 1;
    uint16_t ZR : 1;

    uint16_t Minus : 1;
    uint16_t Plus : 1;
    uint16_t L3 : 1;
    uint16_t R3 : 1;
    uint16_t Home : 1;
    uint16_t Capture : 1;

    uint16_t reserved : 2;

    uint8_t dPad;

    uint8_t leftXAxis;
    uint8_t leftYAxis;

    uint8_t rightXAxis;
    uint8_t rightYAxis;

    uint8_t filler;
} switch_report_t;

#pragma pack(pop)

void switch_setup(void);
void switch_task(void);
void switch_makeReport(void);

#endif