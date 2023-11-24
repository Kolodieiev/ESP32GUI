#pragma once
#include "Arduino.h"
#include "screen/WidgetCreator.h"
#include "lib/GUI/IScreen.h"
#include "lib/GUI/widget/menu/FixedMenu.h"
#include "lib/GUI/widget/scrollbar/ScrollBar.h"
#include "lib/GUI/widget/progressbar/ProgressBar.h"

class PrefScreen : public IScreen
{
public:
    PrefScreen(TFT_eSPI &tft);
    virtual ~PrefScreen() {}

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
    enum Mode
    {
        PREF_SELECT_MODE,
        PREF_EDIT_MODE
    };
    enum Preference
    {
        BRIGHT_PREFERENCE
    };

    Mode _mode{PREF_SELECT_MODE};
    Preference _preference{BRIGHT_PREFERENCE};

    FixedMenu *_menu;
    ScrollBar *_scrollbar;
    ProgressBar *_progress;

    void showPrefList();

    // Яскравість
    const uint8_t BRIGHT_ITEM_ID = 1;
    const uint8_t MAX_BRIGHT = 240;
    uint8_t _old_bright;
    void showBrightPref();
    void brightUp();
    void brightDown();
    //
    WidgetCreator _creator{_tft};
};