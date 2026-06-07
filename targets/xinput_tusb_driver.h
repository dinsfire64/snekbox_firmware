#ifndef _XINPUT_TUSB_DRV_H_
#define _XINPUT_TUSB_DRV_H_

#include "device/usbd_pvt.h"
#include "tusb.h"

bool xinputd_control_request_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request);

extern usbd_class_driver_t const _xinputd_driver;

#endif