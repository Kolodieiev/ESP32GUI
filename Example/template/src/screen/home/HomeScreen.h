#pragma once
#include <Arduino.h>

#include "lib/GUI/IScreen.h"
#include "lib/GUI/widget/text/Label.h"

class HomeScreen : public IScreen
{
public:
    HomeScreen(TFT_eSPI &tft);
    virtual ~HomeScreen(){};

protected:
    virtual void loop() override;
    virtual void doGUIWork() override;
    //
    virtual void okClickedHandler() override;
    virtual void upClickedHandler() override;
    virtual void downClickedHandler() override;
    virtual void leftClickedHandler() override;
    virtual void rightClickedHandler() override;
    virtual void okPressedHandler() override;
    virtual void upPressedHandler() override;
    virtual void downPressedHandler() override;
    virtual void leftPressedHandler() override;
    virtual void rightPressedHandler() override;

private:
    // *IWidget рекомендується кешувати, якщо їхні об'єкти часто оновлюються на екрані.
    // А можна діставати через ID віджета з контейнера "на льоту". (IWidget * findWidgetByID(uint16_t) )
    Label *_time_txt;
};
