#include "TextBox.h"

TextBox::TextBox(uint16_t widget_ID, TFT_eSPI &tft) : Label(widget_ID, tft)
{
    _text_gravity = GRAVITY_CENTER;
}

TextBox *TextBox::clone(uint16_t id) const
{
    TextBox *clone = new TextBox(*this);
    clone->_id = id;
    return clone;
}

bool TextBox::removeLastChar()
{
    if (!_is_dynamic)
    {
        if (_text == nullptr)
            return false;

        _is_dynamic = true;
        _dynamic_text = _text;
        _text = NULL;
    }
    else if (_dynamic_text == "")
        return false;

    _dynamic_text = getSubStr(_dynamic_text, 0, getRealStrLen(_dynamic_text) - 1);

    _is_changed = true;
    return true;
}

uint16_t TextBox::getFitStr(String &ret_str) const
{
    uint16_t first_char_pos{1};

    uint16_t len;

    if (!_is_dynamic)
    {
        if (_text == nullptr)
            len = 0;
        else
            len = strlen(_text);
    }
    else
        len = _dynamic_text.length();

    const char *ch_str = _is_dynamic ? _dynamic_text.c_str() : _text;

    while (first_char_pos < len - 1)
    {
        uint16_t pix_num = calcTextPixels(first_char_pos);

        if (pix_num < _width)
        {
            ret_str = getSubStr(ch_str, first_char_pos, getRealStrLen(ch_str) - 1);
            return pix_num;
        }

        first_char_pos++;
    }

    return 0;
}

void TextBox::onDraw()
{
    if (!_is_changed)
        return;

    _is_changed = false;

    clear();

    _tft.setTextFont(_font_ID);
    _tft.setTextSize(_text_size);
    _tft.setTextColor(_text_color);

    uint16_t txtYPos = calcYStrOffset();
    uint16_t str_pix_num = calcTextPixels();

    if (str_pix_num + _text_offset < _width)
    {
        uint16_t txt_x_pos = calcXStrOffset(str_pix_num);

        if (!_is_dynamic)
        {
            if (_text != nullptr)
                _tft.drawString(_text, _x_pos + txt_x_pos, _y_pos + txtYPos);
        }
        else
            _tft.drawString(_dynamic_text, _x_pos + txt_x_pos, _y_pos + txtYPos);
    }
    else
    {
        String sub_str;
        uint16_t sub_str_pix_num = getFitStr(sub_str);
        uint16_t txt_x_pos = calcXStrOffset(sub_str_pix_num);

        _tft.drawString(sub_str, _x_pos + txt_x_pos, _y_pos + txtYPos);
    }
}
