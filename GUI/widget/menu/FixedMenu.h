#pragma once
#include <Arduino.h>
#include "Menu.h"

class FixedMenu : public Menu
{
public:
    FixedMenu(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~FixedMenu();
    virtual FixedMenu *clone(uint16_t id) const;

    /*!
     * @brief
     *       Перемістити фокус на попередній елемент в списку.
     *
     * @return
     *        true в разі успіху операції. Інакше false.
     */
    virtual bool focusUp() override;

    /*!
     * @brief
     *       Перемістити фокус на наступний елемент в списку.
     *
     * @return
     *        true в разі успіху операції. Інакше false.
     */
    virtual bool focusDown() override;
};
