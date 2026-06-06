#include "settings.h"

#include <hardware/flash.h>
#include <hardware/sync.h>
#include <string.h>

#include <pico/multicore.h>

volatile bool settings_loaded;
volatile SnekConfigSaved_t saved_settings;
volatile SnekConfigRuntime_t runtime_settings;

const SnekConfigSaved_t *flash_settings_contents = (const SnekConfigSaved_t *)(XIP_BASE + SETTINGS_QSPI_ADDRESS);

static bool button_was_pressed = false;
static bool mode_change_fired = false;
static uint64_t press_start_us = 0;

void PrintSettings(void)
{
    // TODO: Make a preprocessor that loops through the struct?
    DebugPrintf("\n---SETTINGS START---");
    PRINT_SETTING(saved_settings.current_usb_mode);
    DebugPrintf("---SETTINGS END---\n");
}

void SetDefaults(void)
{
    DebugPrintf("Setting defaults...");

    saved_settings.magic = SETTINGS_MAGIC;

    // TODO: Make this a preprocessor?
    saved_settings.current_usb_mode = DEFAULT_USB_MODE;
}

void Validatesaved_settings(void)
{
    if (saved_settings.current_usb_mode >= USB_MODE_TOTAL_MODES)
    {
        DebugPrintf("saved_settings.current_usb_mode %d over", saved_settings.current_usb_mode);
        saved_settings.current_usb_mode = DEFAULT_USB_MODE;
    }
}

uint8_t calculate_crc(const volatile SnekConfigSaved_t *config)
{
    const uint8_t *start = (const uint8_t *)config;
    const uint8_t *end = (const uint8_t *)&config->crc;

    uint8_t crc = 0;

    while (start < end)
    {
        // additive checksum with rotate left
        crc = (crc << 1) | (crc >> 7);
        crc += *start++;
    }

    return crc;
}

bool LoadSettingsFromQSPI()
{
    bool valid_loaded = false;

    DebugPrintf("LoadSettingsFromQSPI Magic: %08x", flash_settings_contents->magic);

    if (flash_settings_contents->magic == SETTINGS_MAGIC)
    {
        DebugPrintf("Valid settings magic.");

        uint8_t calc_crc = calculate_crc(flash_settings_contents);

        if (flash_settings_contents->crc == calc_crc)
        {
            DebugPrintf("Valid settings loaded %08x, %02x == %02x",
                        flash_settings_contents->magic, flash_settings_contents->crc, calc_crc);

            memcpy(&saved_settings, flash_settings_contents, sizeof(saved_settings));

            valid_loaded = true;
        }
        else
        {
            DebugPrintf("invalid crc %02x != %02x", flash_settings_contents->crc, calc_crc);
        }
    }
    else
    {
        DebugPrintf("Settings, invalid magic %08x != %08x", flash_settings_contents->magic, SETTINGS_MAGIC);
    }

    return valid_loaded;
}

void LoadSettings(void)
{
    bool valid_settings_loaded = false;

    if (settings_loaded)
    {
        return;
    }

    // if we didn't load valid settings, then write them back to flash at the end.
    valid_settings_loaded = LoadSettingsFromQSPI();

    if (!valid_settings_loaded)
    {
        SetDefaults();
        SaveSettings(false);
    }

    Validatesaved_settings();

    if (!gpio_get(PIN_SNEKBOX_DIP2))
    {
        DebugPrintf("RECV");
        runtime_settings.helper_mode = HELPER_MODE_RECV;
    }

    PrintSettings();

    settings_loaded = true;
}

void SaveSettings(bool parkCore1)
{
    // size_t numOfPages = (sizeof(SnekConfigSaved_t) + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    // DebugPrintf("SaveSettings addr:%08x pages:%d -> %d", SETTINGS_QSPI_ADDRESS, numOfPages, numOfPages * FLASH_PAGE_SIZE);

    saved_settings.crc = calculate_crc(&saved_settings);

    uint8_t page[FLASH_PAGE_SIZE];
    memset(page, 0xFF, FLASH_PAGE_SIZE);
    memcpy(page, &saved_settings, sizeof(saved_settings));

    uint32_t ints = save_and_disable_interrupts();

    if (parkCore1)
        multicore_lockout_start_blocking();

    flash_range_erase(SETTINGS_QSPI_ADDRESS, FLASH_SECTOR_SIZE);
    flash_range_program(SETTINGS_QSPI_ADDRESS, page, FLASH_PAGE_SIZE);

    if (parkCore1)
        multicore_lockout_end_blocking();

    restore_interrupts(ints);
}

void ChangeNextMode()
{
    saved_settings.current_usb_mode++;

    if (saved_settings.current_usb_mode >= USB_MODE_TOTAL_MODES)
    {
        // wrap around, reset to the start of the index.
        saved_settings.current_usb_mode = DEFAULT_USB_MODE;
    }

    DebugPrintf("Changing to mode %02x", saved_settings.current_usb_mode);

    SaveSettings(true);
    DebugPrintf("Settings saved %08x", flash_settings_contents->magic);

    rebootDevice();
}

void SettingsTask()
{
    bool change_modes_btn = !gpio_get(PIN_SNEKBOX_BTN1) && !gpio_get(PIN_SNEKBOX_BTN2);
    uint32_t now = time_us_64();

    if (change_modes_btn)
    {
        if (!button_was_pressed)
        {
            button_was_pressed = true;
            mode_change_fired = false;
            press_start_us = now;
        }

        if (!mode_change_fired && (now - press_start_us) >= CHANGE_MODE_US)
        {
            mode_change_fired = true;
            ChangeNextMode();
        }
    }
    else
    {
        button_was_pressed = false;
        mode_change_fired = false;
    }
}