#pragma once

#include <Arduino.h>

class I2C_Master
{
public:
    ~I2C_Master();
    void begin();
    uint8_t getLastError();
    bool hasConnection(uint8_t addr);
    void write(uint8_t addr, const uint8_t *data_ptr, uint8_t data_size);
    void writeRegister(uint8_t addr, uint8_t reg, uint8_t value);
    void read(uint8_t addr, uint8_t *data_buff, uint8_t data_size);
    void readAtEnd(uint8_t addr, uint8_t *data_buff, uint16_t offset, uint8_t data_size);
    uint8_t readRegister(uint8_t addr, uint8_t reg);
    void waitAndRead(uint8_t addr, uint8_t *data_buff, uint8_t data_size);
    void beginTransmission(uint8_t addr);
    void endTransmission();
    void send(uint8_t value);
    void send(const uint8_t *data_ptr, uint8_t data_size);

private:
    uint8_t _last_error;
    bool _is_inited{false};
};
