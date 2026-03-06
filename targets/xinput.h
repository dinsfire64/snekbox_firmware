#ifndef _XINPUT_H_
#define _XINPUT_H_

#include <stdint.h>

#pragma pack(push, 1)

typedef struct
{
  uint8_t rid;
  uint8_t rsize;

  uint8_t dpad_up : 1;
  uint8_t dpad_down : 1;
  uint8_t dpad_left : 1;
  uint8_t dpad_right : 1;
  uint8_t start : 1;
  uint8_t back : 1;
  uint8_t left_stick : 1;
  uint8_t right_stick : 1;

  uint8_t left_bumper : 1;
  uint8_t right_bumper : 1;
  uint8_t home : 1;
  uint8_t _pad : 1;
  uint8_t a : 1;
  uint8_t b : 1;
  uint8_t x : 1;
  uint8_t y : 1;

  uint8_t lt;
  uint8_t rt;
  int16_t l_x;
  int16_t l_y;
  int16_t r_x;
  int16_t r_y;
  uint8_t reserved_1[6];
} XInputReport_t;

#pragma pack(pop)

_Static_assert(sizeof(XInputReport_t) == 20, "XInputReport_t size error");

extern XInputReport_t gamepad_state;

void xinput_task();
void xinput_make_report();

#endif