#include "Focusable.h"

Focusable::Focusable(uint16_t widget_ID, TFT_eSPI &tft) : Label(widget_ID, tft) {}

Focusable *Focusable::clone(uint16_t id) const
{
    Focusable *clone = new Focusable(*this);
    clone->_id = id;
    return clone;
}

void Focusable::setFocus()
{
    if (_has_focus)
        return;

    _has_focus = true;

    if (_need_change_border)
    {
        _old_border_state = _has_border;
        _has_border = true;

        _old_border_color = _border_color;
        _border_color = _focus_border_color;
    }

    if (_need_change_back)
    {
        _old_back_color = _back_color;
        _back_color = _focus_back_color;
    }

    _is_changed = true;
}

void Focusable::removeFocus()
{
    if (!_has_focus)
        return;

    _has_focus = false;

    if (_need_change_border)
    {
        _has_border = _old_border_state;
        _border_color = _old_border_color;
    }

    if (_need_change_back)
        _back_color = _old_back_color;

    _is_changed = true;
}
