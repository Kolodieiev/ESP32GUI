#pragma once
#include <Arduino.h>
#include "../layout/IWidgetContainer.h"
#include "../text/Focusable.h"

class Menu : public IWidgetContainer
{

public:
    Menu(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~Menu();
    virtual void onDraw();

    virtual bool focusUp() = 0;
    virtual bool focusDown() = 0;

    virtual void deleteWidgets() override;

    /*!
     * @brief
     *       Знайти елемент в контейнері по його ідентифікатору.
     *
     * @return
     *        Focusable * в разі успіху операції. Інакше nullptr.
     */
    Focusable *findItemByID(uint16_t item_ID) const;

    /*!
     * @briefconst
     *       Задати висоту для елементів меню.
     *
     * @param  height
     *       Висота елементів меню.
     */
    void setItemsHeight(uint16_t height)
    {
        _item_height = height > 0 ? height : 1;
        _is_changed = true;
    }

    /*!
     * @brief
     *       Задати орієнтацію меню.
     *       По замовченню встановлено ORIENTATION_VERTICAL.
     * @param  orientation
     *       Може мати значення: ORIENTATION_VERTICAL / ORIENTATION_HORIZONTAL.
     */
    void setOrientation(const Orientation orientation)
    {
        _orientation = orientation;
        _is_changed = true;
    }

    /*!
     * @brief
     *       Задати флаг, який перемикає елемент меню в стан Ticker-у, якщо даний елемент знаходиться у фокусі
     *       і текст не поміщується на цьому елементі.
     * @param  state
     *       Стан флага.
     */
    void setIsTickerInFocus(const bool state)
    {
        _draw_ticker = state;
        _is_changed = true;
    }

    uint16_t getItemsHeight() const { return _item_height; }
    bool getIsTickerInFocus() const { return _draw_ticker; }
    /*!
     * @return
     *        Ідентифікатор елементу, на якому встановлено фокус.
     */
    uint16_t getCurrentItemID() const;

    /*!
     * @return
     *        Текст елементу, на якому встановлено фокус.
     */
    String getCurrentItemText() const;

protected:
    bool _draw_ticker{false};

    uint16_t _first_item_index{0};
    uint16_t _cur_focus_pos{0};

    uint16_t _item_height{1};
    uint16_t _item_width{1};

    Orientation _orientation{ORIENTATION_HORIZONTAL};

    void drawItems(uint16_t start, uint16_t count);
    uint16_t getCyclesCount() const;
};
