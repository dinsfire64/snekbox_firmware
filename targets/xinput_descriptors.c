#include "xinput_descriptors.h"
#include "usb_descriptors.h"

tusb_desc_device_t const xinput_desc_device =
    {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,

        .bcdUSB = 0x0200,

        .bDeviceClass = TUSB_CLASS_VENDOR_SPECIFIC,
        .bDeviceSubClass = TUSB_CLASS_VENDOR_SPECIFIC,
        .bDeviceProtocol = TUSB_CLASS_VENDOR_SPECIFIC,

        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

        .idVendor = VID_XINPUT,
        .idProduct = PID_XINPUT,
        .bcdDevice = 0x0214,

        .iManufacturer = STRID_MANUFACTURER,
        .iProduct = STRID_XINPUT_PRODUCT,
        .iSerialNumber = STRID_SERIAL,

        .bNumConfigurations = 1,
};

uint8_t const xinput_desc_fs_configuration[] = {

    0x09, // bLength
    0x02, // bDescriptorType (Configuration)
    0x99,
    0x00, // wTotalLength 0x99
    0x04, // bNumInterfaces 4
    0x01, // bConfigurationValue
    0x00, // iConfiguration (String Index)
    0xA0, // bmAttributes (remote wakeup)
    0xFA, // bMaxPower 500mA

    // Control Interface (0x5D 0xFF)
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x00, // bInterfaceNumber 0
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints 2
    0xFF, // bInterfaceClass
    0x5D, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    // Gamepad Descriptor
    0x11, // bLength
    0x21, // bDescriptorType (HID)
    0x00,
    0x01, // bcdHID 1.10
    0x01, // SUB_TYPE
    0x25, // reserved2
    0x81, // DEVICE_EPADDR_IN
    0x14, // bMaxDataSizeIn
    0x00,
    0x00,
    0x00,
    0x00,
    0x13, // reserved3
    0x02, // DEVICE_EPADDR_OUT is this right?
    0x08, // bMaxDataSizeOut
    0x00,
    0x00, // reserved4

    // Report IN Endpoint 1.1
    0x07, // bLength
    0x05, // bDescriptorType (Endpoint)
    0x81, // bEndpointAddress (IN/D2H)
    0x03, // bmAttributes (Interrupt)
    0x20,
    0x00, // wMaxPacketSize 32
    0x01, // bInterval 1 (unit depends on device speed)

    // Report OUT Endpoint 1.2
    0x07, // bLength
    0x05, // bDescriptorType (Endpoint)
    0x02, // bEndpointAddress (OUT/H2D)
    0x03, // bmAttributes (Interrupt)
    0x20,
    0x00, // wMaxPacketSize 32
    0x08, // bInterval 8 (unit depends on device speed)

    // Interface Audio
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x01, // bInterfaceNumber 1
    0x00, // bAlternateSetting
    0x04, // bNumEndpoints 4
    0xFF, // bInterfaceClass
    0x5D, // bInterfaceSubClass
    0x03, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    // Audio Descriptor
    0x1B, // bLength
    0x21,
    0x00,
    0x01,
    0x01,
    0x01,
    0x83, // XINPUT_MIC_IN
    0x40, // ??
    0x01, // ??
    0x04, // XINPUT_AUDIO_OUT
    0x20, // ??
    0x16, // ??
    0x85, // XINPUT_UNK_IN
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x16,
    0x06, // XINPUT_UNK_OUT
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,

    // Report IN Endpoint 2.1
    0x07, // bLength
    0x05, // bDescriptorType (Endpoint)
    0x83, // bEndpointAddress (XINPUT_MIC_IN)
    0x03, // bmAttributes (Interrupt)
    0x20,
    0x00, // wMaxPacketSize 32
    0x02, // bInterval 2 (unit depends on device speed)

    // Report OUT Endpoint 2.2
    0x07, // bLength
    0x05, // bDescriptorType (Endpoint)
    0x04, // bEndpointAddress (XINPUT_AUDIO_OUT)
    0x03, // bmAttributes (Interrupt)
    0x20,
    0x00, // wMaxPacketSize 32
    0x04, // bInterval 4 (unit depends on device speed)

    // Report IN Endpoint 2.3
    0x07, // bLength
    0x05, // bDescriptorType (Endpoint)
    0x85, // bEndpointAddress (XINPUT_UNK_IN)
    0x03, // bmAttributes (Interrupt)
    0x20,
    0x00, // wMaxPacketSize 32
    0x40, // bInterval 128

    // Report OUT Endpoint 2.4
    0x07, // bLength
    0x05, // bDescriptorType (Endpoint)
    0x06, // bEndpointAddress (XINPUT_UNK_OUT)
    0x03, // bmAttributes (Interrupt)
    0x20,
    0x00, // wMaxPacketSize 32
    0x10, // bInterval 16

    // Interface Plugin Module
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x02, // bInterfaceNumber 2
    0x00, // bAlternateSetting
    0x01, // bNumEndpoints 1
    0xFF, // bInterfaceClass
    0x5D, // bInterfaceSubClass
    0x02, // bInterfaceProtocol
    0x00, // iInterface (String Index)

    // PluginModuleDescriptor : {
    0x09, // bLength
    0x21, // bDescriptorType
    0x00,
    0x01, // version 1.00
    0x01, // ??
    0x22, // ??
    0x86, // XINPUT_PLUGIN_MODULE_IN,
    0x03, // ??
    0x00, // ??

    // Report IN Endpoint 3.1
    0x07, // bLength
    0x05, // bDescriptorType (Endpoint)
    0x86, // bEndpointAddress (XINPUT_PLUGIN_MODULE_IN)
    0x03, // bmAttributes (Interrupt)
    0x20,
    0x00, // wMaxPacketSize 32
    0x10, // bInterval 8 (unit depends on device speed)

    // Interface Security
    0x09, // bLength
    0x04, // bDescriptorType (Interface)
    0x03, // bInterfaceNumber 3
    0x00, // bAlternateSetting
    0x00, // bNumEndpoints 0
    0xFF, // bInterfaceClass
    0xFD, // bInterfaceSubClass
    0x13, // bInterfaceProtocol
    0x04, // iInterface (String Index)

    // SecurityDescriptor (XSM3)
    0x06, // bLength
    0x41, // bDescriptType (Xbox 360)
    0x00,
    0x01,
    0x01,
    0x03,
};