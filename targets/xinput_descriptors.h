#ifndef _XINPUT_DESC_H_
#define _XINPUT_DESC_H_

#include "tusb.h"

#define XINPUT_EPINSIZE 32
#define XINPUT_EPOUTSIZE 32

#define XINPUT_EPADDR_IN 0x81
#define XINPUT_EPADDR_OUT 0x02

#define VID_XINPUT 0x045E
#define PID_XINPUT 0x028E

typedef struct TU_ATTR_PACKED
{
    tusb_desc_configuration_t config;

    tusb_desc_interface_t xinput_interface;

    tusb_desc_endpoint_t xinput_reportINEndpoint;
    tusb_desc_endpoint_t xinput_reportOUTEndpoint;
} xinput_cfg_desc_t;

extern tusb_desc_device_t const xinput_desc_device;
extern uint8_t const xinput_desc_fs_configuration[];

#endif