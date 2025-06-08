#ifndef _I2C_H_
#define _I2C_H_

#include "hardware/i2c.h"
#include "common_types.h"

#define I2C_HELPER_ADDR 0x69
#define I2C_HELPER_SPEED 400000

#define REPEAT_SEND_TIME_US (250 * 1000)
#define HAVENT_SEEN_REPEAT_SEND_TIME_US (5 * 1000 * 1000)

void i2c_setup();
void i2c_task();

void i2c_send_state(input_report_short_t rpt);

#endif