#ifndef __JOYBUS_HPP
#define __JOYBUS_HPP

#include "pico/stdlib.h"
#include "debug.h"

#pragma pack(push, 1)

typedef struct
{
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t x : 1;
    uint8_t y : 1;
    uint8_t start : 1;
    uint8_t origin : 1;
    uint8_t errlatch : 1;
    uint8_t errstat : 1;

    uint8_t dLeft : 1;
    uint8_t dRight : 1;
    uint8_t dDown : 1;
    uint8_t dUp : 1;
    uint8_t z : 1;
    uint8_t r : 1;
    uint8_t l : 1;
    uint8_t use_origin : 1;
} GCButtons;

typedef struct
{
    GCButtons btn;

    uint8_t xStick;
    uint8_t yStick;
} GCCommon;

// note the bit structures for this can be a bit odd
// see dolphin for orientation:
// https://github.com/dolphin-emu/dolphin/blob/144d19433aa734c19c34e5978a1b817d2aa12663/Source/Core/Core/HW/SI/SI_DeviceGCController.cpp#L181

typedef union
{
    // pokemon xd gale of darkness and
    // luigi's mansion use mode 0.
    struct
    {
        GCCommon common;

        uint8_t cxStick;
        uint8_t cyStick;

        // note how R and L are reversed
        // this can be tested using luigi's mansion and the vacuum.
        // this is true for all nibble based byte words on this architecture.
        uint8_t analogR : 4;
        uint8_t analogL : 4;

        uint8_t analogB : 4;
        uint8_t analogA : 4;
    } mode0;

    // TODO: find game that uses mode 1 reads.
    struct
    {
        GCCommon common;

        uint8_t cyStick : 4;
        uint8_t cxStick : 4;

        uint8_t analogL;
        uint8_t analogR;

        uint8_t analogB : 4;
        uint8_t analogA : 4;
    } mode1;

    // DDR Mario Mix and Hottest Party reads mode 2
    struct
    {
        GCCommon common;

        uint8_t cyStick : 4;
        uint8_t cxStick : 4;

        uint8_t analogR : 4;
        uint8_t analogL : 4;

        uint8_t analogA;
        uint8_t analogB;
    } mode2;

    // mode 3 is the default reading mode
    struct
    {
        GCCommon common;

        uint8_t cxStick;
        uint8_t cyStick;
        uint8_t analogL;
        uint8_t analogR;
    } mode3;

    // DDR II on the wii reads mode 4.
    // if analogA/analogB are not 0xFF, then the game will not let doubles mode play
    // it will mirror p1's side on p2.
    struct
    {
        GCCommon common;

        uint8_t cxStick;
        uint8_t cyStick;
        uint8_t analogA;
        uint8_t analogB;
    } mode4;

    uint8_t raw[8];

} GCReport;

typedef union
{
    struct
    {
        uint8_t dRight : 1;
        uint8_t dLeft : 1;
        uint8_t dDown : 1;
        uint8_t dUp : 1;
        uint8_t s : 1;
        uint8_t z : 1;
        uint8_t b : 1;
        uint8_t a : 1;

        uint8_t cRight : 1;
        uint8_t cLeft : 1;
        uint8_t cDown : 1;
        uint8_t cUp : 1;
        uint8_t r : 1;
        uint8_t l : 1;
        uint8_t UNKN : 1;
        uint8_t RST : 1;

        int8_t xStick;
        int8_t yStick;
    };
} N64Report;

#pragma pack(pop)

#define N64_C_THRESH 55
#define N64_AXIS_MAX 85.0

void gcn_setup(int dataPin);
void gcn_task();

extern GCReport gcReport;

#endif