#pragma once
#include <TFT_eSPI.h>
#include "IScreen.h"

class ESP32GUI
{
public:
    void run();

private:
    TFT_eSPI _tft;
};

extern ESP32GUI GUI;
