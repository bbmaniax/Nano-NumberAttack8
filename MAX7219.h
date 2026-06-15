// MAX7219 - MAX7219 driver

#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>

class MAX7219
{
public:
    MAX7219(uint8_t pinDIN, uint8_t pinCLK, uint8_t pinCS, uint8_t numDevices = 1);

public:
    void begin();

    void setDigit(uint8_t digit, uint8_t value);
    void clear();
    void setBrightness(uint8_t level);

private:
    void writeRegisterAll(uint8_t reg, uint8_t data);
    void writeRegister(uint8_t device, uint8_t reg, uint8_t data);
    void shiftOut16(uint8_t reg, uint8_t data);

    uint8_t _pinDIN;
    uint8_t _pinCLK;
    uint8_t _pinCS;
    uint8_t _numDevices;
};

#endif // MAX7219_H
