#include "xboxog_descriptors.h"
#include "usb_descriptors.h"

tusb_desc_device_t const xboxog_desc_device =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,

        .bcdUSB = 0x0110,

        .bDeviceClass = 0x00,
        .bDeviceSubClass = 0x00,
        .bDeviceProtocol = 0x00,

        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = VID_XBOX,
        .idProduct = PID_XBOX_CTRL_S,
        .bcdDevice = 0x0120,

        .iManufacturer = STRID_MANUFACTURER,
        .iProduct = STRID_PRODUCT,
        .iSerialNumber = STRID_SERIAL,

        .bNumConfigurations = 1,
};

const xboxog_cfg_desc_t xboxog_desc_fs_configuration = {
    .config = {
        .bLength = sizeof(tusb_desc_configuration_t),
        .bDescriptorType = TUSB_DESC_CONFIGURATION,

        .wTotalLength = sizeof(xboxog_cfg_desc_t),
        .bNumInterfaces = INTERFACE_ID_XBOX_Total,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = 0xc0,
        .bMaxPower = 100 / 2,
    },
    .xboxog_interface = {
        .bLength = sizeof(tusb_desc_interface_t),
        .bDescriptorType = TUSB_DESC_INTERFACE,

        .bInterfaceNumber = INTERFACE_ID_XBOX_Gamepad,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = XID_INTERFACE_CLASS,
        .bInterfaceSubClass = XID_INTERFACE_SUBCLASS,
        .bInterfaceProtocol = 0,
        .iInterface = STRID_XBOX_INTERFACE,
    },
    .xboxog_reportINEndpoint = {
        .bLength = sizeof(tusb_desc_endpoint_t),
        .bDescriptorType = TUSB_DESC_ENDPOINT,

        .bEndpointAddress = XBOX_IN_EPADDR,
        .bmAttributes = {
            .xfer = TUSB_XFER_INTERRUPT,
            .sync = TUSB_ISO_EP_ATT_NO_SYNC,
            .usage = 0x00,
        },
        .wMaxPacketSize = XBOX_EPINSIZE,
        .bInterval = 0x04,
    },
    .xboxog_reportOUTEndpoint = {
        .bLength = sizeof(tusb_desc_endpoint_t),
        .bDescriptorType = TUSB_DESC_ENDPOINT,

        .bEndpointAddress = XBOX_OUT_EPADDR,
        .bmAttributes = {
            .xfer = TUSB_XFER_INTERRUPT,
            .sync = TUSB_ISO_EP_ATT_NO_SYNC,
            .usage = 0x00,
        },
        .wMaxPacketSize = XBOX_EPOUTSIZE,
        .bInterval = 0x04,
    },
};
