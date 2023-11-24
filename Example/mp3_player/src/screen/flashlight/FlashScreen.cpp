#include <pgmspace.h>
#include "FlashScreen.h"
#include "screen/resources/color.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/navbar/NavBar.h"
#include "lib/pref/PrefManager.h"
#include "lib/GUI/util/Util.h"

FlashScreen::FlashScreen(TFT_eSPI &tft): IScreen{tft}
{
    PrefManager pref;
    String display_bright = pref.getPref(STR_BRIGHT_PREF);

    if (display_bright.equals(""))
        _display_brightness = 100;
    else
        _display_brightness = atoi(display_bright.c_str());

    EmptyLayout *layout = new EmptyLayout(1, _tft);
    layout->setBackColor(COLOR_RED);
    layout->setWidth(_tft.width());
    layout->setHeight(_tft.height());

    setLayout(layout);
}

void FlashScreen::upClickedHandler()
{
    if (_bright < 240)
    {
        _bright += 10;
        Util::setBrightness(_bright);
    }
}

void FlashScreen::downClickedHandler()
{
    if (_bright > 20)
    {
        _bright -= 10;
        Util::setBrightness(_bright);
    }
}

void FlashScreen::leftClickedHandler()
{
    getLayout()->setBackColor(COLOR_RED);
}

void FlashScreen::rightClickedHandler()
{
    getLayout()->setBackColor(COLOR_WHITE);
}

void FlashScreen::okPressedHandler()
{
    Util::setBrightness(_display_brightness);
    openScreenByID(ID_SCREEN_HOME);
}

void FlashScreen::okClickedHandler()
{
}

void FlashScreen::loop()
{
}

void FlashScreen::doGUIWork()
{
}

void FlashScreen::upPressedHandler()
{
}

void FlashScreen::downPressedHandler()
{
}

void FlashScreen::leftPressedHandler()
{
}

void FlashScreen::rightPressedHandler()
{
}