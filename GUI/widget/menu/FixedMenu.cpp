#include "FixedMenu.h"

FixedMenu::FixedMenu(uint16_t widget_ID, TFT_eSPI &tft) : Menu(widget_ID, tft) {}

FixedMenu::~FixedMenu()
{
}

bool FixedMenu::focusUp()
{
    if (!_widgets.empty())
        if (_cur_focus_pos > 0)
        {
            Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
            item->setIsTicker(false);
            item->removeFocus();

            _cur_focus_pos--;

            uint16_t cycles_count = getCyclesCount();

            if (_cur_focus_pos < _first_item_index)
            {
                _first_item_index--;
                drawItems(_first_item_index, cycles_count);
            }

            item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
            item->setIsTicker(_draw_ticker);
            item->setFocus();

            return true;
        }

    return false;
}

bool FixedMenu::focusDown()
{
    if (!_widgets.empty())
        if (_cur_focus_pos < _widgets.size() - 1)
        {
            Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
            item->setIsTicker(false);
            item->removeFocus();

            _cur_focus_pos++;

            uint16_t cycles_count = getCyclesCount();

            if (_cur_focus_pos > _first_item_index + cycles_count - 1)
            {
                _first_item_index++;

                drawItems(_first_item_index, cycles_count);
            }

            item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
            item->setIsTicker(_draw_ticker);
            item->setFocus();

            return true;
        }

    return false;
}

FixedMenu *FixedMenu::clone(uint16_t id) const
{
    FixedMenu *clone = new FixedMenu(id, IWidgetContainer::_tft);
    
    for (uint16_t i{0}; i < _widgets.size(); ++i)
    {
        IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
        clone->addWidget(item);
    }

    return clone;
}