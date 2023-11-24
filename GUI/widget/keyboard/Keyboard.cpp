#include "Keyboard.h"
#include "KeyboardRow.h"

Keyboard::Keyboard(uint16_t widget_ID, TFT_eSPI &tft) : IWidgetContainer(widget_ID, tft) {}

Keyboard *Keyboard::clone(uint16_t id) const
{
    Keyboard *clone = new Keyboard(id, IWidgetContainer::_tft);

    for (uint16_t i{0}; i < _widgets.size(); ++i)
    {
        IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
        clone->addWidget(item);
    }

    return clone;
}

uint16_t Keyboard::getCurrentBtnID() const
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної KeyboardRow.");
        return 0;
    }

    KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

    if (row == nullptr)
    {
        log_e("KeyboardRow не знайдено.");
        return 0;
    }

    return row->getCurrentBtnID();
}

String Keyboard::getCurrentBtnTxt() const
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної KeyboardRow.");
        return "";
    }

    KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

    if (row == nullptr)
    {
        log_e("KeyboardRow не знайдено.");
        return "";
    }

    return row->getCurrentBtnTxt();
}

void Keyboard::focusUp()
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної KeyboardRow.");
        return;
    }

    KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

    if (row == nullptr)
    {
        log_e("KeyboardRow не знайдено.");
        return;
    }

    uint16_t focusPos = row->getCurFocusPos();

    row->removeFocus();

    if (_cur_focus_row_pos > 0)
        _cur_focus_row_pos--;
    else
        _cur_focus_row_pos = _widgets.size() - 1;

    row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);
    row->setFocus(focusPos);
}

void Keyboard::focusDown()
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної KeyboardRow.");
        return;
    }

    KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

    if (row == nullptr)
    {
        log_e("KeyboardRow не знайдено.");
        return;
    }

    uint16_t focusPos = row->getCurFocusPos();

    row->removeFocus();

    if (_cur_focus_row_pos < _widgets.size() - 1)
        _cur_focus_row_pos++;
    else
        _cur_focus_row_pos = 0;

    row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);
    row->setFocus(focusPos);
}

void Keyboard::focusLeft()
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної KeyboardRow.");
        return;
    }

    KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

    if (row == nullptr)
    {
        log_e("KeyboardRow не знайдено.");
        return;
    }

    if (row->focusUp())
        return;

    row->removeFocus();
    row->setFocus(row->getSize() - 1);
}

void Keyboard::focusRight()
{
    if (_widgets.size() == 0)
    {
        log_e("Не додано жодної KeyboardRow.");
        return;
    }

    KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[_cur_focus_row_pos]);

    if (row == nullptr)
    {
        log_e("KeyboardRow не знайдено.");
        return;
    }

    if (row->focusDown())
        return;

    row->removeFocus();
    row->setFocus(0);
}

void Keyboard::onDraw()
{
    if (!_is_changed)
    {
        for (uint16_t i{0}; i < _widgets.size(); ++i)
            _widgets[i]->onDraw();
    }
    else
    {
        clear();

        if (_first_drawing)
        {
            if (!_widgets.empty())
            {
                KeyboardRow *row = reinterpret_cast<KeyboardRow *>(_widgets[0]);
                if (row != nullptr)
                    row->setFocus(0);
                else
                    log_e("KeyboardRow не знайдено.");
            }

            _first_drawing = false;
        }

        uint16_t x = _x_pos;
        uint16_t y = _y_pos;

        for (uint16_t i{0}; i < _widgets.size(); ++i)
        {
            _widgets[i]->setPos(x, y);
            _widgets[i]->setWidth(_width);
            _widgets[i]->onDraw();
            y += _widgets[i]->getHeight();
        }

        _is_changed = false;
    }
}