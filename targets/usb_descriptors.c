#include "tusb.h"
#include "usb_descriptors.h"
#include "xboxog_descriptors.h"
#include "pico/unique_id.h"

#if ENABLE_CDC_DEBUG

#define USB_CDC_VID 0xCafe
#define USB_CDC_PID (0x4000)

tusb_desc_device_t const desc_cdc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,

    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_CDC_VID,
    .idProduct = USB_CDC_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = STRID_MANUFACTURER,
    .iProduct = STRID_DEBUG_PRODUCT,
    .iSerialNumber = STRID_SERIAL,

    .bNumConfigurations = 0x01,
};

enum
{
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_TOTAL
};

#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT 0x02
#define EPNUM_CDC_IN 0x82

#define CONFIG_CDC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)

// full speed configuration
uint8_t const desc_fs_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, STRID_DEBUG_PRODUCT, CONFIG_CDC_TOTAL_LEN, 0x00, 100),

    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, STRID_CDC_INTERFACE, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),

};

#endif

// array of pointer to string descriptors
// matches global_string_id
char const *global_string_array[] = {

    (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "icedragon.io",             // 1: Manufacturer
    "xbox og controller s",     // 2: Product
    NULL,                       // 3: Serials will use unique ID if possible
    "xbox og interface",        // 4: xbox interface

    // debugging strings.
    "snek box debug", // 5: Product
    "snek box cdc",   // 6: CDC Interface
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void)
{
#if ENABLE_CDC_DEBUG
    return (uint8_t const *)&desc_cdc_device;
#else
    return (uint8_t const *)&xboxog_desc_device;
#endif
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index; // for multiple configurations

#if ENABLE_CDC_DEBUG
    return (uint8_t const *)&desc_fs_configuration;
#else
    return (uint8_t const *)&xboxog_desc_fs_configuration;
#endif
}

static uint16_t _desc_str[32 + 1];

static inline size_t board_usb_get_serial_prefix(uint16_t desc_str1[], size_t max_chars, const char *prefix)
{
    size_t prefix_len = 0;
    while (prefix[prefix_len] != '\0')
    {
        prefix_len++;
    }

    if (prefix_len * 2 >= max_chars)
        return 0;

    for (size_t i = 0; i < prefix_len; i++)
    {
        desc_str1[i] = prefix[i];
    }

    uint8_t uid[16] TU_ATTR_ALIGNED(4);
    size_t uid_len;

    pico_unique_board_id_t pico_id;
    pico_get_unique_board_id(&pico_id);

    size_t len = PICO_UNIQUE_BOARD_ID_SIZE_BYTES;
    if (len > sizeof(uid))
        len = sizeof(uid);

    memcpy(uid, pico_id.id, len);
    uid_len = len;

    size_t remaining_chars = max_chars - prefix_len;
    if (uid_len > remaining_chars / 2)
        uid_len = remaining_chars / 2;

    for (size_t i = 0; i < uid_len; i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            const char nibble_to_hex[16] = {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
            uint8_t const nibble = (uid[i] >> (j * 4)) & 0xf;
            desc_str1[prefix_len + i * 2 + (1 - j)] = nibble_to_hex[nibble];
        }
    }

    return 2 * uid_len + prefix_len;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    size_t chr_count;

    switch (index)
    {
    case STRID_LANGID:
        memcpy(&_desc_str[1], global_string_array[0], 2);
        chr_count = 1;
        break;

    case STRID_SERIAL:
        const char *prefix = "snek-";
        chr_count = board_usb_get_serial_prefix(_desc_str + 1, 32, prefix);
        break;

    default:
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

        if (!(index < sizeof(global_string_array) / sizeof(global_string_array[0])))
            return NULL;

        const char *str = global_string_array[index];

        // Cap at max char
        chr_count = strlen(str);
        size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
        if (chr_count > max_count)
            chr_count = max_count;

        // Convert ASCII string into UTF-16
        for (size_t i = 0; i < chr_count; i++)
        {
            _desc_str[1 + i] = str[i];
        }
        break;
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

    return _desc_str;
}
