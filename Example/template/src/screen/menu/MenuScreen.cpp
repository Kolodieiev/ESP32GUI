#include <pgmspace.h>
#include "MenuScreen.h"
#include "screen/resources/color.h"
#include "screen/resources/string.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/text/Focusable.h"
#include "lib/GUI/widget/text/Label.h"
#include "lib/GUI/widget/navbar/NavBar.h"

MenuScreen::MenuScreen(TFT_eSPI &tft) : IScreen{tft}
{
    // Створення основного макету екрану.
    EmptyLayout *layout = new EmptyLayout(1, _tft);
    layout->setBackColor(COLOR_MAIN_BACK);
    layout->setWidth(_tft.width());
    layout->setHeight(_tft.height());

    // Встановити цей макет основним для поточного екрану.
    setLayout(layout);

    // Створення панелі навігації.
    NavBar *navbar = new NavBar(1, _tft);
    navbar->setBackColor(COLOR_NAV_PANEL_BACK);
    navbar->setHeight(NAVBAR_HEIGHT);
    navbar->setWidth(_tft.width());
    navbar->setPos(0, _tft.height() - NAVBAR_HEIGHT);

    // Створення тексту для панелі навігації.
    Label *left_txt = new Label(1, _tft);
    left_txt->setText(STR_SELECT);
    left_txt->setAlign(IWidget::ALIGN_CENTER);
    left_txt->setGravity(IWidget::GRAVITY_CENTER);
    left_txt->setTextColor(COLOR_WHITE);
    left_txt->setTextOffset(1);
    left_txt->setWidthToFit(); // Підігнати ширину віджету під текст.

    Label *mid_txt = left_txt->clone(2); // Клонування віджету left_txt.
    mid_txt->setText("");
    mid_txt->setWidthToFit();

    Label *right_txt = left_txt->clone(3);
    right_txt->setText(STR_BACK);
    right_txt->setWidthToFit();

    navbar->setWidgets(left_txt, mid_txt, right_txt); // Додати віджети на панель навігації.

    // Створення меню
    _menu = new FixedMenu(2, _tft);
    _menu->setBackColor(COLOR_MENU_ITEM);
    _menu->setWidth(_tft.width() - SCROLLBAR_WIDTH - 2);
    _menu->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _menu->setItemsHeight((_tft.height() - NAVBAR_HEIGHT) / 3);

    // Створення першого пункту меню.
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

    Focusable *files_item = music_item->clone(2); // Клонування параметрів першого пункту меню.
    files_item->setText(STR_FILES);               // Зміна тексту для другого пункту.

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

    // Додавання віджетів пунктів меню до контейнера меню.
    _menu->addWidget(music_item);
    _menu->addWidget(files_item);
    _menu->addWidget(watch_item);
    _menu->addWidget(pref_item);
    _menu->addWidget(import_item);
    _menu->addWidget(export_item);
    _menu->addWidget(update_item);

    // Створення полоси прокрутки для меню.
    _scrollbar = new ScrollBar(3, _tft);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _scrollbar->setPos(_tft.width() - SCROLLBAR_WIDTH, 0);
    _scrollbar->setMax(_menu->getSize()); // Задати кількість позицій полоси прокрутки.

    // Черга додавання не має значення, якщо віджети не перекривають один одного.
    // Інакше, останній віджет буде відрисовано поверх попередніх.
    layout->addWidget(_menu);
    layout->addWidget(navbar);
    layout->addWidget(_scrollbar);
}

// При натисканні кнопки вибору, відбувається обробка фокусу меню.
void MenuScreen::okClickedHandler()
{
    uint16_t ID = _menu->getCurrentItemID(); // Отримати ідентифікатор віджету, на якому встановлено фокус.

    if (ID == 1)
        openScreenByID(ID_SCREEN_HOME); // В даному випадку відбувається повернення на домашній екран.
                                        // Дивись mp3 плеєр, якщо потрібен більш складний приклад.
    // if (ID == 2)
    //     openScreenByID(ID_SCREEN_HOME);
}

// Зміна поточного елементу меню та переключення позиції полоси прокрутки.
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