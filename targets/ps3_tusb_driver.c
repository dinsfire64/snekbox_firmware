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
            // hid get descriptor
        case 0x06:
            tud_control_xfer(rhport, request, &PS3_HID_Report, sizeof(PS3_HID_Report));
            break;

        case 0x01:
            if (request->bmRequestType == 0xA1)
            {
                DebugPrintf("Ps3 Magic!");
                tud_control_xfer(rhport, request, &ps3_magic_bytes, sizeof(ps3_magic_bytes));
            }
            break;

        // get/set idle, not supported.
        case 0x0A:
        case 0x0B:
            return false;
            break;

        default:
            DebugPrintf("ps3d_ctrl req:%02x reqtype:%02x wV:%04x Len:%d",
                        request->bRequest, request->bmRequestType, request->wValue, request->wLength);
            break;
        }

        /*
        switch (request->bRequest)
        {
        case XID_REQ_GET_DESCRIPTOR:
            if (request->bmRequestType == XID_MSFT_VENDREQ && request->wValue == XID_GET_DESCRIPTOR_VALUE)
            {
                tud_control_xfer(rhport, request, &XID_DESCRIPTOR, sizeof(XID_DESCRIPTOR));
            }
            break;

        case HID_REQ_CONTROL_GET_REPORT:
            if (request->bmRequestType == XID_MSFT_VENDREQ)
            {
                if (request->wValue == XID_GETCAP_IN)
                {
                    tud_control_xfer(rhport, request, &XID_CAP_IN, sizeof(XID_CAP_IN));
                }
                else if (request->wValue == XID_GETCAP_OUT)
                {
                    tud_control_xfer(rhport, request, &XID_CAP_OUT, sizeof(XID_CAP_OUT));
                }
            }
            else
            {
                if (request->bmRequestType == 0xA1 && request->wValue == 0x0100)
                {
                    ps3_makeReport();

                    if (request->wLength != sizeof(xbox_controller_state))
                    {
                        DebugPrintf("xbox og %d != %d", request->wLength, sizeof(xbox_controller_state));
                    }

                    tud_control_xfer(rhport, request, &xbox_controller_state, request->wLength);
                }
            }

        case HID_REQ_CONTROL_SET_REPORT:
            if (request->bmRequestType == 0x21 && request->wValue == 0x0200)
            {
                tud_control_xfer(rhport, request, xbox_input_data, request->wLength);
            }
            break;

            break;

        default:
            DebugPrintf("UNKNOWN bRequest: %d", request->bRequest);
            // stall for unimplemented calls is how the original xbox controller worked.
            return false;
            break;
        }
        */
    }
    return true;
}

static bool ps3d_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
    (void)rhport;

    if (ep_addr == PS3_IN_EPADDR && result == XFER_RESULT_SUCCESS)
    {
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
