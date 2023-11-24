#include <Arduino.h>
#include "I2C_Master.h"
#include <Wire.h>
/*!
 * @brief  I2C_Master init function
 */
I2C_Master::~I2C_Master()
{
    Wire.end();
}
void I2C_Master::begin()
{
    if (!_is_inited)
    {
        Wire.begin();
        _is_inited = true;
    }
}

/*!
 * @brief  Get last action error. Return 0 if action success.
 */
uint8_t I2C_Master::getLastError()
{
    return _last_error;
}

/*!
 * @brief  Check the I2C connection with the device
 * @param  addr
 *         I2C device address
 */
bool I2C_Master::hasConnection(uint8_t addr)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return false;
    }

    Wire.beginTransmission(addr);
    _last_error = Wire.endTransmission();

    return !_last_error;
}

/*!
 * @brief  Send data array to device
 * @param  addr
 *         I2C device address
 */
void I2C_Master::write(uint8_t addr, const uint8_t *dataPointer, uint8_t dataSize)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    Wire.beginTransmission(addr);
    Wire.write(dataPointer, dataSize);
    _last_error = Wire.endTransmission();
}

/*!
 * @brief  Write byte to I2C device register
 * @param  addr
 *         I2C device address
 * @param  reg
 *         Register address
 * @param  value
 *         Register value
 */
void I2C_Master::writeRegister(uint8_t addr, uint8_t reg, uint8_t value)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value);
    _last_error = Wire.endTransmission();
}

/*!
 * @brief  Read data from I2C device
 * @param  addr
 *         I2C device address
 * @param  dataBuff
 *         Pointer to the buffer where the data will be read
 * @param  dataSize
 *         How many bytes to read
 */
void I2C_Master::read(uint8_t addr, uint8_t *dataBuff, uint8_t dataSize)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    _last_error = 0;

    bool isSuccesRequest{false};

    isSuccesRequest = Wire.requestFrom(addr, dataSize) == dataSize;

    if (!isSuccesRequest)
    {
        _last_error = 4;
        return;
    }

    for (uint8_t i = 0; i < dataSize; ++i)
        dataBuff[i] = Wire.read();
}

/*!
 * @brief  Reading data from an I2C device to the end of an array pointer
 * @param  addr
 *         I2C device address
 * @param  dataBuff
 *         Pointer to the buffer where the data will be read
 * @param  offset
 *         Where to start writing data
 * @param  dataSize
 *         How many bytes to read
 */
void I2C_Master::readAtEnd(uint8_t addr, uint8_t *dataBuff, uint16_t offset, uint8_t dataSize)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    _last_error = 0;

    bool isSuccesRequest{false};

    isSuccesRequest = Wire.requestFrom(addr, dataSize) == dataSize;

    if (!isSuccesRequest)
    {
        _last_error = 4;
        return;
    }

    for (uint8_t i = offset; i < offset + dataSize; ++i)
        dataBuff[i] = Wire.read();
}

/*!
 * @brief  Read register value. Return 0 if fail. Need to check getLastError() value in this case.
 * @param  addr
 *         I2C device address
 * @param  reg
 *         Register address
 */
uint8_t I2C_Master::readRegister(uint8_t addr, uint8_t reg)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return 0;
    }

    Wire.beginTransmission(addr);
    Wire.write(reg);
    _last_error = Wire.endTransmission();

    if (_last_error)
        return 0;

    bool isSuccesRequest{false};

    isSuccesRequest = Wire.requestFrom(addr, (uint8_t)1) == 1;

    if (!isSuccesRequest)
    {
        _last_error = 4;
        return 0;
    }

    uint8_t regData{0};

    regData = Wire.read();

    return regData;
}

/*!
 * @brief  Read data from a slow device. Wait < 100ms device data responce. getLastError() check for the action success.
 * @param  addr
 *         I2C device address
 * @param  dataBuff
 *         Pointer to the buffer where the data will be read
 * @param  dataSize
 *         How many bytes to read.
 */
void I2C_Master::waitAndRead(uint8_t addr, uint8_t *dataBuff, uint8_t dataSize)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    _last_error = 0;

    bool isSuccesRequest{false};

    isSuccesRequest = Wire.requestFrom(addr, dataSize) == dataSize;

    if (!isSuccesRequest)
    {
        _last_error = 4;
        return;
    }

    long curTime = millis();

    while (!Wire.available() || (millis() - curTime) > 100)
    {
    }

    if (Wire.available())
        for (uint8_t i = 0; i < dataSize; ++i)
            dataBuff[i] = Wire.read();
}

/*!
 * @brief  Establish a connection with the device for continuous data transfer.
 * @param  addr
 *         I2C device address.
 */
void I2C_Master::beginTransmission(uint8_t addr)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    Wire.beginTransmission(addr);
}

/*!
 * @brief  Close the last established connection
 */
void I2C_Master::endTransmission()
{
    _last_error = Wire.endTransmission();
}

/*!
 * @brief  Send byte of data to the last established connection.
 */
void I2C_Master::send(uint8_t value)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    Wire.write(value);
}

/*!
 * @brief  Send data to the last established connection.
 */
void I2C_Master::send(const uint8_t *dataPointer, uint8_t dataSize)
{
    if (!_is_inited)
    {
        log_e("I2C не ініціалізовно.");
        return;
    }

    Wire.write(dataPointer, dataSize);
}
