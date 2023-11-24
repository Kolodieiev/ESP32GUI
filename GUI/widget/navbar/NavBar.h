#pragma once
#include <Arduino.h>
#include "../IWidget.h"

class NavBar : public IWidget
{
public:
    NavBar(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~NavBar();
    virtual void onDraw();
    NavBar *clone(uint16_t id) const override;

    /*!
     * @brief
     *       Задати віджети, які будуть відображатися на панелі навігації.
     * @param  start
     *       Крайній лівий віджет.
     * @param  middle
     *       Центральний віджет.
     * @param  last
     *       Крайній правий віджет.
     */
    void setWidgets(IWidget *start, IWidget *middle, IWidget *last);

private:
    IWidget *_first{nullptr};
    IWidget *_middle{nullptr};
    IWidget *_last{nullptr};
};
