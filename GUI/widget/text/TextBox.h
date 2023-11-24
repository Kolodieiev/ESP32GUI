#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include "Label.h"

// TODO додати підтримку типу: пароль

enum EditType
{
    TYPE_TEXT = 0,
    TYPE_PASSWORD
};

class TextBox : public Label
{

public:
    TextBox(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~TextBox() {}
    virtual TextBox *clone(uint16_t id) const override;
    virtual void onDraw() override;

    /*!
     * @brief Видалити останній символ із рядка, що зберігається в цьому віджеті.
     */
    bool removeLastChar();

private:
    using Label::isTicker;
    using Label::setGravity;
    using Label::setIsTicker;
    using Label::setWidthToFit;

    uint16_t getFitStr(String &ret_str) const;
};
