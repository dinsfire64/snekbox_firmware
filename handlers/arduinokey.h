#ifndef _ARDUINOKEY_H_
#define _ARDUINOKEY_H_

#include <stdint.h>

#include "tusb.h"

#include "common_types.h"

#define ARDUINOKEY_VID1 0x2341
#define ARDUINOKEY_PID1 0x8037

#define ARDUINOKEY_REPORT_ID 0x02

// number of keys to allow in a report, 6k nro is very common.
#define MAX_KEY_NUM 6

bool is_ARDUINOKEY(uint8_t dev_addr);

void processARDUINOKEY(uint8_t const *report, uint16_t len);

#endif
