#include "Image.h"

Image::Image(uint16_t wiget_ID, TFT_eSPI &tft) : IWidget(wiget_ID, tft) {}

Image *Image::clone(uint16_t id) const
{
    Image *clone = new Image(*this);
    clone->_id = id;
    return clone;
}

void Image::onDraw()
{
    if (!_is_changed)
        return;

    _is_changed = false;

    clear();

    _tft.setSwapBytes(true);

    if (_two_byte_img)
    {
        if (!_is_transparent)
            _tft.pushImage(_x_pos, _y_pos, _width, _height, _two_byte_img);
        else
            _tft.pushImage(_x_pos, _y_pos, _width, _height, _two_byte_img, _transparent_color);
    }
    else
    {
        log_e("Спроба нарисувати NULL-зображення");
    }
}