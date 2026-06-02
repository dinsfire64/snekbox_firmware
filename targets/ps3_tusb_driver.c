#include "debug.h"
#include "tusb.h"

#include "device/usbd_pvt.h"

#include "ps3_descriptors.h"
#include "ps3_tusb_driver.h"

#include "ps3.h"

static uint8_t itf_num;

static void ps3d_init(void)
{
    DebugPrintf("ps3d_init");

    ps3_setup();
}

static void ps3d_reset(uint8_t __unused rhport)
{
    DebugPrintf("ps3d_reset");
    itf_num = 0;
}

static uint16_t ps3d_open(uint8_t rhport,
                          tusb_desc_interface_t const *itf_desc,
                          uint16_t max_len)
{
    TU_VERIFY(itf_desc->bInterfaceClass == TUSB_CLASS_HID, 0);

    uint16_t const drv_len =
        sizeof(tusb_desc_interface_t) +
        sizeof(tusb_hid_descriptor_hid_t) +
        itf_desc->bNumEndpoints * sizeof(tusb_desc_endpoint_t);

    TU_VERIFY(max_len >= drv_len, 0);

    itf_num = itf_desc->bInterfaceNumber;

    uint8_t const *p_desc = (uint8_t const *)itf_desc;

    // Move past interface → HID descriptor
    p_desc = tu_desc_next(p_desc);
    TU_VERIFY(HID_DESC_TYPE_HID == tu_desc_type(p_desc));

    // Move past HID descriptor → endpoint(s)
    p_desc = tu_desc_next(p_desc);

    TU_ASSERT(usbd_edpt_open(rhport, (tusb_desc_endpoint_t const *)p_desc));

    return drv_len;
}

bool ps3d_control_request_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request)
{
    if (request->wIndex == itf_num && stage == CONTROL_STAGE_SETUP)
    {
        switch (request->bRequest)
        {
        case 0x06:
            // hid get descriptor
            tud_control_xfer(rhport, request, &PS3_HID_Report, sizeof(PS3_HID_Report));
            break;

        case 0x01:
            if (request->bmRequestType == 0xA1)
            {
                // required for the console to allow us to press the playstation button
                DebugPrintf("PS3 Magic!");
                tud_control_xfer(rhport, request, &ps3_magic_bytes, sizeof(ps3_magic_bytes));
            }
            break;

        case 0x0A:
        case 0x0B:
            // get/set idle, not supported.
            return false;
            break;

        default:
            // unknown control transfer request.
            DebugPrintf("ps3d_ctrl req:%02x reqtype:%02x wV:%04x Len:%d",
                        request->bRequest, request->bmRequestType, request->wValue, request->wLength);
            break;
        }
    }

    return true;
}

static bool ps3d_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
    (void)rhport;

    if (ep_addr == PS3_IN_EPADDR && result == XFER_RESULT_SUCCESS)
    {
        // we do not have an endpoint for the rumble right now.
        // ps3_process_output(xferred_bytes);
    }

    return true;
}

usbd_class_driver_t const _ps3d_driver =
    {
#if CFG_TUSB_DEBUG >= 2
        .name = "PS3",
#endif
        .init = ps3d_init,
        .reset = ps3d_reset,
        .open = ps3d_open,
        .control_xfer_cb = ps3d_control_request_cb,
        .xfer_cb = ps3d_xfer_cb,
        .sof = NULL,
};
