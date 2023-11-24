#include "DynamicMenu.h"

DynamicMenu::DynamicMenu(IItemsLoader *loader, uint16_t widget_ID, TFT_eSPI &tft) : Menu(widget_ID, tft), _loader{loader} {}

bool DynamicMenu::focusUp()
{
    if (!_widgets.empty())
    {
        if (_cur_focus_pos)
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
        else
        {
            std::vector<IWidget *> temp_vector;

            if (_loader->loadPrev(temp_vector, getItemsNumberOnScreen(), _widgets[_cur_focus_pos]->getID()))
            {
                for (uint16_t i{0}; i < _widgets.size(); ++i)
                    delete _widgets[i];

                _widgets = temp_vector;

                _first_item_index = 0;
                drawItems(_first_item_index, getCyclesCount());
                _cur_focus_pos = _widgets.size() - 1;

                Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
                item->setIsTicker(_draw_ticker);
                item->setFocus();

                return true;
            }
        }
    }

    return false;
}

bool DynamicMenu::focusDown()
{
    if (!_widgets.empty())
    {
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
        else
        {
            std::vector<IWidget *> temp_vector;

            if (_loader->loadNext(temp_vector, getItemsNumberOnScreen(), _widgets[_cur_focus_pos]->getID()))
            {
                for (uint16_t i{0}; i < _widgets.size(); ++i)
                    delete _widgets[i];

                _widgets = temp_vector;

                _first_item_index = 0;
                drawItems(_first_item_index, getCyclesCount());
                _cur_focus_pos = _first_item_index;

                Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
                item->setIsTicker(_draw_ticker);
                item->setFocus();

                return true;
            }
        }
    }

    return false;
}

uint16_t DynamicMenu::getItemsNumberOnScreen() const
{
    return (float)_height / _item_height;
}

DynamicMenu *DynamicMenu::clone(uint16_t id) const
{
    DynamicMenu *clone = new DynamicMenu(_loader, id, IWidgetContainer::_tft);

    for (uint16_t i{0}; i < _widgets.size(); ++i)
    {
        IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
        clone->addWidget(item);
    }

    return clone;
}
