#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "debug.h"
#include "hardware/flash.h"

#define MAKE_STRING(X) #X

#define PRINT_SETTING(setting) \
    DebugPrintf(MAKE_STRING(setting) ": 0x%02x", setting);

// magic word for settings to verify it loaded correctly.
#define SETTINGS_MAGIC 0x4B1D4B1D

// store the settings at the last possible erasable flash page.
#define SETTINGS_QSPI_ADDRESS (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

// save one byte for the CRC at the end of the page.
#define CONFIG_DATA_SIZE (FLASH_PAGE_SIZE - 1)

// time in us for how long the user has to hold the button combo to change modes.
#define CHANGE_MODE_US (5 * 1000 * 1000)

#pragma pack(push, 1)

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

    // save room for potential debug modes here.

    USB_MODE_OG_XBOX = 0x11,
    USB_MODE_XINPUT = 0x12,
    USB_MODE_PS3 = 0x13,

    USB_MODE_TOTAL_MODES
} usb_mode;

typedef struct
{
    io_card_mode retro_io_mode;
    helper_mode helper_mode;
} SnekConfigRuntime_t;

typedef struct
{
    // start of settings
    uint32_t magic;

    usb_mode current_usb_mode;

    // always at the end.
    uint8_t crc;
} SnekConfigSaved_t;

#pragma pack(pop)

_Static_assert(offsetof(SnekConfigSaved_t, crc) == sizeof(SnekConfigSaved_t) - sizeof(uint8_t),
               "crc must be the last field in SnekConfigSaved_t");
_Static_assert(sizeof(SnekConfigSaved_t) <= FLASH_SECTOR_SIZE,
               "SnekConfigSaved_t size exceeded flash erase, please revise the erase and offset.");

#define DEFAULT_USB_MODE USB_MODE_OG_XBOX

extern volatile bool settings_loaded;
extern volatile SnekConfigSaved_t saved_settings;
extern volatile SnekConfigRuntime_t runtime_settings;

void LoadSettings(void);
void SaveSettings(bool parkCore1);

void PrintSettings(void);
void SetDefaults(void);

void SettingsTask();

#endif