#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "input_report.h"

#define INPUT_STATE_AXIS_MIDDLE 0x80

typedef enum
{
    HID_HAT_NONE = 0x0F,

    HID_HAT_UP = 0x00,
    HID_HAT_UP_RIGHT = 0x01,
    HID_HAT_RIGHT = 0x02,
    HID_HAT_RIGHT_DOWN = 0x03,
    HID_HAT_DOWN = 0x04,
    HID_HAT_DOWN_LEFT = 0x05,
    HID_HAT_LEFT = 0x06,
    HID_HAT_UP_LEFT = 0x07,
} hid_hat_t;

typedef struct
{
    bool mounted;
    uint8_t dev_addr;
    uint8_t instance;
    bool is_xinput;
    char product_str[256];
} usb_host_dev_info_t;

#pragma pack(push, 1)

typedef struct
{
    bool new_output_report;

    uint8_t rumbleLarge;
    uint8_t rumbleSmall;

    bool rumble_active;
} output_report_t;

#pragma pack(pop)

extern output_report_t output_report;

extern usb_host_dev_info_t current_device;

#endif