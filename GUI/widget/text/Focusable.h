#pragma once
#include <Arduino.h>
#include "../text/Label.h"

class Focusable : public Label
{
public:
    Focusable(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~Focusable() {}
    virtual Focusable *clone(uint16_t id) const override;

    /*!
     * @brief
     *       Задати флаг відображення межі елементу, при встановленні фокусу на ньому.
     * @param  state
     *       Стан флагу.
     */
    void setChangeBorderState(const bool state)
    {
        _need_change_border = state;
        _is_changed = true;
    }

    /*!
     * @brief
     *       Задати флаг зміни кольору фону елементу, при встановленні фокусу на ньому.
     * @param  state
     *       Стан флагу.
     */
    void setChangeBackState(const bool state)
    {
        _need_change_back = state;
        _is_changed = true;
    }

    /*!
     * @brief
     *       Задати колір межі елементу, при встановленні фокусу на ньому.
     * @param  color
     *       Колір фону.
     */
    void setFocusBorderColor(uint16_t color)
    {
        _focus_border_color = color;
        _is_changed = true;
    }
    uint16_t getFocusBorderColor() const { return _focus_border_color; }

    /*!
     * @brief
     *       Задати колір фону при встановленні фокусу на елементі.
     * @param  color
     *       Колір фону.
     */
    void setFocusBackColor(uint16_t color)
    {
        _focus_back_color = color;
        _is_changed = true;
    }
    uint16_t getFocusBackColor() const { return _focus_back_color; }

    /*!
     * @brief Встановити фокус на елементі.
     */
    void setFocus();

    /*!
     * @brief Прибрати фокус з елементу.
     */
    void removeFocus();

private:
    bool _has_focus{false};
    bool _old_border_state;

    bool _need_change_border{false};
    bool _need_change_back{false};

    uint16_t _focus_border_color{0xFFFF};
    uint16_t _old_border_color{0xFFFF};

    uint16_t _focus_back_color{0xFFFF};
    uint16_t _old_back_color{0xFFFF};
};
