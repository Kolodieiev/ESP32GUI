#pragma once
#include <Arduino.h>
#include "lib/GUI/IScreen.h"
#include "lib/GUI/widget/text/Label.h"

class FlashScreen : public IScreen
{

public:
    FlashScreen(TFT_eSPI &tft);
    virtual ~FlashScreen(){}

protected:
    virtual void loop() override;
    virtual void doGUIWork() override;
    //
    virtual void upClickedHandler() override;
    virtual void downClickedHandler() override;
    virtual void leftClickedHandler() override;
    virtual void rightClickedHandler() override;
    virtual void okClickedHandler() override;
    virtual void okPressedHandler() override;
    virtual void upPressedHandler() override;
    virtual void downPressedHandler() override;
    virtual void leftPressedHandler() override;
    virtual void rightPressedHandler() override;

private:
    uint8_t _bright{50};
    uint8_t _display_brightness;
};
