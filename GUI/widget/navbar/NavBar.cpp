#include "NavBar.h"

NavBar::NavBar(uint16_t widget_ID, TFT_eSPI &tft) : IWidget(widget_ID, tft)
{
    _tft = tft;
}

NavBar::~NavBar()
{
    delete _first;
    delete _middle;
    delete _last;
}

void NavBar::setWidgets(IWidget *first, IWidget *middle, IWidget *last)
{
    if (first == nullptr || middle == nullptr || last == nullptr)
    {
        log_e("Спроба передати NULL-об'єкт.");
        return;
    }

    delete _first;
    delete _middle;
    delete _last;

    _first = first;
    _middle = middle;
    _last = last;

    _is_changed = true;
}

void NavBar::onDraw()
{
    if (_is_changed)
    {
        clear();

        if (_first == nullptr || _middle == nullptr || _last == nullptr)
        {
            return;
        }

        _first->setPos(_x_pos, _y_pos);
        _first->setBackColor(_back_color);
        _first->setHeight(_height);
        _first->onDraw();

        _middle->setPos(_x_pos + (_width - _middle->getWidth()) / 2, _y_pos);
        _middle->setBackColor(_back_color);
        _middle->setHeight(_height);
        _middle->onDraw();

        _last->setPos(_x_pos + _width - _last->getWidth(), _y_pos);
        _last->setBackColor(_back_color);
        _last->setHeight(_height);
        _last->onDraw();

        _is_changed = false;
    }
}

NavBar *NavBar::clone(uint16_t id) const
{
    if (_first == nullptr || _middle == nullptr || _last == nullptr)
    {
        log_e("*IWidget вказує на nullptr.");
        return nullptr;
    }

    NavBar *clone = new NavBar(*this);
    clone->_id = id;

    clone->setWidgets(
        _first->clone(_first->getID()),
        _middle->clone(_middle->getID()),
        _last->clone(_last->getID()));

    return clone;
}
