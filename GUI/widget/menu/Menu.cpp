#include "Menu.h"

Menu::Menu(uint16_t widget_ID, TFT_eSPI &tft) : IWidgetContainer(widget_ID, tft) {}

Menu::~Menu() {}

Focusable *Menu::findItemByID(uint16_t itemID) const
{
    IWidget *widget = findWidgetByID(itemID);

    if (widget == nullptr)
        return nullptr;
    else
        return reinterpret_cast<Focusable *>(widget);
}

void Menu::deleteWidgets()
{
    _first_item_index = 0;
    _cur_focus_pos = 0;

    IWidgetContainer::deleteWidgets();
}

uint16_t Menu::getCurrentItemID() const
{
    if (_widgets.size() == 0)
        return 0;

    return _widgets[_cur_focus_pos]->getID();
}

String Menu::getCurrentItemText() const
{
    if (_widgets.size() == 0)
        return "";

    Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
    return item->getText();
}

void Menu::onDraw()
{
    if (!_is_changed)
    {
        for (uint16_t i{_first_item_index}; i < _first_item_index + getCyclesCount(); ++i)
            _widgets[i]->onDraw();
    }
    else
    {
        _is_changed = false;

        if (_widgets.size() == 0)
            return;

        uint16_t cyclesCount = getCyclesCount();

        if (_first_item_index >= _widgets.size())
            _first_item_index = _widgets.size() - 1;

        if (_cur_focus_pos >= _widgets.size())
            _cur_focus_pos = _widgets.size() - 1;

        Focusable *item = reinterpret_cast<Focusable *>(_widgets[_cur_focus_pos]);
        item->setIsTicker(_draw_ticker);
        item->setFocus();

        drawItems(_first_item_index, cyclesCount);
    }
}

void Menu::drawItems(uint16_t start, uint16_t count)
{
    clear();

    if (_widgets.size() == 0)
        return;

    uint16_t itemXPos = _x_pos + 1;
    uint16_t itemYPos = _y_pos + 1;

    for (uint16_t i{start}; i < start + count; ++i)
    {
        _widgets[i]->setPos(itemXPos, itemYPos);

        if (_orientation == ORIENTATION_HORIZONTAL)
        {
            _widgets[i]->setHeight(_item_height);
            _widgets[i]->setWidth(_width - 2);
            itemYPos += _item_height;
        }
        else
        {
            _widgets[i]->setHeight(_height - 2);
            _widgets[i]->setWidth(_item_width);
            itemXPos += _item_width;
        }

        _widgets[i]->onDraw();
    }
}

uint16_t Menu::getCyclesCount() const
{
    uint16_t cyclesCount;

    if (_orientation == ORIENTATION_HORIZONTAL)
        cyclesCount = (float)_height / _item_height;
    else
        cyclesCount = (float)_width / _item_width;

    uint16_t itemsToEndNum = _widgets.size() - _first_item_index;

    if (cyclesCount > itemsToEndNum)
        cyclesCount = itemsToEndNum;

    return cyclesCount;
}
