#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdarg.h>
#include "version.h"

#define ENABLE_DEBUG_UART false
#define ENABLE_BUFFER_DUMP false
#define ENABLE_REPORT_DUMP false

#define ENABLE_CDC_DEBUG false

#define DEBUG_MIRROR_HELPER false

#define DEBUG_UART_SLOT SNEK_UART_SLOT_DEBUG
#define DEBUG_UART_BAUD 921600
#define DEBUG_UART_OUT_BUFF_SIZE 512

#define DEBUG_UART_TX_PIN PIN_SNEKBOX_UART_TX
#define DEBUG_UART_RX_PIN PIN_SNEKBOX_UART_RX

#define PIN_DEBUG PIN_SNEKBOX_DEBUG

#define PRINT_TIMING false

#define END_LINE "\r\n"

extern bool debug_setup;

void DebugSetup(void);
void DebugPrintf(const char *fmt, ...);
void DebugOutputBuffer(const char *prefix, uint8_t buff[], int len);
int DebugTinyUSBPrintf(const char *fmt, ...);

void DebugSetupDebugPin();
void pulseDebugPin();

#if !(ENABLE_DEBUG_UART)
#define DebugPrintf(char, ...) ;
#endif

#if !(ENABLE_BUFFER_DUMP)
#define DebugOutputBuffer(char, uint8_t, int) ;
#endif

#endif
