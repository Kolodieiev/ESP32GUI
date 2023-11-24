#include <Arduino.h>
#include "DS3231.h"
#include "DS3231Util.h"

DS3231::DS3231(I2C_Master &i2c):_i2c{i2c}
{

}

bool DS3231::begin()
{
    _i2c.begin();

    uint8_t reg = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_STATUS);

    if (_i2c.getLastError())
        return false;

    // disable 32kHz pin
    reg &= ~_BV(DS3231_STS_BIT_EN32KHZ);
    _i2c.writeRegister(DS3231_ADDRESS, DS3231_REG_STATUS, reg);

    // set 24h mode
    reg = _i2c.readRegister(DS3231_ADDRESS, 0x02);
    reg &= ~_BV(6);
    _i2c.writeRegister(DS3231_ADDRESS, 0x02, reg);

    return true;
}

bool DS3231::isDateTimeValid()
{
    uint8_t statusRegValue = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_STATUS);
    return (!_i2c.getLastError() && !(statusRegValue & _BV(DS3231_STS_BIT_OSF)));
}

bool DS3231::isRunning()
{
    uint8_t ctrlReg = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_CONTROL);
    return (!_i2c.getLastError() && !(ctrlReg & _BV(DS3231_CNTRL_BIT_EOSC)));
}

void DS3231::setIsRunning(bool isRunning)
{
    uint8_t ctrlReg = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_CONTROL);

    if (isRunning)
        ctrlReg &= ~_BV(DS3231_CNTRL_BIT_EOSC);
    else
        ctrlReg |= _BV(DS3231_CNTRL_BIT_EOSC);

    _i2c.writeRegister(DS3231_ADDRESS, DS3231_REG_CONTROL, ctrlReg);
}

void DS3231::setDateTime(const DS3231DateTime &dateTime)
{
    uint8_t statusRegValue = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_STATUS);
    statusRegValue &= ~_BV(DS3231_STS_BIT_OSF); // clear the flag
    _i2c.writeRegister(DS3231_ADDRESS, DS3231_REG_STATUS, statusRegValue);

    uint8_t buffer[8];

    buffer[0] = DS3231_REG_TIMEDATE;
    buffer[1] = uint8ToBcd(dateTime.second);
    buffer[2] = uint8ToBcd(dateTime.minute);
    buffer[3] = uint8ToBcd(dateTime.hour); // 24 hour mode only

    uint8_t year = dateTime.year - 2000;
    uint8_t centuryFlag = 0;

    if (year >= 100)
    {
        year -= 100;
        centuryFlag = _BV(7);
    }

    // 1 = Monday
    uint8_t dow = dayOfWeek(dateTime.year, dateTime.month, dateTime.dayOfMonth);

    buffer[4] = uint8ToBcd(dow);
    buffer[5] = uint8ToBcd(dateTime.dayOfMonth);
    buffer[6] = uint8ToBcd(dateTime.month) | centuryFlag;
    buffer[7] = uint8ToBcd(year);

    _i2c.write(DS3231_ADDRESS, buffer, 8);
}

DS3231DateTime DS3231::getDateTime()
{
    uint8_t buffer[DS3231_REG_TIMEDATE_SIZE];
    buffer[0] = DS3231_REG_TIMEDATE;
    _i2c.write(DS3231_ADDRESS, buffer, 1);

    DS3231DateTime dt{0, 0, 0, 0, 0, 0};
    if (_i2c.getLastError() != 0)
        return dt;

    _i2c.read(DS3231_ADDRESS, buffer, DS3231_REG_TIMEDATE_SIZE);
    if (_i2c.getLastError() == 4)
        return dt;

    uint8_t second = bcdToUint8(buffer[0] & 0x7F);
    uint8_t minute = bcdToUint8(buffer[1]);
    uint8_t hour = bcdToBin24Hour(buffer[2]);

    // buffer[3] throwing away day of week

    uint8_t dayOfMonth = bcdToUint8(buffer[4]);
    uint8_t monthRaw = buffer[5];
    uint16_t year = bcdToUint8(buffer[6]) + 2000;

    if (monthRaw & _BV(7)) // century wrap flag
        year += 100;

    uint8_t month = bcdToUint8(monthRaw & 0x7f);

    dt.second = second;
    dt.minute = minute;
    dt.hour = hour;
    dt.dayOfMonth = dayOfMonth;
    dt.month = month;
    dt.year = year;

    return dt;
}

uint8_t DS3231::dayOfWeek(uint16_t year, uint8_t month, uint8_t dayOfMonth)
{
    for (uint8_t indexMonth = 1; indexMonth < month; ++indexMonth)
        dayOfMonth += pgm_read_byte(DAYS_IN_MONTH + indexMonth - 1);

    if (month > 2 && year % 4 == 0)
        dayOfMonth++;

    dayOfMonth = dayOfMonth + 365 * year + (year + 3) / 4 - 1;

    return (dayOfMonth + 6) % 7;
}

DS3231DateTime DS3231::compiledToDS3231DateTime(const char *date, const char *time)
{
    DS3231DateTime dt;

    dt.year = strToUint8(date + 9) + 2000;

    switch (date[0])
    {
    case 'J':
        if (date[1] == 'a')
            dt.month = 1;
        else if (date[2] == 'n')
            dt.month = 6;
        else
            dt.month = 7;
        break;
    case 'F':
        dt.month = 2;
        break;
    case 'A':
        dt.month = date[1] == 'p' ? 4 : 8;
        break;
    case 'M':
        dt.month = date[2] == 'r' ? 3 : 5;
        break;
    case 'S':
        dt.month = 9;
        break;
    case 'O':
        dt.month = 10;
        break;
    case 'N':
        dt.month = 11;
        break;
    case 'D':
        dt.month = 12;
        break;
    }

    dt.dayOfMonth = strToUint8(date + 4);
    dt.hour = strToUint8(time);
    dt.minute = strToUint8(time + 3);
    dt.second = strToUint8(time + 6);

    return dt;
}