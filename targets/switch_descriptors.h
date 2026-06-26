#ifndef _SWITCH_DESC_H_
#define _SWITCH_DESC_H_

#include "tusb.h"
#include "device/usbd_pvt.h"

#define HID_GAMEPAD_EPADDR 0x81
#define HID_GAMEPAD_EPADDR2 0x02

#define HID_GAMEPAD_ITF_NUM 0

// this is taken from a Hori fight pad.
#define SWITCH_VID 0x0F0D
#define SWITCH_PID 0x0092

extern const uint8_t switch_desc_configuration[];
extern usbd_class_driver_t _switch_hid_driver;
extern const uint8_t desc_hid_report_gamepad[];
extern tusb_desc_device_t const switch_desc_device;

#endif