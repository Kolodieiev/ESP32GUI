#pragma once
#include <Arduino.h>

#include "lib/DS3231/DS3231.h"
#include "lib/I2C_Master/I2C_Master.h"

#include "screen/WidgetCreator.h"
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
    const uint16_t UPD_TIME_INTERVAL{5000};
    const uint8_t PIN_VOLT_MEASH_READ{39};

    I2C_Master _i2c;
    DS3231 _watch{_i2c};
    unsigned long _last_time_update{0};

    DS3231DateTime _cur_date_time;
    DS3231DateTime _temp_date_time;

    WidgetCreator _creator{_tft};

    // *IWidget можна кешувати, як нижче.
    // А можна діставати через ID з контейнера "на льоту". (IWidget * findWidgetByID(uint16_t) )
    // Видаляти закешовані покажчики на об'єкти НЕ можна. Контейнер сам видалить свої віджети.
    Label *_time_txt;
    Label *_date_txt;
    Label *_day_of_week_txt;

    Label *_voltage_txt;

    void updTimeOnScreen();
    void updVoltageOnScreen();
};
