#pragma once
#include <Arduino.h>
#include "../layout/IWidgetContainer.h"
#include "../text/Focusable.h"

class KeyboardRow : public IWidgetContainer
{

public:
    KeyboardRow(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~KeyboardRow(){}

    virtual void onDraw();
    virtual KeyboardRow *clone(uint16_t id) const;

    /*!
     * @return
     *        Ідентифікатор елементу, на якому встановлено фокус для данного ряду.
     */
    uint16_t getCurrentBtnID() const;

    /*!
     * @return
     *        Текст елементу для данного ряду, на якому встановлено фокус.
     */
    String getCurrentBtnTxt() const;

    /*!
     * @brief
     *       Перемістити фокус на попередній елемент в списку.
     *
     * @return
     *        true в разі успіху операції. Інакше false.
     */
    bool focusUp();

    /*!
     * @brief
     *       Перемістити фокус на наступний елемент в списку.
     *
     * @return
     *        true в разі успіху операції. Інакше false.
     */
    bool focusDown();

    /*!
     * @brief
     *       Встановити висоту кнопок для данного ряду.
     * @param  height
     *       Значення висоти кнопок.
     */
    void setBtnsHeight(uint16_t height)
    {
        _btn_height = height > 0 ? height : 1;
        _is_changed = true;
    }

    /*!
     * @return
     *        Висоту кнопок для данного ряду.
     */
    uint16_t getBtnsHeight()  const { return _btn_height; }

    /*!
     * @brief
     *       Встановити ширину кнопок для данного ряду.
     * @param  width
     *       Значення ширини кнопок.
     */
    void setBtnsWidth(uint16_t width)
    {
        _btn_width = width > 0 ? width : 1;
        _is_changed = true;
    }

    /*!
     * @return
     *        Ширину кнопок для данного ряду.
     */
    uint16_t getBtnsWidth() const { return _btn_width; }

    /*!
     * @return
     *        Позицію кнопки для данного ряду, на якій встановлено фокус.
     */
    uint16_t getCurFocusPos() const { return _cur_focus_pos; }

    /*!
     * @brief
     *       Встановити фокус на кнопці данного ряду.
     * @param  pos
     *       Порядковий номер кнопки.
     */
    void setFocus(uint16_t pos);

    /*!
     * @brief
     *       Видалити фокус для данного ряду.
     */
    void removeFocus();

private:
    uint16_t _cur_focus_pos{0};

    uint16_t _btn_height{1};
    uint16_t _btn_width{1};
};
