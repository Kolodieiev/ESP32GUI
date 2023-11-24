#include "WatchScreen.h"
#include "screen/resources/color.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/spinbox/SpinBox.h"

WatchScreen::WatchScreen(TFT_eSPI &tft) : IScreen{tft}
{
    _watch.begin();
    _cur_date_time = _watch.getDateTime();

    SpinBox *hours = new SpinBox(ID_HOURS_SPIN, _tft);
    hours->setAlign(IWidget::ALIGN_CENTER);
    hours->setGravity(IWidget::GRAVITY_CENTER);
    hours->setHeight(34);
    hours->setWidth(4 * 10);
    hours->setBackColor(COLOR_BLACK);
    hours->setTextColor(COLOR_ORANGE);
    hours->setFocusBorderColor(COLOR_LIME);
    hours->setChangeBorderState(true);
    hours->setCornerRadius(5);
    hours->setMin(0);
    hours->setMax(23);
    hours->setValue(_cur_date_time.hour);
    hours->setPos((float)(_tft.width() - (hours->getWidth() * 2 + 5)) / 2, 3);

    SpinBox *minutes = hours->clone(ID_MINUTES_SPIN);
    minutes->setMin(0);
    minutes->setMax(59);
    minutes->setValue(_cur_date_time.minute);
    minutes->setPos(hours->getXPos() + hours->getWidth() + 5, hours->getYPos());

    SpinBox *day = hours->clone(ID_DAY_SPIN);
    day->setMin(1);
    day->setMax(31);
    day->setValue(_cur_date_time.dayOfMonth);
    day->setPos((float)(_tft.width() - (hours->getWidth() * 3 + 10)) / 2, hours->getYPos() + hours->getHeight() + 3);

    SpinBox *month = hours->clone(ID_MONTH_SPIN);
    month->setMin(1);
    month->setMax(12);
    month->setValue(_cur_date_time.month);
    month->setPos(day->getXPos() + day->getWidth() + 5, day->getYPos());

    SpinBox *year = hours->clone(ID_YEAR_SPIN);
    year->setMin(2020);
    year->setMax(3000);
    year->setValue(_cur_date_time.year);
    year->setPos(month->getXPos() + month->getWidth() + 5, day->getYPos());

    EmptyLayout *layout = new EmptyLayout(1, _tft);
    layout->setBackColor(COLOR_MAIN_BACK);
    layout->setWidth(_tft.width());
    layout->setHeight(_tft.height());

    hours->setFocus();
    
    layout->addWidget(hours);
    layout->addWidget(minutes);
    layout->addWidget(day);
    layout->addWidget(month);
    layout->addWidget(year);

    setLayout(layout);
}

// void WatchScreen::show()
// {
//     getLayout()->onDraw();
//     SpinBox *hours = (SpinBox *)getLayout()->getWidget(_curent_focus_pos);
//     hours->setFocus();
// }

void WatchScreen::upClickedHandler()
{
    SpinBox *spinBox = (SpinBox *)getLayout()->getWidget(_curent_focus_pos);
    spinBox->up();

    changeCurDateTime(spinBox->getID(), spinBox->getValue());
}

void WatchScreen::downClickedHandler()
{
    SpinBox *spinBox = (SpinBox *)getLayout()->getWidget(_curent_focus_pos);
    spinBox->down();

    changeCurDateTime(spinBox->getID(), spinBox->getValue());
}

void WatchScreen::changeCurDateTime(uint16_t spin_id, uint16_t value)
{
    if (spin_id == ID_HOURS_SPIN)
        _cur_date_time.hour = value;
    else if (spin_id == ID_MINUTES_SPIN)
        _cur_date_time.minute = value;
    else if (spin_id == ID_DAY_SPIN)
        _cur_date_time.dayOfMonth = value;
    else if (spin_id == ID_MONTH_SPIN)
        _cur_date_time.month = value;
    else if (spin_id == ID_YEAR_SPIN)
        _cur_date_time.year = value;
}

void WatchScreen::leftClickedHandler()
{
    SpinBox *spinBox = (SpinBox *)getLayout()->getWidget(_curent_focus_pos);
    spinBox->removeFocus();

    if (_curent_focus_pos > 0)
        _curent_focus_pos--;
    else
        _curent_focus_pos = getLayout()->getSize() - 1;

    spinBox = (SpinBox *)getLayout()->getWidget(_curent_focus_pos);
    spinBox->setFocus();
}

void WatchScreen::rightClickedHandler()
{
    SpinBox *spinBox = (SpinBox *)getLayout()->getWidget(_curent_focus_pos);
    spinBox->removeFocus();

    if (_curent_focus_pos == getLayout()->getSize() - 1)
        _curent_focus_pos = 0;
    else
        _curent_focus_pos++;

    spinBox = (SpinBox *)getLayout()->getWidget(_curent_focus_pos);
    spinBox->setFocus();
}

void WatchScreen::leftPressedHandler()
{
    okPressedHandler();
}

void WatchScreen::okPressedHandler()
{
    _watch.setDateTime(_cur_date_time);
    openScreenByID(ID_SCREEN_MENU);
}

void WatchScreen::rightPressedHandler()
{
    openScreenByID(ID_SCREEN_MENU);
}

#pragma region EMPTY

void WatchScreen::okClickedHandler()
{
}

void WatchScreen::loop()
{
}

void WatchScreen::doGUIWork()
{
}

void WatchScreen::upPressedHandler()
{
}

void WatchScreen::downPressedHandler()
{
}

#pragma endregion EMPTY
