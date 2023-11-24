#include <pgmspace.h>
#include "HomeScreen.h"
#include "screen/resources/color.h"
#include "screen/resources/string.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/navbar/NavBar.h"
#include "lib/GUI/util/Util.h"

HomeScreen::HomeScreen(TFT_eSPI &tft) : IScreen{tft}
{
    _watch.begin();
    // Основний макет екрану. Буде автоматично очищений разом з екраном.
    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);
    // Панель навігації
    NavBar *navbar = _creator.getNavbar(4, STR_MENU, STR_TORCH, "");

    // Години
    _time_txt = new Label(7, _tft);
    _time_txt->setFontID(2);
    _time_txt->setAlign(IWidget::ALIGN_CENTER);
    _time_txt->setBackColor(COLOR_MAIN_BACK);
    _time_txt->setTextColor(COLOR_WHITE);
    _time_txt->setTextSize(2);
    _time_txt->setHeight(15 * 2);
    _time_txt->setWidth(_tft.width());
    // Дата
    _date_txt = _time_txt->clone(8);
    _date_txt->setPos(0, 30);
    _date_txt->setTextSize(1);
    _date_txt->setHeight(15);
    // День тижня
    _day_of_week_txt = _date_txt->clone(9);
    _day_of_week_txt->setText(MON_STR);
    _day_of_week_txt->setPos(0, 45);

    _voltage_txt = _day_of_week_txt->clone(10);
    _voltage_txt->setText(F("0.00"));
    _voltage_txt->setWidthToFit(3);
    _voltage_txt->setPos(_tft.width() - _voltage_txt->getWidth() - 3, 0);

    // Додати віджети до контейнера. widget_ID повинен бути унікальним в межах одного рівня макету.
    layout->addWidget(_time_txt);
    layout->addWidget(_date_txt);
    layout->addWidget(_day_of_week_txt);
    layout->addWidget(_voltage_txt);
    layout->addWidget(navbar);

    // Оновити дані годинника на екрані
    updTimeOnScreen();
    // Оновити дані напруги
    updVoltageOnScreen();
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
    if ((millis() - _last_time_update) > UPD_TIME_INTERVAL)
    {
        updTimeOnScreen();
        updVoltageOnScreen();
        _last_time_update = millis();
    }
}

// Обробка натискання кнопки "Вгору".
void HomeScreen::upClickedHandler()
{
    openScreenByID(ID_SCREEN_FLASH); // Зміна поточного екрану, на екран з ідентифікатором ID_SCREEN_FLASH.
}

// Обробка натискання кнопки "Вліво"
void HomeScreen::leftClickedHandler()
{
    openScreenByID(ID_SCREEN_MENU);
}

void HomeScreen::updTimeOnScreen()
{
    _cur_date_time = _watch.getDateTime();

    if (_cur_date_time != _temp_date_time)
    {
        _temp_date_time = _cur_date_time;

        String tempStr;

        // time
        if (_temp_date_time.hour < 10)
            tempStr = "0" + String(_temp_date_time.hour) + ":";
        else
            tempStr = String(_temp_date_time.hour) + ":";

        if (_temp_date_time.minute < 10)
            tempStr += "0" + String(_temp_date_time.minute);
        else
            tempStr += String(_temp_date_time.minute);

        _time_txt->setText(tempStr);

        // date
        if (_temp_date_time.dayOfMonth < 10)
            tempStr = "0" + String(_temp_date_time.dayOfMonth) + ".";
        else
            tempStr = String(_temp_date_time.dayOfMonth) + ".";

        if (_temp_date_time.month < 10)
            tempStr += "0" + String(_temp_date_time.month) + ".";
        else
            tempStr += String(_temp_date_time.month) + ".";

        tempStr += String(_temp_date_time.year);

        _date_txt->setText(tempStr);

        _day_of_week_txt->setText(DAY_OF_WEEK[_temp_date_time.dayOfWeek()]);

        _time_txt->setWidthToFit();
        _date_txt->setWidthToFit();
        _day_of_week_txt->setWidthToFit();
    }
}

void HomeScreen::updVoltageOnScreen()
{
    const float K{0.001835};

    float bat_voltage{0};

    pinMode(PIN_VOLT_MEASH_READ, INPUT);
    for (uint8_t i{0}; i < 7; ++i)
        bat_voltage += analogRead(PIN_VOLT_MEASH_READ);
    pinMode(PIN_VOLT_MEASH_READ, OUTPUT);

    bat_voltage /= 7;

    bat_voltage *= K;
    String volt_str = String(bat_voltage);

    _voltage_txt->setText(volt_str);
}

// Нижче знаходяться методи управління, які не використовуються на даному екрані, але повинні мати реалізацію.
#pragma region Not_used_here

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

#pragma endregion Not_used_here