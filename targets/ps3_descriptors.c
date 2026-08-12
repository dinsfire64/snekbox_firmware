#include "ps3_descriptors.h"
#include "usb_descriptors.h"
#include "settings.h"

tusb_desc_device_t const ps3_desc_device =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,

        .bcdUSB = 0x110,

        .bDeviceClass = 0,
        .bDeviceSubClass = 0,
        .bDeviceProtocol = 0,

        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = VID_PS3,
        .idProduct = PID_PS3,
        .bcdDevice = 0x120,

        .iManufacturer = STRID_MANUFACTURER,
        .iProduct = STRID_PS3_PRODUCT,
        .iSerialNumber = STRID_SERIAL,

        .bNumConfigurations = 1,
};

const ps3_cfg_desc_t ps3_desc_fs_configuration = {
    .config = {
        .bLength = sizeof(tusb_desc_configuration_t),
        .bDescriptorType = TUSB_DESC_CONFIGURATION,

        .wTotalLength = sizeof(ps3_cfg_desc_t),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = 0xc0,
        .bMaxPower = 100 / 2,
    },
    .ps3_interface = {
        .bLength = sizeof(tusb_desc_interface_t),
        .bDescriptorType = TUSB_DESC_INTERFACE,

        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        .bInterfaceClass = TUSB_CLASS_HID,
        .bInterfaceSubClass = HID_SUBCLASS_NONE,
        .bInterfaceProtocol = HID_ITF_PROTOCOL_NONE,
        .iInterface = STRID_PS3_INTERFACE,
    },
    .ps3_hid_gamepad = {
        .bLength = sizeof(tusb_hid_descriptor_hid_t),
        .bDescriptorType = HID_DESC_TYPE_HID,
        .bcdHID = 0x0110,
        .bCountryCode = 0,
        .bNumDescriptors = 1,
        .bReportType = HID_DESC_TYPE_REPORT,
        .wReportLength = sizeof(PS3_HID_Report),
    },
    .ps3_reportINEndpoint = {
        .bLength = sizeof(tusb_desc_endpoint_t),
        .bDescriptorType = TUSB_DESC_ENDPOINT,

        .bEndpointAddress = PS3_IN_EPADDR,
        .bmAttributes = {
            .xfer = TUSB_XFER_INTERRUPT,
            .sync = TUSB_ISO_EP_ATT_NO_SYNC,
            .usage = 0x00,
        },
        .wMaxPacketSize = PS3_EPINSIZE,
        .bInterval = 0x01,
    },

};
