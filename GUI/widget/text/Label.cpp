#include "Label.h"

Label::Label(uint16_t widget_ID, TFT_eSPI &tft) : IWidget(widget_ID, tft) {}

Label *Label::clone(uint16_t id) const
{
    Label *clone = new Label(*this);
    clone->_id = id;
    return clone;
}

void Label::setWidthToFit(uint16_t add_width_value)
{
    _width = calcTextPixels();
    _width += 2 + add_width_value;
    _is_changed = true;
}

void Label::setText(const char *text)
{
    _text = text;
    _text_len = getRealStrLen(_text);
    _first_draw_char_pos = 0;

    _dynamic_text.clear();
    _is_dynamic = false;
    _is_changed = true;
}

void Label::setText(String text)
{
    _dynamic_text = text;
    _text_len = getRealStrLen(_dynamic_text);
    _first_draw_char_pos = 0;

    _is_dynamic = true;
    _text = NULL;
    _is_changed = true;
}

const char *Label::getText() const
{
    if (!_is_dynamic)
    {
        if (_text == nullptr)
            return "";
        else
            return _text;
    }
    else
        return _dynamic_text.c_str();
}

uint8_t Label::getCharHgt() const
{
    if (_font_ID == 2)
        return chr_hgt_f15 * _text_size;
    else
        return chr_hgt_f9 * _text_size;
}

void Label::setIsTicker(bool state)
{
    _is_ticker = state;
    _temp_is_ticker = _is_ticker;

    if (!state)
        _first_draw_char_pos = 0;

    _is_changed = true;
}

void Label::setFontID(uint8_t font_ID)
{
    if (font_ID != 2 && font_ID != 4)
        _font_ID = 2;
    else
        _font_ID = font_ID;

    _is_changed = true;
}

uint16_t Label::calcXStrOffset(uint16_t str_pix_num) const
{
    if (_width > str_pix_num + _text_offset)
    {
        if (_is_ticker)
            return _text_offset;

        switch (_text_alignment)
        {
        case ALIGN_START:
            return _text_offset;

        case ALIGN_CENTER:
            return (float)(_width - str_pix_num) / 2 + _text_offset;

        case ALIGN_END:
            return _width - str_pix_num + _text_offset;
        }
    }

    return 0;
}

uint16_t Label::calcYStrOffset() const
{
    uint8_t char_height{0};

    if (_font_ID == 2)
        char_height = chr_hgt_f15 * _text_size;
    else if (_font_ID == 4)
        char_height = chr_hgt_f9 * _text_size;

    if (_height > char_height)
    {
        switch (_text_gravity)
        {
        case GRAVITY_TOP:
            return 0;

        case GRAVITY_CENTER:
            return (float)(_height - char_height) / 2;

        case GRAVITY_BOTTOM:
            return _height - char_height;
        }
    }

    return 0;
}

uint16_t Label::getRealStrLen(const String &str) const
{
    if (str == nullptr || str == "")
        return 0;

    uint16_t length{0};

    for (char c : str)
    {
        if ((c & 0xC0) != 0x80)
        {
            ++length;
        }
    }
    return length;
}

uint16_t Label::calcTextPixels(uint16_t char_pos) const
{
    const char *ch_str = _is_dynamic ? _dynamic_text.c_str() : _text;

    if (ch_str == nullptr || strcmp(ch_str, "") == 0)
        return 0;

    uint16_t len = strlen(ch_str);
    uint16_t n = char_pos;

    uint16_t unicode;
    uint16_t pix_sum{0};

    if (_font_ID == 2)
        while (n < len)
        {
            unicode = _tft.decodeUTF8((uint8_t *)ch_str, &n, len - n);
            unicode = getCharPos(unicode);
            pix_sum += pgm_read_byte(widtbl_f15 + unicode);
        }
    else
        while (n < len)
        {
            unicode = _tft.decodeUTF8((uint8_t *)ch_str, &n, len - n);
            unicode = getCharPos(unicode);
            pix_sum += pgm_read_byte(widtbl_f9 + unicode);
        }

    return pix_sum * _text_size;
}

uint16_t Label::getFitStr(String &ret_str, uint16_t start_pos) const
{
    uint16_t chars_counter{0};

    const char *ch_str = _is_dynamic ? _dynamic_text.c_str() : _text;

    if (ch_str == nullptr || strcmp(ch_str, "") == 0)
        return 0;

    uint16_t len = strlen(ch_str);

    if (start_pos >= len)
    {
        log_e("Стартова позиція повинна бути меншою за довжину рядка");
        return 0;
    }

    uint16_t n = start_pos;

    uint16_t unicode;
    uint16_t pix_sum{0};

    if (_font_ID == 2)
    {
        while (n < len)
        {
            unicode = _tft.decodeUTF8((uint8_t *)ch_str, &n, len - n);
            unicode = getCharPos(unicode);

            uint16_t char_w = pgm_read_byte(widtbl_f15 + unicode) * _text_size;

            if (pix_sum + char_w >= _width - _text_offset - 2)
                break;
            else
            {
                pix_sum += char_w;
                chars_counter++;
            }
        }
    }
    else if (_font_ID == 4)
    {
        while (n < len)
        {
            unicode = _tft.decodeUTF8((uint8_t *)ch_str, &n, len - n);
            unicode = getCharPos(unicode);

            uint16_t char_w = pgm_read_byte(widtbl_f9 + unicode) * _text_size;

            if (pix_sum + char_w >= _width - _text_offset - 2)
                break;
            else
            {
                pix_sum += char_w;
                chars_counter++;
            }
        }
    }

    ret_str = getSubStr(ch_str, start_pos, chars_counter);

    return pix_sum;
}

String Label::getSubStr(const String &str, uint16_t start, uint16_t length) const
{
    if (!length)
        return "";

    unsigned int c, i, ix, q;

    unsigned int min = -1, max = -1; //

    for (q = 0, i = 0, ix = str.length(); i < ix; ++i, q++)
    {
        if (q == start)
            min = i;

        if (q <= start + length || length == -1)
            max = i;

        c = (unsigned char)str[i];

        if (c >= 0 && c <= 127)
            i += 0;
        else if ((c & 0xE0) == 0xC0)
            i += 1;
        else if ((c & 0xF0) == 0xE0)
            i += 2;
        else if ((c & 0xF8) == 0xF0)
            i += 3;
        else
            return ""; // invalid utf8
    }

    if (q <= start + length || length == -1)
        max = i;

    if (min == -1 || max == -1)
        return "";

    return str.substring(min, max);
}

uint16_t Label::getCharPos(uint16_t unicode) const
{
    if (unicode > 127)
    {
        if (unicode > 1039 && unicode < 1104)
            unicode = unicode - 944;
        else
            switch (unicode)
            {
            case 1030:
                unicode = 41;
                break;
            case 1031:
                unicode = 162;
                break;
            case 1111:
                unicode = 163;
                break;
            case 1110:
                unicode = 73;
                break;
            case 1028:
                unicode = 160;
                break;
            case 1108:
                unicode = 161;
                break;
            case 1025:
                unicode = 37;
                break;
            case 1105:
                unicode = 69;
                break;
            case 1168:
                unicode = 164;
                break;
            case 1169:
                unicode = 165;
                break;
            case 171:
            case 187:
                unicode = 2;
                break;
            default:
                unicode = CHRS_NMBR - 1;
            }
    }
    else
        unicode -= 32;

    return unicode;
}

void Label::onDraw()
{
    if (!_is_changed)
    {
        if (!_is_ticker)
            return;
        else if ((millis() - _last_time_ticker_update) < TICKER_UPDATE_DELAY)
            return;
        else
            _last_time_ticker_update = millis();
    }

    _is_ticker = _temp_is_ticker;
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

        // Якщо текст не потребує прокрутки, вимикаємо її для зменшення навантаження на ядро.
        _is_ticker = false;
    }
    else
    {
        String sub_str;
        uint16_t sub_str_pix_num = getFitStr(sub_str, _first_draw_char_pos);
        uint16_t txt_x_pos = calcXStrOffset(sub_str_pix_num);

        if (_is_ticker)
        {
            if (_first_draw_char_pos == _text_len - 1 || sub_str_pix_num == 0)
                _first_draw_char_pos = 0;
            else
                _first_draw_char_pos++;
        }

        _tft.drawString(sub_str, _x_pos + txt_x_pos, _y_pos + txtYPos);
    }
}
