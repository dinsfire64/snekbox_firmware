#ifndef _ICEDRAGON_H_
#define _ICEDRAGON_H_

#include <stdbool.h>
#include <stdint.h>

#include "common_types.h"

#define ICEDRAGON_VID_STAC 0x04D8
#define ICEDRAGON_PID_STAC_P1 0xEA4B
#define ICEDRAGON_PID_STAC_P2 0xEA4A

#define ICEDRAGON_VID_STAC2 0x2E8A
#define ICEDRAGON_PID_STAC2_P1 0x10D9
#define ICEDRAGON_PID_STAC2_P2 0x10E9

#define ICEDRAGON_STAC_REPORT_ID_BUTTONS 0x01

#define ICEDRAGON_VID_SNEK 0x2e8a
#define ICEDRAGON_PID_SNEK 0x10a8

#define ICEDRAGON_SNEK_REPORT_ID_BUTTONS 0x01

#define ICEDRAGON_VID_FUSION_HID 0x0547
#define ICEDRAGON_PID_FUSION_HID 0x1002

#define ICEDRAGON_FUSION_HID_REPORT_ID_BUTTONS 0x01

#pragma pack(push, 1)

typedef struct
{
    uint8_t hid_report_id;

    struct
    {
        bool up : 1;
        bool down : 1;
        bool left : 1;
        bool right : 1;
        bool center : 1;
        uint16_t padding : 11;
    };
} stac_state;

typedef struct
{
    uint8_t hid_report_id;

    struct
    {
        bool p1_up : 1;
        bool p1_down : 1;
        bool p1_left : 1;
        bool p1_right : 1;
        bool p1_center : 1;
        bool btn05 : 1;
        bool btn06 : 1;
        bool btn07 : 1;

        bool p2_up : 1;
        bool p2_down : 1;
        bool p2_left : 1;
        bool p2_right : 1;
        bool p2_center : 1;
        bool btn13 : 1;
        bool btn14 : 1;
        bool btn15 : 1;
    };
} stac2_state;

typedef union
{
    struct
    {
        bool btn_service : 1;
        bool btn_test : 1;
        bool btn_p2_s : 1;
        bool btn_p1_s : 1;
        bool btn_p2_u : 1;
        bool btn_p1_u : 1;
        bool btn_p1_l : 1;
        bool btn_p2_l : 1;

        bool btn_p1_d : 1;
        bool btn_p2_d : 1;
        bool btn_p2_r : 1;
        bool btn_p1_r : 1;
        bool btn_p1_b3 : 1;
        bool btn_p2_b3 : 1;
        bool btn_p1_b2 : 1;
        bool btn_p2_b2 : 1;

        bool btn_coin : 1;
        bool btn_p2_b1 : 1;
        bool btn_p1_b1 : 1;
        bool btn_p2_b4 : 1;
        bool btn_p1_b4 : 1;
        bool btn_p2_b5 : 1;
        bool btn_p1_b5 : 1;
        bool btn_p2_b6 : 1;

        bool btn_p1_b6 : 1;
        bool btn_tilt_slam : 1;
        bool pad_u5_0 : 1;
        bool pad_u5_1 : 1;
        bool pad_u5_2 : 1;
        bool pad_u5_3 : 1;
        bool pad_u5_6 : 1;
        bool coin2_pad : 1;
    };
    uint32_t raw;
} snek_buttons_gamepad_t;

typedef struct
{
    uint8_t report_id;
    snek_buttons_gamepad_t btns;
} snek_hid_report_t;

typedef union
{
    struct
    {
        bool btn_P1_UL_U : 1;
        bool btn_P1_UR_D : 1;
        bool btn_P1_CN_L : 1;
        bool btn_P1_LL_R : 1;
        bool btn_P1_LR_START : 1;
        bool btn_P1_SELECT : 1;
        bool btn_P1_MENU_LEFT : 1;
        bool btn_P1_MENU_RIGHT : 1;

        bool pad_p1_0 : 1;
        bool btn_P1_TEST : 1;
        bool btn_P1_COIN : 1;
        bool pad_p1_3 : 1;
        bool pad_p1_4 : 1;
        bool pad_p1_5 : 1;
        bool btn_P1_SERVICE : 1;
        bool btn_P1_CLR : 1;

        bool btn_P2_UL_U : 1;
        bool btn_P2_UR_D : 1;
        bool btn_P2_CN_L : 1;
        bool btn_P2_LL_R : 1;
        bool btn_P2_LR_START : 1;
        bool btn_P2_SELECT : 1;
        bool btn_P2_MENU_LEFT : 1;
        bool btn_P2_MENU_RIGHT : 1;

        bool pad_p2_0 : 1;
        bool btn_P2_TEST : 1;
        bool btn_P2_COIN : 1;
        bool pad_p2_3 : 1;
        bool pad_p2_4 : 1;
        bool pad_p2_5 : 1;
        bool btn_P2_SERVICE : 1;
        bool btn_CLR : 1;
    };
    uint32_t raw;
} fusion_buttons_gamepad_t;

typedef struct
{
    uint8_t report_id;
    fusion_buttons_gamepad_t btns;
    uint8_t axis0;
    uint8_t axis1;
} fusion_hid_report_t;

#pragma pack(pop)

bool is_STAC(uint8_t dev_addr);
void processSTAC(uint8_t const *report, uint16_t len);

bool is_STAC2(uint8_t dev_addr);
void processSTAC2(uint8_t const *report, uint16_t len);

bool is_SNEK(uint8_t dev_addr);
void processSNEK(uint8_t const *report, uint16_t len);

bool is_FUSION_HID(uint8_t dev_addr);
void processFUSION_HID(uint8_t const *report, uint16_t len);

#endif