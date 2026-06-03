#include "debug.h"
#include "xinput_tusb_driver.h"
#include "xinput_descriptors.h"

#include "xinput.h"

#include <xsm3.h>

static uint8_t itf_num;

static uint8_t endpoint_in = 0;
static uint8_t endpoint_out = 0;

uint8_t xauth_buff[0x22];
uint8_t serial[12];

static void xinputd_init(void)
{
    DebugPrintf("xinputd_init");

    // generate a "random" serial for this "controller".
    uint8_t start = to_us_since_boot(get_absolute_time());

    for (int i = 0; i < sizeof(serial); i++)
    {
        serial[i] = (uint8_t)start++;
    }

    // DebugOutputBuffer("SER", serial, sizeof(serial));
}

static void xinputd_reset(uint8_t __unused rhport)
{
    DebugPrintf("xinputd_reset");
    itf_num = 0;
}

static uint16_t xinputd_open(uint8_t rhport,
                             tusb_desc_interface_t const *itf_descriptor,
                             uint16_t max_len)
{
    uint16_t driver_length = 0;
    // Xbox 360 Vendor USB Interfaces: Control, Audio, Plug-in, Security
    if (TUSB_CLASS_VENDOR_SPECIFIC == itf_descriptor->bInterfaceClass)
    {
        driver_length = sizeof(tusb_desc_interface_t) + (itf_descriptor->bNumEndpoints * sizeof(tusb_desc_endpoint_t));
        // TU_VERIFY(max_length >= driver_length, 0);

        tusb_desc_interface_t *p_desc = (tusb_desc_interface_t *)itf_descriptor;
        // Xbox 360 Interfaces (Control 0x01, Audio 0x02, Plug-in 0x03)
        if (itf_descriptor->bInterfaceSubClass == 0x5D &&
            ((itf_descriptor->bInterfaceProtocol == 0x01) ||
             (itf_descriptor->bInterfaceProtocol == 0x02) ||
             (itf_descriptor->bInterfaceProtocol == 0x03)))
        {
            // Get Xbox 360 Definition
            p_desc = (tusb_desc_interface_t *)tu_desc_next(p_desc);
            // TU_VERIFY(XINPUT_DESC_TYPE_RESERVED == p_desc->bDescriptorType, 0);
            driver_length += p_desc->bLength;
            p_desc = (tusb_desc_interface_t *)tu_desc_next(p_desc);
            // Control Endpoints are used for gamepad input/output
            if (itf_descriptor->bInterfaceProtocol == 0x01)
            {
                TU_ASSERT(usbd_open_edpt_pair(rhport, (const uint8_t *)p_desc, itf_descriptor->bNumEndpoints,
                                              TUSB_XFER_INTERRUPT, &endpoint_out, &endpoint_in),
                          0);
            }
            // Xbox 360 Security Interface
        }
        else if (itf_descriptor->bInterfaceSubClass == 0xFD &&
                 itf_descriptor->bInterfaceProtocol == 0x13)
        {
            // Xinput reserved endpoint
            //-------------- Xinput Descriptor --------------//
            p_desc = (tusb_desc_interface_t *)tu_desc_next(p_desc);
            // TU_VERIFY(0x41 == p_desc->bDescriptorType, 0);
            driver_length += p_desc->bLength;
        }
    }

    DebugPrintf("xinputd_open itf=%d len=%d",
                itf_descriptor->bInterfaceNumber,
                driver_length);

    return driver_length;
}

bool xinputd_control_request_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request)
{
    /*
    DebugPrintf("itf_num %d stage:%d wIndex:%04x bRequest:%02x bmRequestType:%02x wValue:%04x wLength:%04x",
                itf_num, stage, request->wIndex, request->bRequest, request->bmRequestType, request->wValue, request->wLength);
    */

    switch (request->bRequest)
    {

    case 0x01:
        if (stage == CONTROL_STAGE_SETUP && request->bmRequestType == 0xC1)
        {
            xinput_make_report();

            if (request->wLength != sizeof(gamepad_state))
            {
                DebugPrintf("xinput %d != %d", request->wLength, sizeof(gamepad_state));
            }

            tud_control_xfer(rhport, request, &gamepad_state, request->wLength);
        }
        break;

    case 0x81:
        if (stage == CONTROL_STAGE_SETUP)
        {
            DebugPrintf("0x81 Start of xsm3");

            // clear the local working space.
            memset(&xauth_buff, 0x00, sizeof(xauth_buff));

            // setup libxsm3 to match this controller's unique config.
            xsm3_set_vid_pid(serial, VID_XINPUT, PID_XINPUT);
            xsm3_initialise_state();
            xsm3_set_identification_data(xsm3_id_data_ms_controller);

            // give the info to the console.
            tud_control_xfer(rhport, request, xsm3_id_data_ms_controller, sizeof(xsm3_id_data_ms_controller));
        }
        break;

    case 0x82:
        if (stage == CONTROL_STAGE_DATA)
        {
            DebugPrintf("0x82 xsm3_do_challenge_init");
            xsm3_do_challenge_init(xauth_buff);
        }
        else if (stage == CONTROL_STAGE_SETUP)
        {
            tud_control_xfer(rhport, request, xauth_buff, request->wLength);
        }
        break;

    case 0x83:
        if (stage == CONTROL_STAGE_SETUP)
        {
            DebugPrintf("0x83 xsm3_challenge_response");
            tud_control_xfer(rhport, request, xsm3_challenge_response, sizeof(xsm3_challenge_response));
        }
        break;

    case 0x84:
        if (stage == CONTROL_STAGE_SETUP)
        {
            DebugPrintf("0x84 keep alive");
            tud_control_xfer(rhport, request, NULL, 0);
        }
        break;

    case 0x86:
        if (stage == CONTROL_STAGE_SETUP)
        {
            DebugPrintf("0x86 confirming complete");

            // two is complete, one is ongoing.
            static short state = 2;
            tud_control_xfer(rhport, request, &state, sizeof(state));
        }
        break;

    case 0x87:
        if (stage == CONTROL_STAGE_DATA)
        {
            DebugPrintf("0x87 verifying complete");
            xsm3_do_challenge_verify(xauth_buff);
        }
        else
        {
            tud_control_xfer(rhport, request, xauth_buff, request->wLength);
        }
        break;

    default:
        DebugPrintf("unhandled.");
        break;
    }

    return true;
}

static bool xinputd_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
    // DebugPrintf("xinputd_xfer_cb %d %02x %d %d", rhport, ep_addr, result, xferred_bytes);
}

usbd_class_driver_t const _xinputd_driver =
    {
#if CFG_TUSB_DEBUG >= 2
        .name = "XINPUT",
#endif
        .init = xinputd_init,
        .reset = xinputd_reset,
        .open = xinputd_open,
        .control_xfer_cb = xinputd_control_request_cb,
        .xfer_cb = xinputd_xfer_cb,
        .sof = NULL,
};
