#include "debug.h"
#include "tusb.h"

#include "device/usbd_pvt.h"

#include "xboxog.h"
#include "xboxog_descriptors.h"

static uint8_t itf_num;

const XIDDescriptor XID_DESCRIPTOR =
    {
        .bLength = sizeof(XIDDescriptor),
        .bDescriptorType = 0x42, // static value
        .bcdXid = 0x0100,        // 1.0 byte swapped
        .bType = 0x01,           // Xbox controller
        .bSubType = 0x02,        // Xbox Type S
        .bMaxInputReportSize = XID_REPORTSIZE_IN,
        .bMaxOutputReportSize = XID_REPORTSIZE_OUT,
};

const XIDGamepadCapabilitiesIn XID_CAP_IN =
    {
        .bReportId = 0x00,
        .bLength = sizeof(XIDGamepadCapabilitiesIn),
        .data = {
            [0 ... 17] = 0xFF, // 1 is "enable" this button.
            [1] = 0x00,        // Original controller reserves this byte.
        },
};

const XIDGamepadCapabilitiesOut XID_CAP_OUT =
    {
        .bReportId = 0x00,
        .bLength = sizeof(XIDGamepadCapabilitiesOut),
        .data = {
            [0 ... 3] = 0xFF,
        },
};

static void xboxogd_init(void)
{
    DebugPrintf("xboxogd_init");

    xboxog_setup();
}

static void xboxogd_reset(uint8_t __unused rhport)
{
    DebugPrintf("xboxogd_reset");
    itf_num = 0;
}

static uint16_t xboxogd_open(uint8_t __unused rhport, tusb_desc_interface_t const *itf_desc, uint16_t max_len)
{

    /*
    TU_VERIFY(TUSB_CLASS_VENDOR_SPECIFIC == itf_desc->bInterfaceClass &&
                  RESET_INTERFACE_SUBCLASS == itf_desc->bInterfaceSubClass &&
                  RESET_INTERFACE_PROTOCOL == itf_desc->bInterfaceProtocol,
              0);
              */

    uint16_t const drv_len = sizeof(tusb_desc_interface_t) +
                             itf_desc->bNumEndpoints * sizeof(tusb_desc_endpoint_t);
    TU_VERIFY(max_len >= drv_len, 0);

    itf_num = itf_desc->bInterfaceNumber;

    uint8_t const *p_desc = tu_desc_next(itf_desc);
    uint8_t found_endpoints = 0;
    while ((found_endpoints < itf_desc->bNumEndpoints) && (drv_len <= max_len))
    {
        tusb_desc_endpoint_t const *desc_ep = (tusb_desc_endpoint_t const *)p_desc;
        if (TUSB_DESC_ENDPOINT == tu_desc_type(desc_ep))
        {
            TU_ASSERT(usbd_edpt_open(rhport, desc_ep));
            found_endpoints += 1;
        }
        p_desc = tu_desc_next(p_desc);
    }

    // usbd_sof_enable(rhport, SOF_CONSUMER_USER, true);

    DebugPrintf("xboxogd_open %d %d success %d", itf_num, drv_len, found_endpoints);
    return drv_len;
}
static bool xboxogd_control_request_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request)
{
    if (request->wIndex == itf_num && stage == CONTROL_STAGE_SETUP)
    {
        // DebugPrintf("xboxogd_ctrl req:%02x reqtype:%02x wV:%04x Len:%d", request->bRequest, request->bmRequestType, request->wValue, request->wLength);

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
                    xboxog_makeReport();

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
    }
    return true;
}

static bool xboxogd_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
    (void)rhport;

    if (ep_addr == XBOX_IN_EPADDR && result == XFER_RESULT_SUCCESS)
    {
        xboxog_process_output(xferred_bytes);
    }

    return true;
}

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request)
{
    return xboxogd_control_request_cb(rhport, stage, request);
}

static usbd_class_driver_t const _xboxogd_driver =
    {
#if CFG_TUSB_DEBUG >= 2
        .name = "XBOXOG",
#endif
        .init = xboxogd_init,
        .reset = xboxogd_reset,
        .open = xboxogd_open,
        .control_xfer_cb = xboxogd_control_request_cb,
        .xfer_cb = xboxogd_xfer_cb,
        .sof = NULL,
};

#if !(ENABLE_CDC_DEBUG)

// Implement callback to add our custom driver
usbd_class_driver_t const *usbd_app_driver_get_cb(uint8_t *driver_count)
{
    *driver_count = 1;
    return &_xboxogd_driver;
}

#endif