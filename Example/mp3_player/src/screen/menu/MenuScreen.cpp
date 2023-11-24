#include <pgmspace.h>
#include "MenuScreen.h"
#include "screen/resources/color.h"
#include "screen/resources/string.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/text/Focusable.h"
#include "lib/GUI/widget/text/Label.h"
#include "lib/GUI/widget/navbar/NavBar.h"

MenuScreen::MenuScreen(TFT_eSPI &tft): IScreen{tft}
{
    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    NavBar *navbar = _creator.getNavbar(1, STR_SELECT, "", STR_BACK);
    layout->addWidget(navbar);

    _menu = new FixedMenu(3, _tft);
    _menu->setBackColor(COLOR_MENU_ITEM);
    _menu->setWidth(_tft.width() - SCROLLBAR_WIDTH - 2);
    _menu->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _menu->setItemsHeight((_tft.height() - NAVBAR_HEIGHT) / 3);

    Focusable *music_item = new Focusable(1, _tft);
    music_item->setText(STR_MUSIC);
    music_item->setGravity(IWidget::GRAVITY_CENTER);
    music_item->setTextOffset(3);
    music_item->setFocusBorderColor(COLOR_LIME);
    music_item->setChangeBorderState(true);
    music_item->setFocusBackColor(COLOR_FOCUS_BACK);
    music_item->setChangeBackState(true);
    music_item->setTextColor(COLOR_WHITE);
    music_item->setBackColor(COLOR_MENU_ITEM);

    Focusable *files_item = music_item->clone(2);
    files_item->setText(STR_FILES);

    Focusable *watch_item = music_item->clone(3);
    watch_item->setText(STR_WATCH);

    Focusable *pref_item = music_item->clone(4);
    pref_item->setText(STR_PREF);

    Focusable *import_item = music_item->clone(5);
    import_item->setText(STR_IMPORT);

    Focusable *export_item = music_item->clone(6);
    export_item->setText(STR_EXPORT);

    Focusable *update_item = music_item->clone(7);
    update_item->setText(STR_FIRMWARE);

    _menu->addWidget(music_item);
    _menu->addWidget(files_item);
    _menu->addWidget(watch_item);
    _menu->addWidget(pref_item);
    _menu->addWidget(import_item);
    _menu->addWidget(export_item);
    _menu->addWidget(update_item);

    _scrollbar = new ScrollBar(2, _tft);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _scrollbar->setPos(_tft.width() - SCROLLBAR_WIDTH, 0);
    _scrollbar->setMax(_menu->getSize());

    layout->addWidget(_menu);
    layout->addWidget(_scrollbar);
}

void MenuScreen::okClickedHandler()
{
    uint16_t ID = _menu->getCurrentItemID();

    if (ID == 1)
        openScreenByID(ID_SCREEN_MUSIC);
    else if (ID == 2)
        openScreenByID(ID_SCREEN_FILES);
    else if (ID == 3)
        openScreenByID(ID_SCREEN_WATCH);
    else if (ID == 4)
        openScreenByID(ID_SCREEN_PREF);
    else if (ID == 5)
        openScreenByID(ID_SCREEN_IMPORT);
    else if (ID == 6)
        openScreenByID(ID_SCREEN_EXPORT);
    else if (ID == 7)
        openScreenByID(ID_SCREEN_UPDATE);
}

void MenuScreen::upClickedHandler()
{
    _menu->focusUp();
    _scrollbar->scrollUp();
}

void MenuScreen::downClickedHandler()
{
    _menu->focusDown();
    _scrollbar->scrollDown();
}

void MenuScreen::leftClickedHandler()
{
    okClickedHandler();
}

void MenuScreen::rightClickedHandler()
{
    openScreenByID(ID_SCREEN_HOME);
}

void MenuScreen::loop()
{
}

void MenuScreen::doGUIWork()
{
}

void MenuScreen::okPressedHandler()
{
}

void MenuScreen::upPressedHandler()
{
}

void MenuScreen::downPressedHandler()
{
}

void MenuScreen::leftPressedHandler()
{
}

void MenuScreen::rightPressedHandler()
{
}