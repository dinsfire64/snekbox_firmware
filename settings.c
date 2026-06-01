#include "settings.h"

volatile bool settings_loaded;
volatile SnekConfig current_settings;

void PrintSettings(void)
{
    // TODO: Make a preprocessor that loops through the struct?
    DebugPrintf("\n---SETTINGS START---");
    PRINT_SETTING(current_settings.current_io_mode);
    PRINT_SETTING(current_settings.current_helper_mode);
    PRINT_SETTING(current_settings.current_usb_mode);
    DebugPrintf("---SETTINGS END---\n");
}

void SetDefaults(void)
{
    DebugPrintf("Setting defaults...");

    // TODO: Make this a preprocessor?
    current_settings.current_io_mode = DEFAULT_IO_MODE;
    current_settings.current_helper_mode = DEFAULT_HELPER_MODE;
    current_settings.current_usb_mode = DEFAULT_USB_MODE;

    SaveSettings();
}

void Validatecurrent_settings(void)
{
    if (current_settings.current_io_mode >= MODE_TOTAL_MODES)
    {
        current_settings.current_io_mode = DEFAULT_IO_MODE;
    }

    if (current_settings.current_helper_mode >= HELPER_MODE_TOTAL_MODES)
    {
        current_settings.current_helper_mode = DEFAULT_HELPER_MODE;
    }

    if (current_settings.current_usb_mode >= USB_MODE_TOTAL_MODES)
    {
        current_settings.current_usb_mode = DEFAULT_USB_MODE;
    }
}

void LoadSettings(void)
{
    if (settings_loaded)
    {
        return;
    }

    SetDefaults();
    Validatecurrent_settings();

    /*
    if (!gpio_get(PIN_SNEKBOX_DIP1))
    {
        DebugPrintf("Helper mode engage");

        if (!gpio_get(PIN_SNEKBOX_DIP2))
        {
            DebugPrintf("SENDER");
            current_settings.current_helper_mode = HELPER_MODE_SENDER;
        }
        else
        {
            DebugPrintf("RECV");
            current_settings.current_helper_mode = HELPER_MODE_RECV;
        }
    }
    */

    if (!gpio_get(PIN_SNEKBOX_DIP2))
    {
        DebugPrintf("RECV");
        current_settings.current_helper_mode = HELPER_MODE_RECV;
    }

    PrintSettings();

    // save settings to ensure they are "valid" as written above.
    SaveSettings();

    settings_loaded = true;
}

void SaveSettings(void)
{
    return;
}