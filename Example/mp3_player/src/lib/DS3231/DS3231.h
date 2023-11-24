#pragma once

#include <Arduino.h>
#include "lib/I2C_Master/I2C_Master.h"

const uint8_t DS3231_ADDRESS = 0x68;
const uint8_t DS3231_REG_CONTROL = 0x0E;
const uint8_t DS3231_REG_STATUS = 0x0F;

const uint8_t DAYS_IN_MONTH[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct DS3231DateTime
{
    unsigned int second : 6;
    unsigned int minute : 6;
    unsigned int hour : 6;
    unsigned int dayOfMonth : 5;
    unsigned int month : 4;
    unsigned int year : 12;

    bool operator==(const DS3231DateTime &dateTime) const
    {
        if (this == &dateTime)
            return true;

        return this->minute == dateTime.minute &&
               this->hour == dateTime.hour &&
               this->dayOfMonth == dateTime.dayOfMonth &&
               this->month == dateTime.month &&
               this->year == dateTime.year;
    }

    bool operator!=(const DS3231DateTime &dateTime) const
    {
        return !(*this == dateTime);
    }

    uint8_t dayOfWeek() const
    {
        uint16_t days = daysSinceFirstOfYear2000(year - 2000, month, dayOfMonth);

        uint8_t day = (days + 6) % 7;

        if (day == 0)
            day = 6;
        else
            day--;

        return day;
    }

private:
    uint16_t daysSinceFirstOfYear2000(uint16_t year, uint8_t month, uint8_t dayOfMonth) const
    {
        uint16_t days = dayOfMonth;
        for (uint8_t indexMonth = 1; indexMonth < month; ++indexMonth)
        {
            days += pgm_read_byte(DAYS_IN_MONTH + indexMonth - 1);
        }
        if (month > 2 && year % 4 == 0)
        {
            days++;
        }
        return days + 365 * year + (year + 3) / 4 - 1;
    }
};

class DS3231
{

public:
    DS3231(I2C_Master &i2c);
    bool begin();
    bool isDateTimeValid();
    bool isRunning();
    void setIsRunning(bool isRunning);
    void setDateTime(const DS3231DateTime &dateTime);
    DS3231DateTime getDateTime();
    uint8_t dayOfWeek(uint16_t year, uint8_t month, uint8_t dayOfMonth);
    DS3231DateTime compiledToDS3231DateTime(const char *date, const char *time);

private:
    const uint8_t DS3231_REG_TIMEDATE = 0x00;

    const uint8_t DS3231_REG_TIMEDATE_SIZE = 7;

    const uint8_t DS3231_CNTRL_BIT_EOSC = 7;

    const uint8_t DS3231_STS_BIT_BSY = 2;
    const uint8_t DS3231_STS_BIT_EN32KHZ = 3;
    const uint8_t DS3231_STS_BIT_OSF = 7;
    //
    I2C_Master _i2c;
};
