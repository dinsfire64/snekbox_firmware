#ifndef _PS3_DESC_H_
#define _PS3_DESC_H_

#include "class/hid/hid.h"
#include "tusb.h"

#define PS3_EPINSIZE 32

#define PS3_IN_EPADDR (0x80 | 1)

// Konami DDR Dance Mat.
// http://classiccomp.de/Reste/GamePads/GP5.html
#define VID_PS3 0x1CCF
#define PID_PS3 0x1010

typedef struct TU_ATTR_PACKED
{
  tusb_desc_configuration_t config;
  tusb_desc_interface_t ps3_interface;
  tusb_hid_descriptor_hid_t ps3_hid_gamepad;
  tusb_desc_endpoint_t ps3_reportINEndpoint;
} ps3_cfg_desc_t;

static const uint8_t PS3_HID_Report[] = {
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x05, // USAGE (Gamepad)

    // all of the face buttons, shoulders, start/select, psbtn

    0xa1, 0x01, // COLLECTION (Application)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x01, //   LOGICAL_MAXIMUM (1)
    0x35, 0x00, //   PHYSICAL_MINIMUM (0)
    0x45, 0x01, //   PHYSICAL_MAXIMUM (1)
    0x75, 0x01, //   REPORT_SIZE (1)
    0x95, 0x0d, //   REPORT_COUNT (13)
    0x05, 0x09, //   USAGE_PAGE (Button)
    0x19, 0x01, //   USAGE_MINIMUM (Button 1)
    0x29, 0x0d, //   USAGE_MAXIMUM (Button 13)
    0x81, 0x02, //   INPUT (Data,Var,Abs)

    0x95, 0x03, //   REPORT_COUNT (3)
    0x81, 0x01, //   INPUT (Cnst,Ary,Abs)

    // eight direction HAT

    0x05, 0x01,       //   USAGE_PAGE (Generic Desktop)
    0x25, 0x07,       //   LOGICAL_MAXIMUM (7)
    0x46, 0x3b, 0x01, //   PHYSICAL_MAXIMUM (315)
    0x75, 0x04,       //   REPORT_SIZE (4)
    0x95, 0x01,       //   REPORT_COUNT (1)
    0x65, 0x14,       //   UNIT (Eng Rot:Angular Pos)
    0x09, 0x39,       //   USAGE (Hat switch)
    0x81, 0x42,       //   INPUT (Data,Var,Abs,Null)

    // two thumb sticks, two triggers

    0x65, 0x00,       //   UNIT (None)
    0x95, 0x01,       //   REPORT_COUNT (1)
    0x81, 0x01,       //   INPUT (Cnst,Ary,Abs)
    0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
    0x46, 0xff, 0x00, //   PHYSICAL_MAXIMUM (255)
    0x09, 0x30,       //   USAGE (X)
    0x09, 0x31,       //   USAGE (Y)
    0x09, 0x32,       //   USAGE (Z)
    0x09, 0x35,       //   USAGE (Rz)
    0x75, 0x08,       //   REPORT_SIZE (8)
    0x95, 0x04,       //   REPORT_COUNT (4)

    // per button analog controls.
    // First four (DPAD) are used by the DDR game.

    0x81, 0x02,       //   INPUT (Data,Var,Abs)
    0x06, 0x00, 0xff, //   USAGE_PAGE (Vendor Specific)
    0x09, 0x20,       //   Unknown (Right)
    0x09, 0x21,       //   Unknown (Left)
    0x09, 0x22,       //   Unknown (Up)
    0x09, 0x23,       //   Unknown (Down)
    0x09, 0x24,       //   Unknown
    0x09, 0x25,       //   Unknown
    0x09, 0x26,       //   Unknown
    0x09, 0x27,       //   Unknown
    0x09, 0x28,       //   Unknown
    0x09, 0x29,       //   Unknown
    0x09, 0x2a,       //   Unknown
    0x09, 0x2b,       //   Unknown
    0x95, 0x0c,       //   REPORT_COUNT (12)
    0x81, 0x02,       //   INPUT (Data,Var,Abs)
    0x0a, 0x21, 0x26, //   Unknown
    0x95, 0x08,       //   REPORT_COUNT (8)
    0xb1, 0x02,       //   FEATURE (Data,Var,Abs)
    0xc0              // END_COLLECTION
};

extern tusb_desc_device_t const ps3_desc_device;
extern const ps3_cfg_desc_t ps3_desc_fs_configuration;

#endif