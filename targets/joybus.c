#include "joybus.h"

#include "pico/platform.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "joybus.pio.h"

#include "__targets.h"

#include "common_types.h"

#include <string.h>

GCCommon gcState;
GCReport dest_report;
N64Report n64Report;
PIO pio = pio1;
uint offset;
int sm_joybus;
pio_sm_config config;
uint8_t buffer[3];
bool n64_connected = false;

/* PIOs are separate state machines for handling IOs with high timing precision. You load a program into them and they do their stuff on their own with deterministic timing,
   communicating with the main cores via FIFOs (and interrupts, if you want).

   Attempting to bit-bang the Joybus protocol by writing down timer snapshots upon GPIO changes and waiting on timers to trigger GPIO changes is bound to encounter some
   issues related to the uncertainty of timings (the STM32F0 is pipelined). Previous versions of this used to do that and would occasionally have the controller disconnect
   for a frame due to a polling fault, which would manifest particularly on the CSS by the hand teleporting back to its default position.
   Ever since the migration to PIOs - the proper tool for the job - this problem is gone.

   This project is adapted from the communication module of pico-rectangle (https://github.com/JulienBernard3383279/pico-rectangle), a digital controller firmware.

   The PIO program expects the system clock to be 125MHz, but you can adapt it by fiddling with the delays in the PIO program.

   One PIO here is configured to wait for new bytes over the joybus protocol. The main core spins a byte to come from the PIO FIFO, which will be the Joybus command
   from the console. It then matches that byte to know whether this is probe, origin or poll request.
   Once it has matched a command and has let the console finish talking, it will start replying, but in the case of the poll command, it will first build the state.
   The state is built by calling a callback passed as parameter to enterMode. This is fine for digital controllers because it takes few microseconds, but isn't
   if your state building is going to take any longer. (The console is fine with some delay between the poll command and response, but adapters don't tolerate more than
   few microseconds) In that case, you'll need to change the control flow for the passed callback to not do any computational work itself.

   Check the T O D O s in this file for things you should check when adapting it to your project.

   I advise checking the RP2040 documentation and this video https://www.youtube.com/watch?v=yYnQYF_Xa8g&ab_channel=stacksmashing to understand
*/

int __time_critical_func(convertToPio)(const uint8_t *command, const int len, uint32_t *result)
{
    // PIO Shifts to the right by default
    // In: pushes batches of 8 shifted left, i.e we get [0x40, 0x03, rumble (the end bit is never pushed)]
    // Out: We push commands for a right shift with an enable pin, ie 5 (101) would be 0b11'10'11
    // So in doesn't need post processing but out does
    uint8_t resultLen = 0;

    if (len != 0)
    {
        resultLen = len / 2 + 1;
        int i;
        for (i = 0; i < resultLen; i++)
        {
            result[i] = 0;
        }
        for (i = 0; i < len; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                result[i / 2] += 1 << (2 * (8 * (i % 2) + j) + 1);
                result[i / 2] += (!!(command[i] & (0x80u >> j))) << (2 * (8 * (i % 2) + j));
            }
        }
        // End bit
        result[len / 2] += 3 << (2 * (8 * (len % 2)));
    }

    return resultLen;
}

void __time_critical_func(convertGCReport)(uint8_t mode)
{
    // convert local report into a gcState
    gcState.btn.dUp = final_input_report.short_report.dpad_up;
    gcState.btn.dDown = final_input_report.short_report.dpad_down;
    gcState.btn.dLeft = final_input_report.short_report.dpad_left;
    gcState.btn.dRight = final_input_report.short_report.dpad_right;

    gcState.btn.a = final_input_report.short_report.btn_east;
    gcState.btn.b = final_input_report.short_report.btn_south;
    gcState.btn.x = final_input_report.short_report.btn_west;
    gcState.btn.y = final_input_report.short_report.btn_north;

    gcState.btn.start = final_input_report.short_report.start;

    gcState.btn.l = final_input_report.short_report.l2;
    gcState.btn.r = final_input_report.short_report.r2;
    gcState.btn.z = final_input_report.short_report.r1 || final_input_report.short_report.select;

    gcState.xStick = final_input_report.short_report.axis_lx;
    gcState.yStick = ~final_input_report.short_report.axis_ly;

    // copy over the bits to the final report.
    dest_report.mode0.common = gcState;

    // make local copies of the weird variables so they can be shifted around.
    uint8_t cxStick = final_input_report.short_report.axis_rx;
    uint8_t cyStick = ~final_input_report.short_report.axis_ry;

    uint8_t analogL = final_input_report.analog_l2;
    uint8_t analogR = final_input_report.analog_r2;

    // holding these are required for some games to detect a dance pad.
    uint8_t analogA = 0xFF;
    uint8_t analogB = 0xFF;

    switch (mode)
    {
    default:
        // Mode 0, 5, 6, 7
        dest_report.mode0.cxStick = cxStick;
        dest_report.mode0.cyStick = cyStick;
        dest_report.mode0.analogL = analogL >> 4;
        dest_report.mode0.analogR = analogR >> 4;
        dest_report.mode0.analogA = analogA >> 4;
        dest_report.mode0.analogB = analogB >> 4;
        break;
    case 1:
        dest_report.mode1.cxStick = cxStick >> 4;
        dest_report.mode1.cyStick = cyStick >> 4;
        dest_report.mode1.analogL = analogL;
        dest_report.mode1.analogR = analogR;
        dest_report.mode1.analogA = analogA >> 4;
        dest_report.mode1.analogB = analogB >> 4;
        break;
    case 2:
        dest_report.mode2.cxStick = cxStick >> 4;
        dest_report.mode2.cyStick = cyStick >> 4;
        dest_report.mode2.analogL = analogL >> 4;
        dest_report.mode2.analogR = analogR >> 4;
        dest_report.mode2.analogA = analogA;
        dest_report.mode2.analogB = analogB;
        break;
    case 3:
        dest_report.mode3.cxStick = cxStick;
        dest_report.mode3.cyStick = cyStick;
        dest_report.mode3.analogL = analogL;
        dest_report.mode3.analogR = analogR;
        // analogA/analogB are not in this mode.
        break;
    case 4:
        dest_report.mode4.cxStick = cxStick;
        dest_report.mode4.cyStick = cyStick;
        // analogL/analogR do not work in this mode.
        dest_report.mode4.analogA = analogA;
        dest_report.mode4.analogB = analogB;
        break;
    }
}

void __time_critical_func(convertN64Report)()
{
    n64Report.a = final_input_report.short_report.btn_east;
    n64Report.b = final_input_report.short_report.btn_south;
    n64Report.z = final_input_report.short_report.r1 ||
                  final_input_report.short_report.l1 ||
                  final_input_report.short_report.select;
    n64Report.s = final_input_report.short_report.start;

    n64Report.dUp = final_input_report.short_report.dpad_up;
    n64Report.dDown = final_input_report.short_report.dpad_down;
    n64Report.dLeft = final_input_report.short_report.dpad_left;
    n64Report.dRight = final_input_report.short_report.dpad_right;

    n64Report.RST = false;

    n64Report.l = final_input_report.short_report.l2;
    n64Report.r = final_input_report.short_report.r2;

    // convert the stick into discrete button presses.
    n64Report.cUp = final_input_report.short_report.axis_ry < N64_C_THRESH;
    n64Report.cDown = final_input_report.short_report.axis_ry > (UINT8_MAX - N64_C_THRESH);
    n64Report.cLeft = final_input_report.short_report.axis_rx < N64_C_THRESH;
    n64Report.cRight = final_input_report.short_report.axis_rx > (UINT8_MAX - N64_C_THRESH);

    // we store uint8, n64 wants signed int8
    n64Report.xStick = final_input_report.short_report.axis_lx - 128;
    n64Report.yStick = ~final_input_report.short_report.axis_ly - 128;

    // clamp the sticks n64 max values.
    n64Report.xStick *= N64_AXIS_MAX / 128;
    n64Report.yStick *= N64_AXIS_MAX / 128;
}

void setupGCNReport()
{
    // all values start as zero except the sticks.
    memset(&gcState, 0x00, sizeof(gcState));

    // sticks are centered by the local report, no need to do so here.

    // starts high until the master calls origin on us.
    gcState.btn.origin = true;
}

void __time_critical_func(gcn_task)()
{
    if (pio_sm_get_rx_fifo_level(pio, sm_joybus) > 0)
    {
        buffer[0] = pio_sm_get_blocking(pio, sm_joybus);

        // Probe or reset.
        if (buffer[0] == 0x00 || buffer[0] == 0xFF)
        {
            // first two bytes are the type of controller we are
            // last byte is rumble status.
            // https://hitmen.c02.at/files/yagcd/yagcd/chap9.html
            // TODO: when under N64, 0x03 means PAK is inserted.
            // this will cause games to attempt to talk to rumble/memory etc.
            // this is also invalid for N64, it should be 0x05 0x00...but some games seem to be okay with it?
            uint8_t probeResponse[3] = {n64_connected ? 0x05 : 0x09,
                                        0x00,
                                        n64_connected ? 0x00 : 0x03};

            uint32_t result[2];
            int resultLen = convertToPio(probeResponse, 3, result);

            sleep_us(6); // 3.75us into the bit before end bit => 6.25 to wait if the end-bit is 5us long

            pio_sm_set_enabled(pio, sm_joybus, false);
            pio_sm_init(pio, sm_joybus, offset + joybus_offset_outmode, &config);
            pio_sm_set_enabled(pio, sm_joybus, true);

            for (int i = 0; i < resultLen; i++)
            {
                pio_sm_put_blocking(pio, sm_joybus, result[i]);
            }

            // DebugPrintf("GCN Probe.");
        }
        else if (buffer[0] == 0x41 || buffer[0] == 0x42)
        {
            // get origin / calibrate origin
            DebugPrintf("GCN Origin");

            // Origin (NOT 0x81)
            uint8_t originResponse[] = {0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x1F, 0x1F, 0x00, 0x00};
            // TODO The origin response sends centered values in this code excerpt.
            // Consider whether that makes sense for your project (digital controllers -> yes)
            uint32_t result[6];
            int resultLen = convertToPio(originResponse, sizeof(originResponse), result);
            // Here we don't wait because convertToPio takes time

            pio_sm_set_enabled(pio, sm_joybus, false);
            pio_sm_init(pio, sm_joybus, offset + joybus_offset_outmode, &config);
            pio_sm_set_enabled(pio, sm_joybus, true);

            for (int i = 0; i < resultLen; i++)
                pio_sm_put_blocking(pio, sm_joybus, result[i]);

            // reset the report since we just called origin.
            gcState.btn.origin = false;
        }
        else if (buffer[0] == 0x40)
        {
            // Could check values past the first byte for reliability
            buffer[0] = pio_sm_get_blocking(pio, sm_joybus);

            // buffer[0] now contains the current polling mode from the game.
            // DebugPrintf("MODE %02x", buffer[0]);
            convertGCReport(buffer[0]);

            // I have NO clue why I needed to force this high this but, but GBI
            // and Pokemon XD Gale of Darkness started working when set to 0b1.
            // not all games must check for it or similar.
            dest_report.mode0.common.btn.use_origin = true;

            buffer[0] = pio_sm_get_blocking(pio, sm_joybus);

            // rumble is the last bit of this byte, binary on/off, active high.
            bool rumble_active = (buffer[0] & 0x01);

            if (output_report.rumble_active != rumble_active)
            {
                output_report.new_output_report = true;
                output_report.rumble_active = rumble_active;
            }

            uint32_t result[5];

            int resultLen = convertToPio((uint8_t *)(&dest_report), sizeof(dest_report), result);

            // sleep_us(7); // add delay so we don't overwrite the stop bit

            pio_sm_set_enabled(pio, sm_joybus, false);
            pio_sm_init(pio, sm_joybus, offset + joybus_offset_outmode, &config);
            pio_sm_set_enabled(pio, sm_joybus, true);

            for (int i = 0; i < resultLen; i++)
                pio_sm_put_blocking(pio, sm_joybus, result[i]);
        }
        else if (buffer[0] == 0x01)
        {
            n64_connected = true;

            // N64 controller state.
            convertN64Report();

            uint32_t result[4];
            int resultLen = convertToPio((uint8_t *)(&n64Report), sizeof(n64Report), result);

            pio_sm_set_enabled(pio, sm_joybus, false);
            pio_sm_init(pio, sm_joybus, offset + joybus_offset_outmode, &config);
            pio_sm_set_enabled(pio, sm_joybus, true);

            for (int i = 0; i < resultLen; i++)
                pio_sm_put_blocking(pio, sm_joybus, result[i]);
        }
        else
        {
            DebugPrintf("GCN Unknown %02x", buffer[0]);

            pio_sm_set_enabled(pio, sm_joybus, false);
            sleep_us(400);
            // If an unmatched communication happens, we wait for 400us for it to finish for sure before starting to listen again
            pio_sm_init(pio, sm_joybus, offset + joybus_offset_inmode, &config);
            pio_sm_set_enabled(pio, sm_joybus, true);
        }
    }
}

void gcn_setup(int dataPin)
{
    n64_connected = false;

    setupGCNReport();

    gpio_init(dataPin);
    gpio_set_dir(dataPin, GPIO_IN);
    gpio_pull_up(dataPin);

    sleep_us(100); // Stabilize voltages

    pio_gpio_init(pio, dataPin);
    offset = pio_add_program(pio, &joybus_program);

    config = joybus_program_get_default_config(offset);
    sm_config_set_in_pins(&config, dataPin);
    sm_config_set_out_pins(&config, dataPin, 1);
    sm_config_set_set_pins(&config, dataPin, 1);
    sm_config_set_clkdiv(&config, 5);
    sm_config_set_out_shift(&config, true, false, 32);
    sm_config_set_in_shift(&config, false, true, 8);

    sm_joybus = pio_claim_unused_sm(pio, true);

    pio_sm_init(pio, sm_joybus, offset, &config);
    pio_sm_set_enabled(pio, sm_joybus, true);
}
