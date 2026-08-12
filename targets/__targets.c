#include "__targets.h"

#include "settings.h"

#include "ps2_phy.h"
#include "ps2_controller.h"
#include "joybus.h"
#include <stdbool.h>

#include "hardware/watchdog.h"

#include "ws2812.h"

#define DETECT_TRESHOLD 4
uint8_t ps2_counter, gcn_counter;

bool detected = false;

#define GPIO_IRQ_ALL (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL | GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_LEVEL_LOW)

void disable_detect_interrupt()
{
    gpio_set_irq_enabled(PIN_SEL, GPIO_IRQ_ALL, false);
    gpio_set_irq_enabled(PIN_ACK_GCN_DAT, GPIO_IRQ_ALL, false);
}

void __time_critical_func(detect_callback)(uint gpio, uint32_t event)
{
    if (ps2_counter < DETECT_TRESHOLD &&
        gpio == PIN_SEL &&
        event & GPIO_IRQ_EDGE_FALL)
    {
        ps2_counter++;
    }
    else if (gcn_counter < DETECT_TRESHOLD &&
             gpio == PIN_ACK_GCN_DAT &&
             event & GPIO_IRQ_EDGE_FALL)
    {
        gcn_counter++;
    }
}

void setup_detect_irq()
{
    ps2_counter = 0;
    gcn_counter = 0;

    gpio_init(PIN_SEL);
    gpio_set_dir(PIN_SEL, GPIO_IN);
    gpio_pull_up(PIN_SEL);

    gpio_init(PIN_ACK_GCN_DAT);
    gpio_set_dir(PIN_ACK_GCN_DAT, GPIO_IN);
    gpio_pull_up(PIN_ACK_GCN_DAT);

    gpio_set_irq_enabled_with_callback(PIN_SEL, GPIO_IRQ_EDGE_FALL, true, detect_callback);
    // gpio_set_irq_enabled(PIN_SEL, GPIO_IRQ_EDGE_RISE, true);

    gpio_set_irq_enabled_with_callback(PIN_ACK_GCN_DAT, GPIO_IRQ_EDGE_FALL, true, detect_callback);
    // gpio_set_irq_enabled(PIN_ACK_GCN_DAT, GPIO_IRQ_EDGE_RISE, true);

    detected = false;
}

void targets_setup()
{
    setup_detect_irq();
    return;
}

void targets_task()
{
    if (!detected)
    {
        if (ps2_counter >= DETECT_TRESHOLD || gcn_counter >= DETECT_TRESHOLD)
        {
            disable_detect_interrupt();
        }

        if (ps2_counter >= DETECT_TRESHOLD)
        {
            DebugPrintf("PSX found.");

            // set to red for PS2 mode.
            set_rgb0(0xff, 0, 0);

            psx_setup();
            runtime_settings.retro_io_mode = MODE_CONSOLE_PS2;

            detected = true;
        }
        else if (gcn_counter >= DETECT_TRESHOLD)
        {
            DebugPrintf("GCN found.");

            // set to purple for GCN mode
            set_rgb0(0xFF, 0, 0xFF);

            gcn_setup(PIN_ACK_GCN_DAT);
            runtime_settings.retro_io_mode = MODE_CONSOLE_GCN;

            detected = true;
        }
    }
    else
    {

        switch (runtime_settings.retro_io_mode)
        {
        case MODE_CONSOLE_PS2:
            psx_task();
            break;

        case MODE_CONSOLE_GCN:
            gcn_task();
            break;

        default:
            // DebugPrintf("ERR %d", runtime_settings.retro_io_mode);
            break;
        }
    }
}