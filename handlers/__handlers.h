#ifndef _HANLDERS_H_
#define _HANLDERS_H_

#include <stdint.h>
#include "common_types.h"

#define HANDLER_OUTPUTREPORT_REPEAT_TIME_US (1000 * 1000)

typedef enum
{
    HANDLER_NONE = 0x00,

    HANDLER_DS3,
    HANDLER_DS4,
    HANDLER_DS5,

    HANDLER_SNEK,
    HANDLER_STAC,
    HANDLER_FUSION_HID,

    HANDLER_KEY,
    HANDLER_MOUSE,

    HANDLER_SMX,
    HANDLER_SWITCH_PRO,

    HANDLER_LTEK,
    HANDLER_DFORCE,

    HANDLER_PHOENIXWAN,

    HANDLER_SOFTMAT,
    HANDLER_DUAL_PS2,

    HANDLER_ZUIKI,

    HANDLER_XINPUT,

    HANDLER_SANTROLLER,
    HANDLER_B2L,
    HANDLER_STAC2,

} handler_type;

handler_type determine_handler(uint8_t dev_addr);

void handlers_task();

void encode_hat(hid_hat_t hat);

#endif