#include "switch_descriptors.h"
#include "usb_descriptors.h"

uint8_t const desc_hid_report_gamepad[] =
    {
        // clang-format off
        
        0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
        0x09, 0x05,        // Usage (Game Pad)
        0xA1, 0x01,        // Collection (Application)
        0x15, 0x00,        //   Logical Minimum (0)
        0x25, 0x01,        //   Logical Maximum (1)
        0x35, 0x00,        //   Physical Minimum (0)
        0x45, 0x01,        //   Physical Maximum (1)
        0x75, 0x01,        //   Report Size (1)
        0x95, 0x0E,        //   Report Count (14)
        0x05, 0x09,        //   Usage Page (Button)
        0x19, 0x01,        //   Usage Minimum (0x01)
        0x29, 0x0E,        //   Usage Maximum (0x0E)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x95, 0x02,        //   Report Count (2)
        0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
        0x25, 0x07,        //   Logical Maximum (7)
        0x46, 0x3B, 0x01,  //   Physical Maximum (315)
        0x75, 0x04,        //   Report Size (4)
        0x95, 0x01,        //   Report Count (1)
        0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
        0x09, 0x39,        //   Usage (Hat switch)
        0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
        0x65, 0x00,        //   Unit (None)
        0x95, 0x01,        //   Report Count (1)
        0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x26, 0xFF, 0x00,  //   Logical Maximum (255)
        0x46, 0xFF, 0x00,  //   Physical Maximum (255)
        0x09, 0x30,        //   Usage (X)
        0x09, 0x31,        //   Usage (Y)
        0x09, 0x32,        //   Usage (Z)
        0x09, 0x35,        //   Usage (Rz)
        0x75, 0x08,        //   Report Size (8)
        0x95, 0x04,        //   Report Count (4)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x75, 0x08,        //   Report Size (8)
        0x95, 0x01,        //   Report Count (1)
        0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,              // End Collection

        // clang-format on
};

tusb_desc_device_t const switch_desc_device =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = 0x00,
        .bDeviceSubClass = 0x00,
        .bDeviceProtocol = 0x00,
        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = SWITCH_VID,
        .idProduct = SWITCH_PID,
        .bcdDevice = 0x0100,

        .iManufacturer = STRID_MANUFACTURER,
        .iProduct = STRID_SWITCH,
        .iSerialNumber = 0,

        .bNumConfigurations = 0x01,
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + \
                          (TUD_HID_INOUT_DESC_LEN * 1))

#define ITF_NUM_TOTAL 1

const uint8_t switch_desc_configuration[] =
    {
        // Config number, interface count, string index, total length, attribute, power in mA
        TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
        // Interface number, string index, protocol, report descriptor len, EP OUT & IN address, size & polling interval
        TUD_HID_INOUT_DESCRIPTOR(HID_GAMEPAD_ITF_NUM, STRID_SWITCH_INTERFACE, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report_gamepad), HID_GAMEPAD_EPADDR, HID_GAMEPAD_EPADDR2, CFG_TUD_HID_EP_BUFSIZE, 1),
};

usbd_class_driver_t _switch_hid_driver =
    {
#if CFG_TUSB_DEBUG >= 2
        .name = "switch_hid",
#endif
        .init = hidd_init,
        .reset = hidd_reset,
        .open = hidd_open,
        .control_xfer_cb = hidd_control_xfer_cb,
        .xfer_cb = hidd_xfer_cb,
        .sof = NULL,
};