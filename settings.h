#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "debug.h"

#define MAKE_STRING(X) #X

#define PRINT_SETTING(setting) \
    DebugPrintf(MAKE_STRING(setting) ": 0x%02x", setting);

typedef enum
{
    MODE_UNKNOWN = 0x00,

    MODE_CONSOLE_PS2 = 0x01,
    MODE_CONSOLE_GCN = 0x02,

    MODE_TOTAL_MODES
} io_card_mode;

typedef enum
{
    HELPER_MODE_NONE = 0x00,

    HELPER_MODE_SENDER = 0x01,
    HELPER_MODE_RECV = 0x02,

    HELPER_MODE_TOTAL_MODES
} helper_mode;

typedef enum
{
    USB_MODE_NONE = 0x00,

    USB_MODE_OG_XBOX = 0x01,
    USB_MODE_XINPUT = 0x02,

    USB_MODE_TOTAL_MODES
} usb_mode;

typedef struct
{
    io_card_mode current_io_mode;
    helper_mode current_helper_mode;
    usb_mode current_usb_mode;
} SnekConfig;

#define DEFAULT_IO_MODE MODE_TOTAL_MODES
#define DEFAULT_HELPER_MODE HELPER_MODE_NONE
#define DEFAULT_USB_MODE USB_MODE_OG_XBOX

extern volatile bool settings_loaded;
extern volatile SnekConfig current_settings;

void LoadSettings(void);
void SaveSettings(void);

void PrintSettings(void);
void SetDefaults(void);

#endif