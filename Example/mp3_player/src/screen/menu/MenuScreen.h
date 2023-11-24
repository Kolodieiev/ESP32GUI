#pragma once
#include <Arduino.h>

#include "screen/WidgetCreator.h"
#include "lib/GUI/IScreen.h"
#include "lib/GUI/widget/scrollbar/ScrollBar.h"
#include "lib/GUI/widget/menu/FixedMenu.h"

class MenuScreen : public IScreen
{

public:
    MenuScreen(TFT_eSPI &tft);
    virtual ~MenuScreen(){};

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
    WidgetCreator _creator{_tft};

    ScrollBar *_scrollbar;
    FixedMenu *_menu;
};