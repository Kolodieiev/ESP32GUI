#pragma once
#include <Arduino.h>
#include "Menu.h"
#include "IItemsLoader.h"

class DynamicMenu : public Menu
{
public:
    DynamicMenu(IItemsLoader *loader, uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~DynamicMenu(){}
    virtual DynamicMenu *clone(uint16_t id) const;

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

    uint16_t getItemsNumberOnScreen() const;

private:
    IItemsLoader *_loader;
};
