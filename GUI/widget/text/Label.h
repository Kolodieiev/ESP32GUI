#pragma once
#include <Arduino.h>
#include "../IWidget.h"

class Label : public IWidget
{
public:
    Label(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~Label(){};
    virtual void onDraw();
    virtual Label *clone(uint16_t id) const override;

    /*!
     * @brief
     *      Викликайте цей метод тільки після того, як налаштували інші параметри для віджета.
     *      Вираховує і підганяє ширину віджета таким чином, щоб вмістити текст + відступ.
     * @param  add_width_value
     *      Значення буде додане до ширини елементу. По замовченню 0.
     */
    void setWidthToFit(uint16_t add_width_value = 0);

    void setText(const char *text);
    void setText(String text);

    /*!
     * @brief
     *      Встановити розмір тексту.
     * @param  size
     *      Значення може бути від 1 до 7.
     */
    void setTextSize(uint8_t size)
    {
        _text_size = size;
        _is_changed = true;
    }
    void setTextColor(uint16_t textColor)
    {
        _text_color = textColor;
        _is_changed = true;
    }

    /*!
     * @brief
     *      Встановити ID шрифту для данного віджета.
     *      По замовченню встановлено шрифт з ID == 2.
     * @param  font_ID
     *      Наразі може мати значення тільки 2 або 4.
     */
    void setFontID(uint8_t font_ID);

    /*!
     * @brief
     *       Встановити положення тексту по вертикалі.
     *       По замовченню встановлено GRAVITY_TOP.
     * @param  gravity
     *       Може мати значення: GRAVITY_TOP / GRAVITY_CENTER / GRAVITY_BOTTOM.
     */
    void setGravity(const Gravity gravity)
    {
        _text_gravity = gravity;
        _is_changed = true;
    }

    /*!
     * @brief
     *       Встановити положення тексту по горизонталі.
     *       По замовченню встановлено ALIGN_START.
     * @param  alignment
     *       Може мати значення: ALIGN_START / ALIGN_CENTER / ALIGN_END.
     */
    void setAlign(const Alignment alignment)
    {
        _text_alignment = alignment;
        _is_changed = true;
    }

    /*!
     * @brief
     *       Встановити зміщення позиції тексту (в пікселях) вправо.
     * @param  offset
     *       Значення зміщення тексту.
     */
    void setTextOffset(uint8_t offset)
    {
        _text_offset = offset;
        _is_changed = true;
    }

    /*!
     * @return Кількість символів в тексті, що зберігається в данному віджеті.
     */
    uint16_t getTextLength() const { return _text_len; }

    /*!
     * @brief
     *       Встановити чи буде текст автоматично прокручуватися,
     *       якщо ширини віджета не достатньо, щоб вмістити рядок повністю.
     * @param  state
     *       Логічне значення перемикача.
     */
    void setIsTicker(bool state);

    const char *getText() const;
    /*!
     * @return Висоту шрифту в пікселях для данного віджета.
     */
    uint8_t getCharHgt() const;
    uint8_t getFontID() const { return _font_ID; }
    bool isTicker() const { return _temp_is_ticker; }

protected:
    bool _is_dynamic{false};
    const char *_text{nullptr};
    uint16_t _text_len{0};
    String _dynamic_text{""};
    uint8_t _text_size{1};
    uint16_t _text_color{0xFFFF};
    uint8_t _font_ID{2};
    uint8_t _text_offset{0};
    Gravity _text_gravity{GRAVITY_TOP};
    Alignment _text_alignment{ALIGN_START};

    bool _temp_is_ticker{false};
    bool _is_ticker{false};
    unsigned long _last_time_ticker_update{0};
    const uint8_t TICKER_UPDATE_DELAY{160};

    uint16_t _first_draw_char_pos{0};
    //
    uint16_t calcXStrOffset(uint16_t str_pix_num) const;
    uint16_t calcYStrOffset() const;
    uint16_t getRealStrLen(const String &str) const;
    uint16_t getFitStr(String &ret_str, uint16_t start_pos = 0) const;
    String getSubStr(const String &str, uint16_t from, uint16_t length) const;
    uint16_t calcTextPixels(uint16_t char_pos = 0) const;
    uint16_t getCharPos(uint16_t unicode) const;
};