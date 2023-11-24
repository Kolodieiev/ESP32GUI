/*Author Makuna*/

#pragma once

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <inttypes.h>
#endif

#if !defined(ISR_ATTR)

#if defined(ARDUINO_ARCH_ESP8266)
#define ISR_ATTR ICACHE_RAM_ATTR
#elif defined(ARDUINO_ARCH_ESP32)
#define ISR_ATTR ICACHE_RAM_ATTR
#else
#define ISR_ATTR
#endif

#endif

#ifndef _BV
#define _BV(b) (1UL << (b))
#endif

extern uint8_t bcdToUint8(uint8_t val);
extern uint8_t uint8ToBcd(uint8_t val);
extern uint8_t bcdToBin24Hour(uint8_t bcdHour);
extern uint8_t strToUint8(const char *pString);

 