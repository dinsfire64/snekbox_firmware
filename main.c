#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"
#include "hardware/watchdog.h"

#include "pio_usb.h"
#include "tusb.h"

#include "debug.h"
#include "settings.h"
#include "ws2812.h"
#include "i2c.h"

#include "helpers.h"
#include "common_types.h"
#include "input_report.h"

#include "targets/__targets.h"
#include "targets/ps2_phy.h"
#include "targets/joybus.h"
#include "targets/xboxog.h"

#include "handlers/__handlers.h"
#include "handlers/ds3.h"
#include "handlers/ds4.h"
#include "handlers/ds5.h"
#include "handlers/smx.h"
#include "handlers/xinput_handler.h"
#include "handlers/keyboard.h"
#include "handlers/mouse.h"
#include "handlers/icedragon.h"
#include "handlers/switch_pro.h"
#include "handlers/ltek.h"
#include "handlers/dforce.h"
#include "handlers/gamo.h"
#include "handlers/generic_softmat.h"
#include "handlers/dual_ps2.h"
#include "handlers/zuiki.h"
#include "handlers/santroller.h"
#include "handlers/b2l.h"
#include "handlers/ddr_grandprix.h"
#include "handlers/arcin.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

#define WATCHDOG_TIMEOUT_STARTUP_MS 2000
#define WATCHDOG_TIMEOUT_NORMAL_MS 1000

uint8_t prev_report[CFG_TUH_HID][CFG_TUH_HID_EPIN_BUFSIZE * 2];
usb_host_dev_info_t current_device = {0x00};
output_report_t output_report = {0x00};

uint64_t prev_time = 0;
uint16_t temp_buf_16[256] = {0};

void init_local_state()
{
  current_device.mounted = false;

  memset(current_device.product_str, 0x00, sizeof(current_device.product_str));
  memset(temp_buf_16, 0x00, TU_ARRAY_SIZE(temp_buf_16));

  input_report.short_report.controller_type = SPECIAL_CONTROLLER_NONE;

  // TODO: Implement a quirks/reset system.
  is_switch_pro_init1 = false;
  is_switch_pro_fully_init = false;

  is_ds3_init = false;

  reset_report();
}

/*------------- MAIN -------------*/

// core1: handle host events
void core1_main()
{
  sleep_ms(10);

  // skip boot reports from devices.
  tuh_hid_set_default_protocol(HID_PROTOCOL_REPORT);

  // Use tuh_configure() to pass pio configuration to the host stack
  // Note: tuh_configure() must be called before
  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = PIN_SNEKBOX_USB_DP;
  pio_cfg.pinout = SNEKBOX_USB_PIO_PINOUT;
  tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);

  if (current_settings.current_helper_mode == HELPER_MODE_NONE ||
      current_settings.current_helper_mode == HELPER_MODE_SENDER)
  {
    // To run USB SOF interrupt in core1, init host stack for pio_usb (roothub
    // port1) on core1
    tuh_init(1);
  }
  else if (current_settings.current_helper_mode == HELPER_MODE_RECV)
  {
    i2c_setup();
  }

  while (true)
  {
    // the recv does not need to host a usb device, just accept i2c
    if (current_settings.current_helper_mode == HELPER_MODE_NONE ||
        current_settings.current_helper_mode == HELPER_MODE_SENDER)
    {
      // tinyusb host task
      tuh_task();
    }

    i2c_task();
  }
}

void setup_hardware()
{
  init_ws2812();

  gpio_init(PIN_SNEKBOX_5V_EN);
  gpio_set_dir(PIN_SNEKBOX_5V_EN, GPIO_OUT);

  // turning off the power on certain devices causes an inrush of current when it wakes back up
  // just keep the power flowing forever...
  // gpio_put(PIN_SNEKBOX_5V_EN, 0);
  // sleep_ms(250);
  gpio_put(PIN_SNEKBOX_5V_EN, 1);

  gpio_init(PIN_SNEKBOX_LED);
  gpio_set_dir(PIN_SNEKBOX_LED, GPIO_OUT);
  gpio_put(PIN_SNEKBOX_LED, 1);

  gpio_init(PIN_SNEKBOX_BTN1);
  gpio_set_dir(PIN_SNEKBOX_BTN1, GPIO_IN);
  gpio_pull_up(PIN_SNEKBOX_BTN1);

  gpio_init(PIN_SNEKBOX_BTN2);
  gpio_set_dir(PIN_SNEKBOX_BTN2, GPIO_IN);
  gpio_pull_up(PIN_SNEKBOX_BTN2);

  gpio_init(PIN_SNEKBOX_BTN3);
  gpio_set_dir(PIN_SNEKBOX_BTN3, GPIO_IN);
  gpio_pull_up(PIN_SNEKBOX_BTN3);

  gpio_init(PIN_SNEKBOX_BTN4);
  gpio_set_dir(PIN_SNEKBOX_BTN4, GPIO_IN);
  gpio_pull_up(PIN_SNEKBOX_BTN4);

  gpio_init(PIN_SNEKBOX_DIP1);
  gpio_set_dir(PIN_SNEKBOX_DIP1, GPIO_IN);
  gpio_pull_up(PIN_SNEKBOX_DIP1);

  gpio_init(PIN_SNEKBOX_DIP2);
  gpio_set_dir(PIN_SNEKBOX_DIP2, GPIO_IN);
  gpio_pull_up(PIN_SNEKBOX_DIP2);
}

// core0: handle device events
int main(void)
{
  // default 125MHz is not appropriate. Sysclock should be multiple of 12MHz for USB PIO.
  set_sys_clock_khz(120000, true);

  DebugSetup();

  watchdog_enable(WATCHDOG_TIMEOUT_STARTUP_MS, true);

  setup_hardware();

// flash the RGB light in the event debug is on to throw a visual indicator.
#if (ENABLE_DEBUG_UART || ENABLE_BUFFER_DUMP)
  for (int i = 0; i < 5; i++)
  {
    set_rgb0(0xFF, 0xFF, 0xFF);
    sleep_us(50 * 1000);
    set_rgb0(0, 0, 0);
    sleep_us(50 * 1000);
  }
#endif

#if DEBUG_MIRROR_HELPER
  en_helper_report = true;
#endif

  LoadSettings();

  init_local_state();
  targets_setup();

  tud_init(0);

  // all USB task run in core1
  multicore_reset_core1();
  multicore_launch_core1(core1_main);

  sleep_ms(100);

  watchdog_enable(WATCHDOG_TIMEOUT_NORMAL_MS, true);

  while (true)
  {
    targets_task();

    tud_task();
    xboxog_task();

    handlers_task();

    // the recv refreshing watchdog in their ISR.
    if (current_settings.current_helper_mode != HELPER_MODE_RECV)
    {
      watchdog_update();
    }
  }

  return 0;
}

#define LANGUAGE_ID_ENG 0x0409

static void _convert_utf16le_to_utf8(const uint16_t *utf16, size_t utf16_len, uint8_t *utf8, size_t utf8_len)
{
  // TODO: Check for runover.
  (void)utf8_len;
  // Get the UTF-16 length out of the data itself.

  size_t uint8_t_index = 0;

  for (size_t i = 0; i < utf16_len; i++)
  {
    if (uint8_t_index > utf8_len)
    {
      break;
    }

    uint16_t chr = utf16[i];
    if (chr < 0x80)
    {
      utf8[uint8_t_index++] = chr & 0xffu;
    }
    else if (chr < 0x800)
    {
      utf8[uint8_t_index++] = (uint8_t)(0xC0 | (chr >> 6 & 0x1F));
      utf8[uint8_t_index++] = (uint8_t)(0x80 | (chr >> 0 & 0x3F));
    }
    else
    {
      // TODO: Verify surrogate.
      utf8[uint8_t_index++] = (uint8_t)(0xE0 | (chr >> 12 & 0x0F));
      utf8[uint8_t_index++] = (uint8_t)(0x80 | (chr >> 6 & 0x3F));
      utf8[uint8_t_index++] = (uint8_t)(0x80 | (chr >> 0 & 0x3F));
    }
    // TODO: Handle UTF-16 code points that take two entries.
  }
}

static int _count_utf8_bytes(const uint16_t *buf, size_t len)
{
  size_t total_bytes = 0;
  for (size_t i = 0; i < len; i++)
  {
    uint16_t chr = buf[i];
    if (chr < 0x80)
    {
      total_bytes += 1;
    }
    else if (chr < 0x800)
    {
      total_bytes += 2;
    }
    else
    {
      total_bytes += 3;
    }
    // TODO: Handle UTF-16 code points that take two entries.
  }
  return (int)total_bytes;
}

static void convert_utf16_to_utf8_str(uint16_t *temp_buf, size_t buf_len)
{
  memset(current_device.product_str, 0x00, sizeof(current_device.product_str));

  size_t utf16_len = ((temp_buf[0] & 0xff) - 2) / sizeof(uint16_t);
  size_t utf8_len = (size_t)_count_utf8_bytes(temp_buf + 1, utf16_len);

  _convert_utf16le_to_utf8(temp_buf + 1, utf16_len, (uint8_t *)temp_buf, sizeof(uint16_t) * buf_len);

  ((uint8_t *)temp_buf)[utf8_len] = '\0';

  memcpy(current_device.product_str, &((uint8_t *)temp_buf)[0], utf8_len);

  DebugPrintf("found string %s", current_device.product_str);
}

// USB Host

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

void tuh_mount_cb(uint8_t dev_addr)
{
  // uint16_t vid, pid;
  // tuh_vid_pid_get(dev_addr, &vid, &pid);

  // DebugPrintf("tuh_mount_cb New USB Device %04x:%04x", vid, pid);

  // some devices need a product string comparison to compare, so call this before checking the vid/pid combos.
}

void tuh_umount_cb(uint8_t dev_addr)
{
  (void)dev_addr;

  DebugPrintf("tuh_umount_cb device disconnected %d", dev_addr);

  set_rgb1(0, 0, 0);
}

//--------------------------------------------------------------------+
// Host HID
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len)
{
  (void)desc_report;
  (void)desc_len;

  if (desc_len == 0 || desc_report == NULL)
  {
    DebugPrintf("Invalid report");
    return;
  }

  // Interface protocol (hid_interface_protocol_enum_t)
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  DebugPrintf("[%04x:%04x][%u] HID Interface%u, Protocol = %d", vid, pid, dev_addr, instance, itf_protocol);

  if (itf_protocol == HID_ITF_PROTOCOL_MOUSE)
  {
    DebugPrintf("Ignoring mice.");
    return;
  }

  // print the HID to debug/parse later.
  DebugOutputBuffer("DESC", desc_report, desc_len);

  if (tuh_descriptor_get_product_string_sync(dev_addr, LANGUAGE_ID_ENG, temp_buf_16, TU_ARRAY_SIZE(temp_buf_16)) == XFER_RESULT_SUCCESS)
  {
    convert_utf16_to_utf8_str(temp_buf_16, TU_ARRAY_SIZE(temp_buf_16));
  }
  else
  {
    DebugPrintf("Could not get string descriptor");
  }

  // Receive report from boot keyboard & mouse only
  // tuh_hid_report_received_cb() will be invoked when report is available
  if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD ||
      itf_protocol == HID_ITF_PROTOCOL_MOUSE ||
      determine_handler(dev_addr) != HANDLER_NONE)
  {
    current_device.mounted = true;
    current_device.dev_addr = dev_addr;

    // TODO: Refactor this for multiple endpoints on composite devices.
    current_device.instance = instance;

    DebugPrintf("Setting up hid for %d:%d", dev_addr, instance);

    // dualshock 3 need some magic bytes to init the controller.
    if (is_DS3(dev_addr))
    {
      init_ds3(dev_addr, instance);
    }

    // only enable helper mode for known devices
    if (is_SNEK(dev_addr) || itf_protocol == HID_ITF_PROTOCOL_KEYBOARD)
    {
      DebugPrintf("enabling helper over i2c...");
      en_helper_report = true;
    }

    if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD)
    {
      set_key_mapping(dev_addr);
    }

    // kick off receiving another hid report.
    if (tuh_hid_receive_report(dev_addr, instance))
    {
      DebugPrintf("Device %d:%d Mounted.", dev_addr, instance);
      set_rgb1(0, 0xFF, 0);
    }
    else
    {
      DebugPrintf("Error: cannot request report");
    }
  }
  else
  {
    DebugPrintf("Unknown device %04x:%04x", vid, pid);
    set_rgb1(0xFF, 0, 0);
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  // reset the local state on a disconnect.
  init_local_state();

  DebugPrintf("[%u] HID Interface%u is unmounted", dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len)
{
  (void)len;
  bool newReport = false;

  // set_rgb1(0x00, counter++, 0x00);

  // NOTE: in pio_usb.c, pio_usb_ll_configure_endpoint, ep->interval = 1
  //  will override the polling decision of the slave device.

#if PRINT_TIMING
  static uint64_t prev_time = 0;
  uint64_t current_time = to_us_since_boot(get_absolute_time());
  DebugPrintf("%0.2f ms", (current_time - prev_time) / 1000.0);
  prev_time = current_time;
#endif

  if (len > 0)
  {
    if (memcmp(report, prev_report[instance], len))
    {
      // DebugPrintf("%d:%d", dev_addr, instance);

#if ENABLE_REPORT_DUMP
      DebugOutputBuffer("RPT:", report, len);
#endif

      newReport = true;

      memcpy(prev_report[instance], report, len);
    }

    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

    switch (itf_protocol)
    {
    case HID_ITF_PROTOCOL_KEYBOARD:
      if (newReport)
      {
        process_kbd_report(dev_addr, (hid_keyboard_report_t const *)report, len);
      }
      break;

    case HID_ITF_PROTOCOL_MOUSE:
      if (newReport)
      {
        process_mouse_report(dev_addr, (hid_mouse_report_t const *)report);
      }
      break;

    default:

      if (is_SWITCH_PRO(dev_addr) && !is_switch_pro_fully_init)
      {
        init_switch_pro(dev_addr, instance);
      }

      if (newReport)
      {
        switch (determine_handler(dev_addr))
        {
          DISPATCH_NEW_REPORT(DFORCE)
          DISPATCH_NEW_REPORT(DS3)
          DISPATCH_NEW_REPORT(DS4)
          DISPATCH_NEW_REPORT(DS5)
          DISPATCH_NEW_REPORT(SMX)
          DISPATCH_NEW_REPORT(SNEK)
          DISPATCH_NEW_REPORT(STAC)
          DISPATCH_NEW_REPORT(STAC2)
          DISPATCH_NEW_REPORT(SWITCH_PRO)
          DISPATCH_NEW_REPORT(LTEK)
          DISPATCH_NEW_REPORT(PHOENIXWAN)
          DISPATCH_NEW_REPORT(SOFTMAT)
          DISPATCH_NEW_REPORT(DUAL_PS2)
          DISPATCH_NEW_REPORT(ZUIKI)
          DISPATCH_NEW_REPORT(FUSION_HID)
          DISPATCH_NEW_REPORT(SANTROLLER)
          DISPATCH_NEW_REPORT(B2L)
          DISPATCH_NEW_REPORT(B2LV2)
          DISPATCH_NEW_REPORT(DDR_GRANDPRIX)
          DISPATCH_NEW_REPORT(ARCIN)
        default:
          DebugPrintf("Unknown handler type for dev %d:%d", dev_addr, instance);
          DebugOutputBuffer("RPT:", report, len);
          set_rgb1(0, 0, 0);
          break;
        }
      }
      break;
    }
  }
  else
  {
    DebugPrintf("Stalled report on %d:%d", dev_addr, instance);

    // on stall, just retry.
  }

#if ENABLE_REPORT_DUMP
  if (newReport)
  {
    DebugPrintf("%s %s %s %s",
                input_report.short_report.dpad_up ? "U" : ".",
                input_report.short_report.dpad_down ? "D" : ".",
                input_report.short_report.dpad_left ? "L" : ".",
                input_report.short_report.dpad_right ? "R" : ".");
  }
#endif

  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance))
  {
    DebugPrintf("Error: cannot request report");
  }
}
