#pragma once

#include <Arduino.h>
#include "DS3231.h"
#include "lib/I2C_Master/I2C_Master.h"


struct DS3231AlarmData
{
    uint8_t hour;
    uint8_t minute;
};

class DS3231Alarm
{
private:
    const uint8_t DS3231_REG_ALARMTWO = 0x0B;
    const uint8_t DS3231_REG_ALARMTWO_SIZE = 3;

    // control bits
    const uint8_t DS3231_CNTRL_BIT_A2IE = 1;

    // status bits
    const uint8_t DS3231_A1F = 0;
    const uint8_t DS3231_A2F = 1;
    const uint8_t DS3231_AIFMASK = (_BV(DS3231_A1F) | _BV(DS3231_A2F));
    //
    I2C_Master _i2c;

public:
    DS3231Alarm(I2C_Master &i2c);
    bool isEnabled();
    bool isAlarmed();
    void setAlarmData(const DS3231AlarmData &alarmData);
    DS3231AlarmData getAlarmData();
    void enable(bool enableWithoutExternalPower);
    void disable();

    void procAlarm();
};

 