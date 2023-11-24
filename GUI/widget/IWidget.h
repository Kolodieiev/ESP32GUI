#pragma once
#include <Arduino.h>

#include <TFT_eSPI.h>

class IWidget
{

public:
    enum Alignment : uint8_t
    {
        ALIGN_START = 0,
        ALIGN_CENTER,
        ALIGN_END
    };

    enum Gravity : uint8_t
    {
        GRAVITY_TOP = 0,
        GRAVITY_CENTER,
        GRAVITY_BOTTOM
    };

    enum Orientation : uint8_t
    {
        ORIENTATION_HORIZONTAL = 0,
        ORIENTATION_VERTICAL,
    };

    IWidget(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~IWidget() = 0;
    virtual IWidget *clone(uint16_t id) const = 0;

    virtual void onDraw() = 0;

    void forcedOnDraw();
    void setPos(uint16_t x, uint16_t y);
    void setHeight(uint16_t height);
    void setWidth(uint16_t width);
    void setBackColor(uint16_t back_color);

    /*!
     * @brief  Задати скруглення вуглів елементу.
     * @param  radius
     *         Значення скруглення вуглів.
     */
    void setCornerRadius(const uint8_t radius);

    /*!
     * @brief
     *          Встановити відображення межі елементу товщиною 1пкс.
     *          Межа буде відображатися за рахунок ширини самого елементу.
     * @param  state
     *         Логічне значення стану.
     */
    void setBorder(const bool state);

    void setBorderColor(uint16_t color);

    uint8_t getCornerRadius() const { return _corner_radius; }
    uint16_t getID() const { return _id; }
    uint16_t getXPos() const { return _x_pos; }
    uint16_t getYPos() const { return _y_pos; }
    uint16_t getHeight() const { return _height; }
    uint16_t getWidth() const { return _width; }
    uint16_t getBackColor() const { return _back_color; }
    uint16_t getBorderColor() const { return _border_color; }

    /*!
     * @brief  Залити елемент фоновим кольором.
     */
    void clear() const;

    bool hasBorder() const { return _has_border; }

protected:
    uint16_t _id{0};
    bool _is_changed{true};
    bool _has_border{false};

    uint16_t _x_pos{0};
    uint16_t _y_pos{0};
    uint16_t _width{1};
    uint16_t _height{1};
    uint16_t _back_color{0x0000};
    uint16_t _border_color{0x0000};
    uint8_t _corner_radius{0};
    TFT_eSPI &_tft;
};
