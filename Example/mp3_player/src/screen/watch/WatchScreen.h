#pragma once
#include <Arduino.h>
#include "lib/DS3231/DS3231.h"
#include "lib/I2C_Master/I2C_Master.h"
#include "lib/GUI/IScreen.h"

class WatchScreen : public IScreen
{
public:
    WatchScreen(TFT_eSPI &tft);
    virtual ~WatchScreen() {}

protected:
    virtual void loop() override;
    virtual void doGUIWork() override;
    //
    virtual void upClickedHandler() override;
    virtual void downClickedHandler() override;
    virtual void leftClickedHandler() override;
    virtual void rightClickedHandler() override;
    virtual void okClickedHandler() override;
    virtual void okPressedHandler() override;
    virtual void upPressedHandler() override;
    virtual void downPressedHandler() override;
    virtual void leftPressedHandler() override;
    virtual void rightPressedHandler() override;

private:
    const uint8_t ID_HOURS_SPIN = 2;
    const uint8_t ID_MINUTES_SPIN = 3;
    const uint8_t ID_DAY_SPIN = 4;
    const uint8_t ID_MONTH_SPIN = 5;
    const uint8_t ID_YEAR_SPIN = 6;

    I2C_Master _i2c;
    DS3231 _watch{_i2c};
    DS3231DateTime _cur_date_time;

    uint8_t _curent_focus_pos{0};

    void changeCurDateTime(uint16_t widget_id, uint16_t value);
};
