#include "DS3231Alarm.h"
#include "DS3231Util.h"

DS3231Alarm::DS3231Alarm(I2C_Master &i2c):_i2c{i2c}
{

}

bool DS3231Alarm::isEnabled()
{
    uint8_t ctrlReg = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_CONTROL);
    return ctrlReg &= _BV(DS3231_AIFMASK);
}

bool DS3231Alarm::isAlarmed(){
    uint8_t statusReg = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_STATUS);
    return statusReg &= _BV(DS3231_CNTRL_BIT_A2IE);
}

void DS3231Alarm::setAlarmData(const DS3231AlarmData &alarmData)
{
    uint8_t buffer[3];

    uint8_t flags{0x08}; // hour minutes match

    buffer[0] = DS3231_REG_ALARMTWO;
    buffer[1] = uint8ToBcd(alarmData.minute) | ((flags & 0x01) << 7);
    buffer[2] = uint8ToBcd(alarmData.hour) | ((flags & 0x02) << 6); // 24 hour mode only
    //buffer[3] = uint8ToBcd(0) | ((flags & 0x0c) << 4); // day of

    _i2c.write(DS3231_ADDRESS, buffer, 3);
}

void DS3231Alarm::enable(bool enableWithoutExternalPower)
{
    //6 bit change
    _i2c.writeRegister(DS3231_ADDRESS, DS3231_REG_CONTROL, 0b00000110);
}

void DS3231Alarm::disable()
{
    _i2c.writeRegister(DS3231_ADDRESS, DS3231_REG_CONTROL, 0b00000000);
}

DS3231AlarmData DS3231Alarm::getAlarmData()
{
    uint8_t buffer[DS3231_REG_ALARMTWO_SIZE];

    buffer[0] = DS3231_REG_ALARMTWO;

    _i2c.write(DS3231_ADDRESS, buffer, 1);
    if (_i2c.getLastError() != 0)
        return DS3231AlarmData{0, 0};

    _i2c.read(DS3231_ADDRESS, buffer, DS3231_REG_ALARMTWO_SIZE);
    if (_i2c.getLastError() != 0)
        return DS3231AlarmData{0, 0};


    uint8_t minute = bcdToUint8(buffer[0] & 0x7F);
    uint8_t hour = bcdToBin24Hour(buffer[1] & 0x7f);

    return DS3231AlarmData{hour, minute};
}

void DS3231Alarm::procAlarm()
{
    uint8_t sreg = _i2c.readRegister(DS3231_ADDRESS, DS3231_REG_STATUS);
    sreg &= ~DS3231_AIFMASK; // clear the flags
    _i2c.writeRegister(DS3231_ADDRESS, DS3231_REG_STATUS, sreg);
}

