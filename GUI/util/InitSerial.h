#pragma once
#include <Arduino.h>

#if (CORE_DEBUG_LEVEL == 5)

class InitSerial
{
public:
    InitSerial()
    {
        Serial.begin(115200);
        delay(100);
    }
};

InitSerial stub;

#endif