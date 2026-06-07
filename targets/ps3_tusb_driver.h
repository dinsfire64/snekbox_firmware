#ifndef _PS3_TUSB_DRV_H_
#define _PS3_TUSB_DRV_H_

#include "device/usbd_pvt.h"
#include "tusb.h"

static const uint8_t ps3_magic_bytes[] = {
    0x21,
    0x26,
    0x01,
    0x07,
    0x00,
    0x00,
    0x00,
    0x00,
};

bool ps3d_control_request_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request);

extern usbd_class_driver_t const _ps3d_driver;

#endif