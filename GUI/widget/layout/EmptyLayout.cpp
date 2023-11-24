#include "EmptyLayout.h"

EmptyLayout::EmptyLayout(uint16_t widget_ID, TFT_eSPI &tft) : IWidgetContainer(widget_ID, tft) {}

EmptyLayout::~EmptyLayout() {}

void EmptyLayout::onDraw()
{
    if (!_is_changed)
    {
        for (uint16_t i{0}; i < _widgets.size(); ++i)
            _widgets[i]->onDraw();
    }
    else
    {
        _is_changed = false;
        clear();

        for (uint16_t i{0}; i < _widgets.size(); ++i)
            _widgets[i]->forcedOnDraw();
    }
}

EmptyLayout *EmptyLayout::clone(uint16_t id) const
{
    EmptyLayout *clone = new EmptyLayout(id, IWidgetContainer::_tft);

    for (uint16_t i{0}; i < _widgets.size(); ++i)
    {
        IWidget *item = _widgets[i]->clone(_widgets[i]->getID());
        clone->addWidget(item);
    }

    return clone;
}
