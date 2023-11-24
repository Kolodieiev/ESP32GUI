#include "WidgetCreator.h"

WidgetCreator::WidgetCreator(TFT_eSPI &tft) : _tft{tft} {}

EmptyLayout *WidgetCreator::getEmptyLayout(uint16_t id)
{
    EmptyLayout *layout = new EmptyLayout(1, _tft);
    layout->setBackColor(COLOR_MAIN_BACK);
    layout->setWidth(_tft.width());
    layout->setHeight(_tft.height());
    return layout;
}

Focusable *WidgetCreator::getFocusableItem(uint16_t id, const char *text, uint8_t font_id)
{
    Focusable *item = new Focusable(id, _tft);
    item->setText(text);
    item->setFontID(font_id);
    item->setGravity(IWidget::GRAVITY_CENTER);
    item->setTextOffset(3);
    item->setFocusBorderColor(COLOR_LIME);
    item->setChangeBorderState(true);
    item->setTextColor(COLOR_WHITE);
    item->setBackColor(COLOR_MENU_ITEM);

    return item;
}

NavBar *WidgetCreator::getNavbar(uint16_t id, const char *left, const char *middle, const char *right)
{
    Label *sel_txt = new Label(1, _tft);
    sel_txt->setText(left);
    sel_txt->setAlign(IWidget::ALIGN_CENTER);
    sel_txt->setGravity(IWidget::GRAVITY_CENTER);
    sel_txt->setTextColor(COLOR_WHITE);
    sel_txt->setTextOffset(1);
    sel_txt->setWidthToFit();

    Label *mid_txt = sel_txt->clone(2);
    mid_txt->setText(middle);
    mid_txt->setWidthToFit();

    Label *back_txt = sel_txt->clone(3);
    back_txt->setText(right);
    back_txt->setWidthToFit();

    NavBar *navbar = new NavBar(id, _tft);
    navbar->setBackColor(COLOR_NAV_PANEL_BACK);
    navbar->setHeight(NAVBAR_HEIGHT);
    navbar->setWidth(_tft.width());
    navbar->setPos(0, _tft.height() - NAVBAR_HEIGHT);
    navbar->setWidgets(sel_txt, mid_txt, back_txt);

    return navbar;
}

DynamicMenu *WidgetCreator::getDynamicMenu(uint16_t id, IItemsLoader *loader)
{
    DynamicMenu *menu = new DynamicMenu(loader, id, _tft);
    menu->setBackColor(COLOR_MENU_ITEM);
    menu->setWidth(_tft.width());
    menu->setHeight(_tft.height() - NAVBAR_HEIGHT);
    menu->setItemsHeight((_tft.height() - NAVBAR_HEIGHT) / 3);
    return menu;
}
