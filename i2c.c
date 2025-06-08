#include "i2c.h"

#include "debug.h"

#include "hardware/watchdog.h"
#include "input_report.h"
#include "pico/stdlib.h"

#include "common_types.h"
#include "settings.h"
#include "ws2812.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <i2c_slave.h>
#include <i2c_fifo.h>

bool i2c_led_toggle;
input_report_short_t prev_sent_report;
input_report_short_t incoming_report;

bool has_setup_i2c = false;
bool has_seen_slave = false;

volatile bool new_report = false;

uint64_t prev_send_time = 0;
uint8_t debug_cnt = 0;

static struct
{
    uint8_t mem[256];
    uint8_t mem_address;
} context;

void i2c_led_toggle_cmd()
{
    gpio_put(PIN_SNEKBOX_LED, i2c_led_toggle);
    i2c_led_toggle = !i2c_led_toggle;
}

uint8_t calculate_checksum(const uint8_t *data, size_t size)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < size; ++i)
    {
        // XOR each byte
        checksum ^= data[i];
    }
    return checksum;
}

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void __not_in_flash_func(i2c_slave_handler)(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE:
        // master has written some data
        uint8_t byte = i2c_read_byte(i2c);
        context.mem[context.mem_address] = byte;
        context.mem_address++;
        break;
    case I2C_SLAVE_REQUEST:
        // master is requesting data
        break;
    case I2C_SLAVE_FINISH:
        // master has signalled Stop / Restart

        // DebugOutputBuffer("i2c", context.mem, context.mem_address);

        if (context.mem_address == sizeof(incoming_report))
        {
            memcpy(&incoming_report, context.mem, sizeof(incoming_report));
            new_report = true;
            context.mem_address = 0;
        }
        else
        {
            DebugPrintf("unknown finish %d", context.mem_address);
        }

        break;
    default:
        DebugPrintf("?? %02x", event);
        break;
    }
}

void i2c_setup()
{
    memset(&prev_sent_report, 0x00, sizeof(prev_sent_report));

    DebugPrintf("setting up i2c");

    i2c_led_toggle = false;
    i2c_led_toggle_cmd();

    i2c_init(SNEK_HELPER_I2C_INSTANCE, I2C_HELPER_SPEED);

    gpio_init(PIN_SNEK_I2C_SDA_PIN);
    gpio_init(PIN_SNEK_I2C_SCL_PIN);

    gpio_set_function(PIN_SNEK_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SNEK_I2C_SCL_PIN, GPIO_FUNC_I2C);

    context.mem_address = 0;

    if (current_settings.current_helper_mode == HELPER_MODE_RECV)
    {
        DebugPrintf("---HELPER CORE %d", get_core_num());
        i2c_slave_init(SNEK_HELPER_I2C_INSTANCE, I2C_HELPER_ADDR, &i2c_slave_handler);

        set_rgb1(0x00, 0x00, 0xff);
    }
    else
    {
        DebugPrintf("SENDER");
    }

    DebugPrintf("i2c setup done");
    has_setup_i2c = true;
}

void i2c_send_state(input_report_short_t rpt)
{
    int count = i2c_write_blocking(SNEK_HELPER_I2C_INSTANCE, I2C_HELPER_ADDR, (uint8_t *)&rpt, sizeof(rpt), false);

    // if we can write at all, we have a slave.
    if (count > 0)
    {
        has_seen_slave = true;
    }

    // if we didn't write the whole thing, or we have an error where we have seen a slave
    // force a resend.
    if (count != sizeof(rpt) || (count < 0 && has_seen_slave))
    {
        DebugPrintf("i2c write err %d", debug_cnt++);

        // force our state machine to constantly resend on a disconnect.
        prev_send_time = 0;
    }

    i2c_led_toggle_cmd();
}

void i2c_task()
{
    if (current_settings.current_helper_mode == HELPER_MODE_RECV)
    {
        if (new_report)
        {
            uint8_t expected_checksum = calculate_checksum((uint8_t *)&incoming_report, sizeof(incoming_report) - 1);

            if (incoming_report.checksum == expected_checksum)
            {
                memcpy(&input_report.short_report, &incoming_report, sizeof(incoming_report));

                i2c_led_toggle_cmd();
                watchdog_update();
            }
            else
            {
                DebugPrintf("checksum invalid %02x != %02x", expected_checksum, incoming_report.checksum);
            }

            new_report = false;
        }
    }
    else
    {
        uint64_t curr_time = time_us_64();

        input_report_short_t to_send = helper_short_report;

        // code that forces 1p over to 2p for testing throughput.
#if DEBUG_MIRROR_HELPER
        to_send = input_report.short_report;
#endif

        if (en_helper_report)
        {
            if (!has_setup_i2c)
            {
                i2c_setup();
            }

            // calc the checksum of everything but the last byte.
            to_send.checksum = calculate_checksum((uint8_t *)&to_send, sizeof(to_send) - 1);

            // send if there is a change
            // or to feed the watchdog if we have seen it
            // or attempt a write slower in case the device just got connected.
            if (memcmp(&to_send, &prev_sent_report, sizeof(prev_sent_report)) ||
                (has_seen_slave && (curr_time - prev_send_time >= REPEAT_SEND_TIME_US)) ||
                (!has_seen_slave && (curr_time - prev_send_time >= HAVENT_SEEN_REPEAT_SEND_TIME_US)))
            {
                i2c_send_state(to_send);

                memcpy(&prev_sent_report, &to_send, sizeof(to_send));
                prev_send_time = curr_time;
            }
        }
    }
}