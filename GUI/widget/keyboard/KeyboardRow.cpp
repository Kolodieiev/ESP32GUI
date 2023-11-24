#include "KeyboardRow.h"

KeyboardRow::KeyboardRow(uint16_t widget_ID, TFT_eSPI &tft) : IWidgetContainer(widget_ID, tft) {}

KeyboardRow *KeyboardRow::clone(uint16_t id) const
{
    KeyboardRow *clone = new KeyboardRow(id, IWidgetContainer::_tft);

    for (uint16_t i{0}; i < _widgets.size(); ++i)
    {
        IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
        clone->addWidget(item);
    }

    return clone;
}

uint16_t KeyboardRow::getCurrentBtnID() const
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної кнопки.");
        return 0;
    }

    if (_cur_focus_pos > _widgets.size() - 1)
    {
        log_e("Кнопку не знайдено.");
        return 0;
    }

    return _widgets[_cur_focus_pos]->getID();
}

String KeyboardRow::getCurrentBtnTxt() const
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної кнопки.");
        return "";
    }

    if (_cur_focus_pos > _widgets.size() - 1)
    {
        log_e("Кнопку не знайдено.");
        return "";
    }

    Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);

    if (item == nullptr)
    {
        log_e("Кнопку не знайдено.");
        return "";
    }

    return item->getText();
}

bool KeyboardRow::focusUp()
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної кнопки.");
        return false;
    }

    if (_cur_focus_pos > 0)
    {
        Focusable *btn = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);

        if (btn == nullptr)
        {
            log_e("Кнопку не знайдено.");
            return false;
        }

        btn->removeFocus();

        _cur_focus_pos--;

        btn = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
        btn->setFocus();

        return true;
    }

    return false;
}

bool KeyboardRow::focusDown()
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної кнопки.");
        return false;
    }

    if (_cur_focus_pos < _widgets.size() - 1)
    {
        Focusable *btn = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);

        if (btn == nullptr)
        {
            log_e("Кнопку не знайдено.");
            return false;
        }

        btn->removeFocus();

        _cur_focus_pos++;

        btn = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
        btn->setFocus();

        return true;
    }

    return false;
}

void KeyboardRow::setFocus(uint16_t pos)
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної кнопки.");
        return;
    }

    Focusable *btn = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);

    if (btn == nullptr)
    {
        log_e("Кнопку не знайдено.");
        return;
    }

    btn->removeFocus();

    if (pos > _widgets.size() - 1)
        _cur_focus_pos = _widgets.size() - 1;
    else
        _cur_focus_pos = pos;

    btn = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
    btn->setFocus();
}

void KeyboardRow::removeFocus()
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної кнопки.");
        return;
    }

    Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);

    if (item == nullptr)
    {
        log_e("Кнопку не знайдено.");
        return;
    }

    item->removeFocus();

    _cur_focus_pos = 0;
}

void KeyboardRow::onDraw()
{
    if (!_is_changed)
    {
        for (uint16_t i{0}; i < _widgets.size(); ++i)
            _widgets[i]->onDraw();
    }
    else
    {
        clear();

        uint16_t step = (_width - _btn_width * _widgets.size()) / (_widgets.size() + 1);

        uint16_t x = _x_pos + step;

        uint16_t y = _y_pos + (_height - _btn_height) / 2;

        for (uint16_t i{0}; i < _widgets.size(); ++i)
        {
            _widgets[i]->setPos(x, y);
            _widgets[i]->setWidth(_btn_width);
            _widgets[i]->setHeight(_btn_height);
            _widgets[i]->onDraw();

            x += _btn_width + step;
        }

        _is_changed = false;
    }
}
