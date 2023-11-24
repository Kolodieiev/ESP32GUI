#include <pgmspace.h>
#include "lib/pref/PrefManager.h"
#include "lib/GUI/util/Util.h"

#include "PrefScreen.h"
#include "screen/resources/color.h"
#include "screen/resources/string.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/text/Focusable.h"
#include "lib/GUI/widget/text/Label.h"
#include "lib/GUI/widget/navbar/NavBar.h"

PrefScreen::PrefScreen(TFT_eSPI &tft): IScreen{tft}
{
    showPrefList();
}

void PrefScreen::showPrefList()
{
    _mode = PREF_SELECT_MODE;

    NavBar *navbar = _creator.getNavbar(1, STR_SELECT, "", STR_BACK);

    _menu = new FixedMenu(2, _tft);
    _menu->setBackColor(COLOR_MENU_ITEM);
    _menu->setWidth(_tft.width() - SCROLLBAR_WIDTH - 2);
    _menu->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _menu->setItemsHeight((_tft.height() - NAVBAR_HEIGHT) / 3);

    Focusable *bright_item = new Focusable(BRIGHT_ITEM_ID, _tft);
    bright_item->setText(STR_BRIGHT);
    bright_item->setGravity(IWidget::GRAVITY_CENTER);
    bright_item->setFocusBorderColor(COLOR_LIME);
    bright_item->setChangeBorderState(true);
    bright_item->setTextColor(COLOR_WHITE);
    bright_item->setBackColor(COLOR_MENU_ITEM);
    bright_item->setTextOffset(3);

    _menu->addWidget(bright_item);

    _scrollbar = new ScrollBar(3, _tft);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _scrollbar->setPos(_tft.width() - SCROLLBAR_WIDTH, 0);
    _scrollbar->setMax(_menu->getSize());

    EmptyLayout *layout = new EmptyLayout(1, _tft);
    layout->setBackColor(COLOR_MAIN_BACK);
    layout->setWidth(_tft.width());
    layout->setHeight(_tft.height());

    layout->addWidget(_menu);
    layout->addWidget(_scrollbar);
    layout->addWidget(navbar);

    setLayout(layout);
}

void PrefScreen::showBrightPref()
{
    _mode = PREF_EDIT_MODE;
    _preference = BRIGHT_PREFERENCE;

    PrefManager pref;
    String display_bright = pref.getPref(STR_BRIGHT_PREF);

    if (display_bright.equals(""))
        _old_bright = 100;
    else
        _old_bright = atoi(display_bright.c_str());

    NavBar *navbar = _creator.getNavbar(1, STR_OK, "", STR_BACK);

    Label *header_txt = new Label(2, _tft);
    header_txt->setText(STR_BRIGHT);
    header_txt->setWidth(_tft.width());
    header_txt->setBackColor(COLOR_MAIN_BACK);
    header_txt->setTextColor(COLOR_WHITE);
    header_txt->setAlign(IWidget::ALIGN_CENTER);
    header_txt->setGravity(IWidget::GRAVITY_CENTER);

    _progress = new ProgressBar(3, _tft);
    _progress->setBackColor(COLOR_BLACK);
    _progress->setProgressColor(COLOR_ORANGE);
    _progress->setBorderColor(COLOR_WHITE);
    _progress->setMax(MAX_BRIGHT);
    _progress->setWidth(_tft.width() - 5 * 8);
    _progress->setHeight(20);
    _progress->setProgress(_old_bright);

    _progress->setPos((float)(_tft.width() - _progress->getWidth()) / 2, 30);

    EmptyLayout *layout = new EmptyLayout(1, _tft);
    layout->setBackColor(COLOR_MAIN_BACK);
    layout->setWidth(_tft.width());
    layout->setHeight(_tft.height());

    layout->addWidget(navbar);
    layout->addWidget(header_txt);
    layout->addWidget(_progress);

    setLayout(layout);
}

void PrefScreen::upClickedHandler()
{
    if (_mode == PREF_SELECT_MODE)
    {
        _menu->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_mode == PREF_EDIT_MODE)
    {
        if (_preference == BRIGHT_PREFERENCE)
        {
            brightUp();
        }
    }
}

void PrefScreen::downClickedHandler()
{
    if (_mode == PREF_SELECT_MODE)
    {
        _menu->focusDown();
        _scrollbar->scrollDown();
    }
    else if (_mode == PREF_EDIT_MODE)
    {
        if (_preference == BRIGHT_PREFERENCE)
        {
            brightDown();
        }
    }
}

void PrefScreen::leftClickedHandler()
{
    okClickedHandler();
}

void PrefScreen::rightClickedHandler()
{
    if (_mode == PREF_SELECT_MODE)
        openScreenByID(ID_SCREEN_MENU);
    else if (_mode == PREF_EDIT_MODE)
    {
        if (_preference == BRIGHT_PREFERENCE)
        {
            Util::setBrightness(_old_bright);
        }

        showPrefList();
    }
}

void PrefScreen::okClickedHandler()
{
    if (_mode == PREF_SELECT_MODE)
    {
        uint16_t id = _menu->getCurrentItemID();
        if (id == BRIGHT_ITEM_ID)
        {
            showBrightPref();
        }
    }
    else if (_mode == PREF_EDIT_MODE)
    {
        PrefManager pref;
        pref.setPref(STR_BRIGHT_PREF, String(_progress->getProgress()).c_str());
        showPrefList();
    }
}

void PrefScreen::brightUp()
{
    uint16_t cur_progress = _progress->getProgress();

    if (cur_progress < MAX_BRIGHT)
    {
        cur_progress += 10;
        _progress->setProgress(cur_progress);
        Util::setBrightness(cur_progress);
    }
}

void PrefScreen::brightDown()
{
    uint16_t cur_progress = _progress->getProgress();

    if (cur_progress > 20)
    {
        cur_progress -= 10;
        _progress->setProgress(cur_progress);
        Util::setBrightness(cur_progress);
    }
}

#pragma region EMPTY

void PrefScreen::loop()
{
}

void PrefScreen::doGUIWork()
{
}

void PrefScreen::okPressedHandler()
{
}

void PrefScreen::upPressedHandler()
{
}

void PrefScreen::downPressedHandler()
{
}

void PrefScreen::leftPressedHandler()
{
}

void PrefScreen::rightPressedHandler()
{
}

#pragma endregion EMPTY
