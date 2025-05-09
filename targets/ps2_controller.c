#include "ps2_controller.h"

#include "ps2_phy.h"

#include "common_types.h"

#include <string.h>

ps2_button_state_t ps2_input_state = {0x00};

static uint8_t mode = MODE_PS2_STARTUP;
static bool config = false;
static bool analogLock = false;
static bool isTatacon = false;

static uint8_t motorBytes[6];
static uint8_t pollConfig[4];

static uint8_t fullPadState[19] = {0};

uint8_t debug_counter = 0;

bool prev_analog_button = false;

uint8_t prev_effector_sent = 0;
uint8_t gf_effector_hold_cnt = 0;

void setMode(uint8_t newMode)
{
    /*
    if(analogLock)
    {
        DebugPrintf("Not changing!");
        return;
    }
    */

    /*
     if (mode == MODE_ANALOG || mode == MODE_ANALOG_PRESSURE)
     {
         gpio_put(PIN_DEBUG, 1);
     }
     else
     {
         gpio_put(PIN_DEBUG, 0);
     }
     */

    // DebugPrintf("Changing mode to: %02x", newMode);
    mode = newMode;
}

void __not_in_flash_func(updateFullPadState)()
{
    // PS2 is active low, so convert our local state.
    ps2_input_state.buttons1.val.select = !final_input_report.short_report.select;
    ps2_input_state.buttons1.val.l3 = !final_input_report.short_report.l3;
    ps2_input_state.buttons1.val.r3 = !final_input_report.short_report.r3;
    ps2_input_state.buttons1.val.start = !final_input_report.short_report.start;
    ps2_input_state.buttons1.val.up = !final_input_report.short_report.dpad_up;
    ps2_input_state.buttons1.val.right = !final_input_report.short_report.dpad_right;
    ps2_input_state.buttons1.val.down = !final_input_report.short_report.dpad_down;
    ps2_input_state.buttons1.val.left = !final_input_report.short_report.dpad_left;

    ps2_input_state.buttons2.val.l2 = !final_input_report.short_report.l2;
    ps2_input_state.buttons2.val.r2 = !final_input_report.short_report.r2;
    ps2_input_state.buttons2.val.l1 = !final_input_report.short_report.l1;
    ps2_input_state.buttons2.val.r1 = !final_input_report.short_report.r1;
    ps2_input_state.buttons2.val.tri = !final_input_report.short_report.btn_north;
    ps2_input_state.buttons2.val.cir = !final_input_report.short_report.btn_east;
    ps2_input_state.buttons2.val.cross = !final_input_report.short_report.btn_south;
    ps2_input_state.buttons2.val.squ = !final_input_report.short_report.btn_west;

    // ensure the id dpad is set for custom controllers.
    switch (final_input_report.short_report.controller_type)
    {
    case SPECIAL_CONTROLLER_GUITAR:
        ps2_input_state.buttons1.val.left = false;
        break;

    case SPECIAL_CONTROLLER_POPN:
        ps2_input_state.buttons1.val.down = false;
        ps2_input_state.buttons1.val.left = false;
        ps2_input_state.buttons1.val.right = false;
        break;

    case SPECIAL_CONTROLLER_GF_GUITAR:
        ps2_input_state.buttons1.val.left = false;
        ps2_input_state.buttons1.val.right = false;

        // if the effector has changed on the arcade controller
        // then we need to hold select for just a little bit
        // to let the game know an effect has changed.
        if (prev_effector_sent != final_input_report.short_report.gf_effector)
        {
            gf_effector_hold_cnt = GF_EFFECTOR_HOLD_COUNT;
        }

        if (gf_effector_hold_cnt > 0)
        {
            // ps2 is active low, so hold.
            ps2_input_state.buttons1.val.select = false;
            gf_effector_hold_cnt--;

            prev_effector_sent = final_input_report.short_report.gf_effector;
        }
        else
        {
            ps2_input_state.buttons1.val.select = true;
        }

        break;

    case SPECIAL_CONTROLLER_DM_DRUMS:
        ps2_input_state.buttons1.val.up = false;
        ps2_input_state.buttons1.val.left = false;
        ps2_input_state.buttons1.val.right = false;
        break;

    case SPECIAL_CONTROLLER_TATACON:
        isTatacon = true;
        break;

    default:
        break;
    }

    // map the guide button to the analog change for PSX games.
    if (final_input_report.short_report.guide &&
        prev_analog_button != final_input_report.short_report.guide)
    {
        force_ps2_to_analog();
    }
    prev_analog_button = final_input_report.short_report.guide;

    // start gen the buffer.
    fullPadState[0] = 0x5A;
    fullPadState[1] = ps2_input_state.buttons1.raw;
    fullPadState[2] = ps2_input_state.buttons2.raw;
    fullPadState[3] = final_input_report.short_report.axis_rx;
    fullPadState[4] = final_input_report.short_report.axis_ry;
    fullPadState[5] = final_input_report.short_report.axis_lx;
    fullPadState[6] = final_input_report.short_report.axis_ly;

    if (final_input_report.enable_analog_facebuttons)
    {
        fullPadState[7] = final_input_report.analog_dpad_right;
        fullPadState[8] = final_input_report.analog_dpad_left;
        fullPadState[9] = final_input_report.analog_dpad_up;
        fullPadState[10] = final_input_report.analog_dpad_down;
        fullPadState[11] = final_input_report.analog_btn_north;
        fullPadState[12] = final_input_report.analog_btn_east;
        fullPadState[13] = final_input_report.analog_btn_south;
        fullPadState[14] = final_input_report.analog_btn_west;
        fullPadState[15] = final_input_report.analog_l1;
        fullPadState[16] = final_input_report.analog_r1;
    }
    else
    {
        // other wise convert binary to pure analog.
        fullPadState[7] = (ps2_input_state.buttons1.val.right) ? 0x00 : 0xFF;
        fullPadState[8] = (ps2_input_state.buttons1.val.left) ? 0x00 : 0xFF;
        fullPadState[9] = (ps2_input_state.buttons1.val.up) ? 0x00 : 0xFF;
        fullPadState[10] = (ps2_input_state.buttons1.val.down) ? 0x00 : 0xFF;
        fullPadState[11] = (ps2_input_state.buttons2.val.tri) ? 0x00 : 0xFF;
        fullPadState[12] = (ps2_input_state.buttons2.val.cir) ? 0x00 : 0xFF;
        fullPadState[13] = (ps2_input_state.buttons2.val.cross) ? 0x00 : 0xFF;
        fullPadState[14] = (ps2_input_state.buttons2.val.squ) ? 0x00 : 0xFF;
        fullPadState[15] = (ps2_input_state.buttons2.val.l1) ? 0x00 : 0xFF;
        fullPadState[16] = (ps2_input_state.buttons2.val.r1) ? 0x00 : 0xFF;
    }

    // our internal input state has analog triggers for these.
    fullPadState[17] = (final_input_report.analog_l2 > 0) ? final_input_report.analog_l2 : (ps2_input_state.buttons2.val.l2) ? 0x00
                                                                                                                             : 0xFF;
    fullPadState[18] = (final_input_report.analog_r2 > 0) ? final_input_report.analog_r2 : (ps2_input_state.buttons2.val.r2) ? 0x00
                                                                                                                             : 0xFF;
}

void initPS2ControllerState()
{
    setMode(MODE_PS2_STARTUP);
    config = false;
    analogLock = false;

    memset(motorBytes, 0xFF, sizeof(motorBytes));
    // memset(pollConfig, 0x00, sizeof(pollConfig));
    memset(&ps2_input_state, 0xFF, sizeof(ps2_input_state));
    memset(fullPadState, 0xFF, sizeof(fullPadState));

    ps2_input_state.buttons1.raw = 0xFF;
    ps2_input_state.buttons2.raw = 0xFF;

    // I've found some games don't give us a poll config before asking for it
    // so let's predefine a sane one.
    pollConfig[0] = 0xFF;
    pollConfig[1] = 0xFF;
    pollConfig[2] = 0x03;
    pollConfig[3] = 0x00;

    updateFullPadState();
}

void processRumble(uint8_t index, uint8_t value)
{
    if (index == 0 || index > 5)
    {
        return;
    }

    switch (motorBytes[index - 1])
    {
    case 0x00:
        // this rumble motor is binary.
        value = value ? 0xFF : 0x00;

        if (value != output_report.rumbleSmall)
        {
            output_report.new_output_report = true;
        }

        output_report.rumbleSmall = value ? 0xFF : 0x00;
        break;

    case 0x01:

        if (value != output_report.rumbleLarge)
        {
            output_report.new_output_report = true;
        }

        output_report.rumbleLarge = value;
        break;
    }
}

// placed in RAM for quick replies to the host.
void __not_in_flash_func(process_joy_req)()
{
    SEND(config ? MODE_CONFIG : mode);
    uint8_t cmd = RECV_CMD();

    switch (cmd)
    {
    case CMD_POLL:
        processPoll();
        break;
    case CMD_CONFIG:
        if (!isTatacon) processConfig();
        break;
    case CMD_STATUS:
        processStatus();
        break;
    case CMD_CONST_46:
        processConst46();
        break;
    case CMD_CONST_47:
        processConst47();
        break;
    case CMD_CONST_4C:
        processConst4c();
        break;
    case CMD_POLL_CONFIG_STATUS:
        if (!isTatacon) processPollConfigStatus();
        break;
    case CMD_ENABLE_RUMBLE:
        processEnableRumble();
        break;
    case CMD_POLL_CONFIG:
        if (!isTatacon) processPollConfig();
        break;
    case CMD_PRES_CONFIG:
        if (!isTatacon) processPresConfig();
        break;
    case CMD_ANALOG_SWITCH:
        if (!isTatacon) processAnalogSwitch();
        break;
    default:
        // DebugPrintf("Unknown CMD: 0x%.2x", cmd);
        // pulseDebugPin();
        // sleep_us(250);
        break;
    }

    // print our debug *after* the transaction as it takes time
    // and we need to service our host.
    if (cmd != CMD_POLL && cmd != CMD_CONFIG && cmd != CMD_ENABLE_RUMBLE)
    {
        DebugPrintf("%02x %02x %d", mode, cmd, debug_counter++);
    }
}

// 0x40
void processPresConfig()
{
    if (!config)
    {
        return;
    }
    uint8_t buf[7] = {0x5A,
                      0x00,
                      0x00,
                      0x02,
                      0x00,
                      0x00,
                      0x5A};
    for (uint8_t i = 0; i < 7; i++)
    {
        SEND(buf[i]);
        RECV_CMD();
    }
}
// 0x41
void processPollConfigStatus()
{
    if (!config)
    {
        return;
    }
    // DebugOutputBuffer("PFG", pollConfig, sizeof(pollConfig));
    uint8_t buf[7] = {0x5A,
                      (mode == MODE_DIGITAL) ? 0x00 : pollConfig[0],
                      (mode == MODE_DIGITAL) ? 0x00 : pollConfig[1],
                      (mode == MODE_DIGITAL) ? 0x00 : pollConfig[2],
                      (mode == MODE_DIGITAL) ? 0x00 : pollConfig[3],
                      0x00,
                      (mode == MODE_DIGITAL) ? 0x00 : 0x5A};
    for (uint8_t i = 0; i < 7; i++)
    {
        SEND(buf[i]);
        RECV_CMD();
    }
}
// 0x42
void processPoll()
{
    config = false;

    updateFullPadState();

    switch (mode)
    {
    case MODE_DIGITAL:
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            SEND(fullPadState[i]);
            processRumble(i, RECV_CMD());
        }
        break;
    }
    case MODE_ANALOG:
    {
        for (uint8_t i = 0; i < 7; i++)
        {
            SEND(fullPadState[i]);
            processRumble(i, RECV_CMD());
        }
        break;
    }
    case MODE_ANALOG_PRESSURE:
    {
        for (uint8_t i = 0; i < 19; i++)
        {
            SEND(fullPadState[i]);
            processRumble(i, RECV_CMD());
        }
        break;
    }
    }
}
// 0x43
void processConfig()
{
    updateFullPadState();

    switch (config ? MODE_CONFIG : mode)
    {
    case MODE_CONFIG:
    {
        for (uint8_t i = 0; i < 7; i++)
        {
            SEND((i == 0) ? 0x5A : 0x00);
            if (i == 1)
                config = RECV_CMD();
            else
                RECV_CMD();
        }
        break;
    }
    case MODE_DIGITAL:
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            SEND(fullPadState[i]);
            if (i == 1)
                config = RECV_CMD();
            else
                RECV_CMD();
        }
        break;
    }
    case MODE_ANALOG:
    {
        for (uint8_t i = 0; i < 7; i++)
        {
            SEND(fullPadState[i]);
            if (i == 1)
                config = RECV_CMD();
            else
                RECV_CMD();
        }
        break;
    }
    case MODE_ANALOG_PRESSURE:
    {
        for (uint8_t i = 0; i < 19; i++)
        {
            SEND(fullPadState[i]);
            if (i == 1)
                config = RECV_CMD();
            else
                RECV_CMD();
        }
        break;
    }
    }
}

uint8_t detectAnalogType()
{
    if ((pollConfig[0] + pollConfig[1] + pollConfig[2] + pollConfig[3]) > 0)
    {
        return MODE_ANALOG_PRESSURE;
    }
    else
    {
        return MODE_ANALOG;
    }
}

// 0x44
void processAnalogSwitch()
{
    if (!config)
    {
        return;
    }
    for (uint8_t i = 0; i < 7; i++)
    {
        switch (i)
        {
        case 0:
            SEND(0x5A);
            RECV_CMD();
            break;
        case 1:
            SEND(0x00);
            setMode((RECV_CMD() == 0x01) ? detectAnalogType() : MODE_DIGITAL);
            break;
        case 2:
            SEND(0x00);
            analogLock = (RECV_CMD() == 0x03) ? 1 : 0;
            break;
        default:
            SEND(0x00);
            RECV_CMD();
            break;
        }
    }
}
// 0x45
void processStatus()
{
    if (!config)
    {
        return;
    }

    // guitar hero II only works if the type is 0x01
    bool isGuitar = (final_input_report.short_report.controller_type == SPECIAL_CONTROLLER_GUITAR);

    uint8_t buf[7] = {0x5A,
                      isGuitar ? 0x01 : 0x03,               // Physical Type: 0x03 standard, 0x01 guitar
                      0x02,                                 // 0x02 is dualshock
                      (mode == MODE_DIGITAL) ? 0x00 : 0x01, // analog light on/off
                      0x02,
                      0x01,
                      0x00};
    for (uint8_t i = 0; i < 7; i++)
    {

        SEND(buf[i]);
        RECV_CMD();
    }
}
// 0x46
void processConst46()
{
    if (!config)
    {
        return;
    }
    SEND(0x5A);
    RECV_CMD();
    SEND(0x00);
    uint8_t offset = RECV_CMD();
    uint8_t buf[5] = {0x00,
                      0x00,
                      (offset == 0x00) ? 0x02 : 0x00,
                      (offset == 0x00) ? 0x00 : 0x00,
                      (offset == 0x00) ? 0x0A : 0x14};
    for (uint8_t i = 0; i < 5; i++)
    {
        SEND(buf[i]);
        RECV_CMD();
    }
}
// 0x47
void processConst47()
{
    if (!config)
    {
        return;
    }
    uint8_t buf[7] = {0x5A,
                      0x00,
                      0x00,
                      0x02,
                      0x00,
                      0x01,
                      0x00};
    for (uint8_t i = 0; i < 7; i++)
    {
        SEND(buf[i]);
        RECV_CMD();
    }
}
// 0x4C
void processConst4c()
{
    if (!config)
    {
        return;
    }
    SEND(0x5A);
    RECV_CMD();
    SEND(0x00);
    uint8_t offset = RECV_CMD();
    uint8_t buf[5] = {0x00,
                      0x00,
                      (offset == 0x00) ? 0x04 : 0x07, // controller type (if set to 0x01 it's digital)
                      0x00,
                      0x00};
    for (uint8_t i = 0; i < 5; i++)
    {
        SEND(buf[i]);
        RECV_CMD();
    }
}
// 0x4D
void processEnableRumble()
{
    if (!config)
    {
        return;
    }
    for (uint8_t i = 0; i < 7; i++)
    {
        if (i == 0)
        {
            SEND(0x5A);
            RECV_CMD();
        }
        else
        {
            SEND(motorBytes[i - 1]);
            motorBytes[i - 1] = RECV_CMD();
        }
    }

    // DebugOutputBuffer("MTR:", motorBytes, sizeof(motorBytes));
}
// 0x4F
void processPollConfig()
{
    if (!config)
    {
        return;
    }
    for (int i = 0; i < 7; i++)
    {
        if (i >= 1 && i <= 4)
        {
            SEND((0x00));
            pollConfig[i - 1] = RECV_CMD();
        }
        else
        {
            SEND((i == 0 || i == 6) ? 0x5A : 0x00);
            RECV_CMD();
        }
    }

    // DebugOutputBuffer("PFG:", pollConfig, sizeof(pollConfig));
    setMode(detectAnalogType());
}

void force_ps2_to_analog()
{
    if (!analogLock)
    {
        DebugPrintf("Changing to analog...");
        setMode(MODE_ANALOG);
    }
    else
    {
        DebugPrintf("Analog lock enabled.");
    }
}