#include "IWidgetContainer.h"

IWidgetContainer::IWidgetContainer(uint16_t widget_ID, TFT_eSPI &tft) : IWidget(widget_ID, tft) {}

IWidgetContainer::~IWidgetContainer()
{
    deleteWidgets();
}

bool IWidgetContainer::addWidget(IWidget *widget_ptr)
{
    if (widget_ptr == nullptr)
    {
        log_e("*IWidget не може бути NULL.");
        return false;
    }

    uint16_t search_ID = widget_ptr->getID();

    if (!search_ID)
    {
        log_e("WidgetID повинен бути > 0.");
        return false;
    }

    for (uint16_t i{0}; i < _widgets.size(); ++i)
        if (_widgets[i]->getID() == search_ID)
        {
            log_e("WidgetID повинен бути унікальним.");
            return false;
        }

    _widgets.push_back(widget_ptr);

    return true;
}

bool IWidgetContainer::deleteWidgetByID(uint16_t widget_ID)
{
    auto widgetsIt{_widgets.begin()};

    for (uint16_t i{0}; i < _widgets.size();  ++i)
    {
        if (_widgets[i]->getID() == widget_ID)
        {
            delete _widgets[i];
            _widgets.erase(widgetsIt + i);
            _is_changed = true;
            return true;
        }
    }

    return false;
}

IWidget *IWidgetContainer::findWidgetByID(uint16_t widget_ID) const
{
    for (uint16_t i{0}; i < _widgets.size();  ++i)
    {
        if (_widgets[i]->getID() == widget_ID)
            return _widgets[i];
    }

    return nullptr;
}

IWidget *IWidgetContainer::getWidget(uint16_t widget_pos) const
{
    if (_widgets.size() > widget_pos)
        return _widgets[widget_pos];

    return nullptr;
}

uint16_t IWidgetContainer::getSize() const
{
    return _widgets.size();
}

void IWidgetContainer::deleteWidgets()
{
    for (uint16_t i{0}; i < _widgets.size();  ++i)
        delete _widgets[i];

    _widgets.clear();
}
