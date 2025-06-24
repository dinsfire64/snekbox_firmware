#include "keyboard.h"

static hid_keyboard_report_t prev_report = {0, 0, {0}}; // previous report to check key released
uint8_t repeat_keycode[MAX_KEYBOARD_REPORT_SIZE] = {0};

keyboard_input_report_t keyboard_report = {0};

keyboard_mapping_t key_mappings_standard[] = {
    {HID_KEY_ARROW_UP, &keyboard_report.dpad_up},
    {HID_KEY_ARROW_DOWN, &keyboard_report.dpad_down},
    {HID_KEY_ARROW_LEFT, &keyboard_report.dpad_left},
    {HID_KEY_ARROW_RIGHT, &keyboard_report.dpad_right},

    {HID_KEY_I, &keyboard_report.btn_north},
    {HID_KEY_K, &keyboard_report.btn_south},
    {HID_KEY_L, &keyboard_report.btn_east},
    {HID_KEY_J, &keyboard_report.btn_west},

    {HID_KEY_SPACE, &keyboard_report.guide},

    {HID_KEY_ENTER, &keyboard_report.start},
    {HID_KEY_BACKSPACE, &keyboard_report.select},
};

keyboard_mapping_t *current_mapping = key_mappings_standard;
size_t current_mapping_size = sizeof(key_mappings_standard) / sizeof(keyboard_mapping_t);

void switch_mapping(keyboard_mapping_t *new_mapping, size_t new_mapping_size)
{
    current_mapping = new_mapping;
    current_mapping_size = new_mapping_size;
}

void set_key_mapping(uint8_t dev_addr)
{
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    // TODO: check pid/vid etc, set accordingly

    DebugPrintf("Setting standard keyboard mapping.");
    switch_mapping(key_mappings_standard, sizeof(key_mappings_standard) / sizeof(keyboard_mapping_t));
}

void processKeyEvent(uint8_t keycode, bool pressed)
{
    for (uint16_t i = 0; i < current_mapping_size; i++)
    {
        if (current_mapping[i].keycode == keycode)
        {
            *(current_mapping[i].button) = pressed;
            // NOTE: do not break as multiple key events can be assigned to the same button.
        }
    }
}

void processModifierEvent(uint8_t modifier)
{
    (void)modifier;

    // for debugging and testing with helpers on the bench.
    helper_short_report.start = (modifier & (KEYBOARD_MODIFIER_LEFTCTRL));
    helper_short_report.btn_south = (modifier & (KEYBOARD_MODIFIER_LEFTALT));
    helper_short_report.btn_east = (modifier & (KEYBOARD_MODIFIER_LEFTSHIFT));
}

void convert_keyboard_report()
{
    input_report.short_report.dpad_up = keyboard_report.dpad_up;
    input_report.short_report.dpad_down = keyboard_report.dpad_down;
    input_report.short_report.dpad_left = keyboard_report.dpad_left;
    input_report.short_report.dpad_right = keyboard_report.dpad_right;

    input_report.short_report.btn_north = keyboard_report.btn_north;
    input_report.short_report.btn_south = keyboard_report.btn_south;
    input_report.short_report.btn_east = keyboard_report.btn_east;
    input_report.short_report.btn_west = keyboard_report.btn_west;

    input_report.short_report.guide = keyboard_report.guide;

    input_report.short_report.start = keyboard_report.start;
    input_report.short_report.select = keyboard_report.select;
}

// look up new key in previous keys
static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode, uint16_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        if (report->keycode[i] == keycode)
            return true;
    }

    return false;
}

// release logic from https://github.com/dquadros/RPTerm/blob/main/keybd.cpp
// press logic from tinyusb host example

// convert hid keycode to ascii and print via usb device CDC (ignore non-printable)
void process_kbd_report(uint8_t dev_addr, hid_keyboard_report_t const *report, uint16_t len)
{
    (void)dev_addr;

    // TODO: figure out if we need to reset since every report coming in
    // might not contain the full state of the keyboard.
    // reset_report();

    // DebugOutputBuffer("KEY", report->keycode, len);

    // handle the modifier presses
    if (prev_report.modifier != report->modifier)
    {
        DebugPrintf("MOD CHANGE: %02x", report->modifier);

        processModifierEvent(report->modifier);
    }

    // check for key release
    for (uint8_t i = 0; i < len; i++)
    {
        if (repeat_keycode[i] && !find_key_in_report(report, repeat_keycode[i], len))
        {
            // DebugPrintf("Key released %02x", repeat_keycode[i]);
            processKeyEvent(repeat_keycode[i], false);

            repeat_keycode[i] = 0;
        }
    }

    for (uint8_t i = 0; i < len; i++)
    {
        uint8_t keycode = report->keycode[i];

        if (keycode)
        {
            if (find_key_in_report(&prev_report, keycode, len))
            {
                // exist in previous report means the current key is holding
                // DebugPrintf("%02x held!", keycode);
            }
            else
            {
                // not existed in previous report means the current key is pressed
                // DebugPrintf("Key pressed %02x", keycode);

                processKeyEvent(keycode, true);

                // bool const is_shift = report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);

                // save the key as being held as it was just pressed
                for (int j = 0; j < len; j++)
                {
                    if (repeat_keycode[j] == 0)
                    {
                        repeat_keycode[j] = keycode;
                        break;
                    }
                }
            }
        }
    }

    prev_report = *report;

    // convert the local report to the main report.
    convert_keyboard_report();
}