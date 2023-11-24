/*
 * Генератор однотипних віджетів.
 * Використовується тут для часткового очищення файлів вікон від однотипного коду.
 */

#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

#include "screen/resources/color.h"

#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/text/Focusable.h"
#include "lib/GUI/widget/navbar/NavBar.h"
#include "lib/GUI/widget/menu/DynamicMenu.h"

class WidgetCreator
{
public:
    WidgetCreator(TFT_eSPI &tft);

    EmptyLayout *getEmptyLayout(uint16_t id);
    Focusable *getFocusableItem(uint16_t id, const char *text, uint8_t font_id = 2);
    NavBar *getNavbar(uint16_t id, const char *left, const char *middle, const char *right);
    DynamicMenu *getDynamicMenu(uint16_t id, IItemsLoader *loader);

private:
    const uint8_t SCROLLBAR_WIDTH{5};
    const uint8_t NAVBAR_HEIGHT{18};

    TFT_eSPI &_tft;
};
