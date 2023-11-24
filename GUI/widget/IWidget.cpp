#include "IWidget.h"

IWidget::IWidget(uint16_t widget_ID, TFT_eSPI &tft) : _id{widget_ID}, _tft{tft} {}

IWidget::~IWidget() {}

void IWidget::forcedOnDraw()
{
    _is_changed = true;
    onDraw();
}

void IWidget::setPos(uint16_t x, uint16_t y)
{
    _x_pos = x;
    _y_pos = y;
    _is_changed = true;
}

void IWidget::setWidth(uint16_t width)
{
    _width = width;
    _is_changed = true;
}

void IWidget::setHeight(uint16_t height)
{
    _height = height;
    _is_changed = true;
}

void IWidget::setBackColor(uint16_t back_color)
{
    _back_color = back_color;
    _is_changed = true;
}

void IWidget::setCornerRadius(const uint8_t radius)
{
    _corner_radius = radius;
    _is_changed = true;
}

void IWidget::setBorder(const bool state)
{
    _has_border = state;
    _is_changed = true;
}

void IWidget::setBorderColor(uint16_t color)
{
    _border_color = color;
    _is_changed = true;
}

void IWidget::clear() const
{
    if (!_corner_radius)
        _tft.fillRect(_x_pos, _y_pos, _width, _height, _back_color);
    else
        _tft.fillRoundRect(_x_pos, _y_pos, _width, _height, _corner_radius, _back_color);

    if (_has_border)
    {
        if (!_corner_radius)
            _tft.drawRect(_x_pos, _y_pos, _width, _height, _border_color);
        else
            _tft.drawRoundRect(_x_pos, _y_pos, _width, _height, _corner_radius, _border_color);
    }
}
