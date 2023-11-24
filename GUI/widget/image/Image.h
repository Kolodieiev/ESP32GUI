#pragma once
#include <Arduino.h>
#include "../IWidget.h"

class Image : public IWidget
{

public:
    Image(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~Image() {}
    virtual void onDraw();
    virtual Image *clone(uint16_t id) const;

    void setImage(const uint16_t *image)
    {
        _two_byte_img = image;
        _is_changed = true;
    }

    void setTransparentColor(uint16_t color)
    {
        _is_transparent = true;
        _transparent_color = color;
        _is_changed = true;
    }

    void clearTransparent()
    {
        _is_transparent = false;
        _is_changed = true;
    }

private:
    bool _is_transparent{false};
    uint16_t _transparent_color;

    const uint16_t *_two_byte_img;
};
