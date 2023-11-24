#include "SpinBox.h"

SpinBox::SpinBox(uint16_t widget_ID, TFT_eSPI &tft) : Focusable(widget_ID, tft) {}

SpinBox *SpinBox::clone(uint16_t id) const
{
    SpinBox *clone = new SpinBox(*this);
    clone->_id = id;
    return clone;
}

void SpinBox::setMin(const int32_t min)
{
    _min = min;

    if (_value < _min)
    {
        _value = _min;
        setText(String(_value));
    }

    _is_changed = true;
}

void SpinBox::setMax(const int32_t max)
{
    _max = max;
    if (_value > _max)
    {
        _value = _max;
        setText(String(_value));
    }

    _is_changed = true;
}

void SpinBox::setValue(const int32_t value)
{
    if (value < _min)
        _value = _min;
    else if (value > _max)
        _value = _max;
    else
        _value = value;

    setText(String(_value));
}

void SpinBox::up()
{
    if (_value < _max)
        _value++;
    else
        _value = _min;

    setText(String(_value));
}

void SpinBox::down()
{
    if (_value > _min)
        _value--;
    else
        _value = _max;

    setText(String(_value));
}
