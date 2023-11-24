#include "HomeScreen.h"
#include "screen/resources/color.h"
#include "screen/resources/string.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/navbar/NavBar.h"

HomeScreen::HomeScreen(TFT_eSPI &tft) : IScreen{tft}
{
    // В конструкторі створються усі віджети, які повинні відображатися на екрані
    // під час першої відрисовки.

    // Будь-який віджет, що додається на екран, НЕ потрібно видаляти в ДЕСТРУКТОРІ.
    // Екран сам видалить макет, а макет видалить свої віджети.
    // Видаляти потрібно тільки екран у основному циклі. Див. файл ESP32GUI.

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
    left_txt->setText(STR_MENU);
    left_txt->setAlign(IWidget::ALIGN_CENTER);
    left_txt->setGravity(IWidget::GRAVITY_CENTER);
    left_txt->setTextColor(COLOR_WHITE);
    left_txt->setTextOffset(1);
    left_txt->setWidthToFit(); // Підігнати ширину віджету під текст.

    Label *mid_txt = left_txt->clone(2); // Клонування віджету left_txt.
    mid_txt->setText("");
    mid_txt->setWidthToFit();

    Label *right_txt = left_txt->clone(3);
    right_txt->setText("");
    right_txt->setWidthToFit();

    navbar->setWidgets(left_txt, mid_txt, right_txt); // Додати віджети на панель навігації.

    // Віджет для відображення тексту.
    // В даному прикладі використовується закешований покажчик.
    _time_txt = new Label(2, _tft);
    _time_txt->setText("00:00");
    _time_txt->setFontID(2);
    _time_txt->setAlign(IWidget::ALIGN_CENTER);
    _time_txt->setBackColor(COLOR_MAIN_BACK);
    _time_txt->setTextColor(COLOR_WHITE);
    _time_txt->setTextSize(2);
    _time_txt->setHeight(15 * 2);
    _time_txt->setWidth(_tft.width());

    // День тижня
    Label *day_of_week_txt = _time_txt->clone(3);
    day_of_week_txt->setText(MON_STR);
    day_of_week_txt->setTextSize(1);
    day_of_week_txt->setPos(0, 35);

    // Додати віджети до контейнера. widget_ID повинен бути унікальним в межах одного рівня макету.
    layout->addWidget(_time_txt);
    layout->addWidget(day_of_week_txt);
    layout->addWidget(navbar);
}

// Цикл без затримок. Сюди можна додати псевдопаралельні задачі,
// які повинні виконуватися в контексті поточного екрану, але не в контексті GUI.
void HomeScreen::loop()
{
}

// Фонова робота в межах частоти оновлення екрану, що не повинна бути привязана до натискання клавіш.
// Частоту оновлення можна налаштувати в IScreen.h.
void HomeScreen::doGUIWork()
{
}

// Обробка натискання кнопки "Вліво"
void HomeScreen::leftClickedHandler()
{
    openScreenByID(ID_SCREEN_MENU); // Зміна поточного екрану, екраном з ідентифікатором ID_SCREEN_MENU.
}

// Нижче знаходяться методи управління, які не використовуються на поточному екрані, але повинні мати реалізацію.
#pragma region "Not_used_here"

void HomeScreen::upClickedHandler()
{
}

void HomeScreen::downClickedHandler()
{
}

void HomeScreen::rightClickedHandler()
{
}

void HomeScreen::okClickedHandler()
{
}

void HomeScreen::okPressedHandler()
{
}

void HomeScreen::upPressedHandler()
{
}

void HomeScreen::downPressedHandler()
{
}

void HomeScreen::leftPressedHandler()
{
}

void HomeScreen::rightPressedHandler()
{
}

#pragma endregion "Not_used_here"
