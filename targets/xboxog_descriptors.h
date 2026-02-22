#ifndef _XBOXOG_DESCRIPT_H_
#define _XBOXOG_DESCRIPT_H_

#include "tusb.h"

// see https://xboxdevwiki.net/Xbox_Input_Devices

#define XBOX_EPINSIZE 32
#define XBOX_EPOUTSIZE 32

#define ENDPOINT_DIR_OUT 0x00
#define ENDPOINT_DIR_IN 0x80

#define XBOX_IN_EPADDR (ENDPOINT_DIR_IN | 1)
#define XBOX_OUT_EPADDR (ENDPOINT_DIR_OUT | 2)

#define XID_INTERFACE_CLASS 88
#define XID_INTERFACE_SUBCLASS 66

// xbox type s controller
#define VID_XBOX 0x045e
#define PID_XBOX_CTRL_S 0x0289

enum InterfaceDescriptorsXBOX_t
{
    INTERFACE_ID_XBOX_Gamepad = 0,
    INTERFACE_ID_XBOX_Total
};

typedef enum
{
    STRID_XBOXOG_LANGID = 0,
    STRID_XBOXOG_MANUFACTURER,
    STRID_XBOXOG_PRODUCT,
    STRID_XBOXOG_SERIAL,
    STRID_XBOXOG_INTERFACE,
    STRID_XBOXOG_TOTAL
} xboxog_string_id_t;

#pragma pack(push, 1)

typedef struct TU_ATTR_PACKED
{
    tusb_desc_configuration_t config;

    tusb_desc_interface_t xboxog_interface;

    tusb_desc_endpoint_t xboxog_reportINEndpoint;
    tusb_desc_endpoint_t xboxog_reportOUTEndpoint;
} xboxog_cfg_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdXid;
    uint8_t bType;
    uint8_t bSubType;
    uint8_t bMaxInputReportSize;
    uint8_t bMaxOutputReportSize;
    uint16_t wAlternateProductIds[4];
} XIDDescriptor;

typedef struct
{
    uint8_t bReportId;
    uint8_t bLength;
    uint8_t data[18];
} XIDGamepadCapabilitiesIn;

typedef struct
{
    uint8_t bReportId;
    uint8_t bLength;
    uint8_t data[4];
} XIDGamepadCapabilitiesOut;

typedef struct
{
    uint8_t startByte; // Always 0x00
    uint8_t bLength;
    struct
    {
        uint8_t btn_dpad_up : 1;
        uint8_t btn_dpad_down : 1;
        uint8_t btn_dpad_left : 1;
        uint8_t btn_dpad_right : 1;

        uint8_t btn_start : 1;
        uint8_t btn_back : 1;
        uint8_t btn_l3 : 1;
        uint8_t btn_r3 : 1;
    };
    uint8_t reserved;
    uint8_t A;
    uint8_t B;
    uint8_t X;
    uint8_t Y;
    uint8_t BLACK;
    uint8_t WHITE;
    uint8_t L;
    uint8_t R;
    int16_t leftStickX;
    int16_t leftStickY;
    int16_t rightStickX;
    int16_t rightStickY;
} USB_XboxGamepad_Data_t;

typedef struct
{
    uint8_t startByte; // always zero
    uint8_t bLength;
    uint16_t lValue;
    uint16_t rValue;
} USB_XboxGamepad_OutReport_t;

#pragma pack(pop)

extern char const *xboxog_string_desc_arr[];
extern const size_t xboxog_string_desc_arr_len;
extern tusb_desc_device_t const xboxog_desc_device;
extern const xboxog_cfg_desc_t xboxog_desc_fs_configuration;

#endif