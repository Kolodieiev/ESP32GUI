#pragma once
#include <Arduino.h>
#include "lib/GUI/IScreen.h"
#include "screen/WidgetCreator.h"

class UpdateScreen : public IScreen
{

public:
    UpdateScreen(TFT_eSPI &tft);
    virtual ~UpdateScreen() {}

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
    const uint8_t SD_CS{5};

    WidgetCreator _creator{_tft};
    
    void showUpdating();
};
