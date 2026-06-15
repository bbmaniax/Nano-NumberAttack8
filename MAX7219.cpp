// MAX7219 - MAX7219 driver

#include <Arduino.h>
#include "MAX7219.h"

constexpr uint8_t REG_NOOP        = 0x00;
constexpr uint8_t REG_DIGIT0      = 0x01;
constexpr uint8_t REG_DECODE_MODE = 0x09;
constexpr uint8_t REG_INTENSITY   = 0x0A;
constexpr uint8_t REG_SCAN_LIMIT  = 0x0B;
constexpr uint8_t REG_SHUTDOWN    = 0x0C;
constexpr uint8_t REG_DISP_TEST   = 0x0F;

MAX7219::MAX7219(uint8_t pinDIN, uint8_t pinCLK, uint8_t pinCS, uint8_t numDevices) :
    _pinDIN(pinDIN),
    _pinCLK(pinCLK),
    _pinCS(pinCS),
    _numDevices(numDevices)
{
}

void MAX7219::begin()
{
    pinMode(_pinDIN, OUTPUT);
    pinMode(_pinCLK, OUTPUT);
    pinMode(_pinCS, OUTPUT);
    digitalWrite(_pinDIN, LOW);
    digitalWrite(_pinCLK, LOW);
    digitalWrite(_pinCS, HIGH);
    delay(100);

    digitalWrite(_pinCS, LOW);
    delayMicroseconds(10);
    for (uint8_t i = 0; i < _numDevices; i++) {
        shiftOut16(REG_NOOP, 0x00);
    }
    delayMicroseconds(10);
    digitalWrite(_pinCS, HIGH);
    delayMicroseconds(100);

    writeRegisterAll(REG_DISP_TEST,   0x00);
    writeRegisterAll(REG_SHUTDOWN,    0x01);
    writeRegisterAll(REG_SCAN_LIMIT,  0x07);
    writeRegisterAll(REG_DECODE_MODE, 0x00);
    writeRegisterAll(REG_INTENSITY,   0x07);

    clear();
}

void MAX7219::setDigit(uint8_t digit, uint8_t value)
{
    uint8_t device = digit / 8;
    uint8_t pos    = 7 - (digit % 8);
    writeRegister(device, REG_DIGIT0 + pos, value);
}

void MAX7219::clear()
{
    for (uint8_t digit = 0; digit < 8; digit++) {
        writeRegisterAll(REG_DIGIT0 + digit, 0x00);
    }
}

void MAX7219::setBrightness(uint8_t level)
{
    if (level > 15) level = 15;
    writeRegisterAll(REG_INTENSITY, level);
}

void MAX7219::writeRegisterAll(uint8_t reg, uint8_t data)
{
    digitalWrite(_pinCS, LOW);
    delayMicroseconds(10);
    for (uint8_t i = 0; i < _numDevices; i++) {
        shiftOut16(reg, data);
    }
    delayMicroseconds(10);
    digitalWrite(_pinCS, HIGH);
    delayMicroseconds(50);
}

void MAX7219::writeRegister(uint8_t device, uint8_t reg, uint8_t data)
{
    digitalWrite(_pinCS, LOW);
    delayMicroseconds(10);
    for (uint8_t i = 0; i < _numDevices; i++) {
        uint8_t d = (_numDevices - 1 - i);
        if (d == device) {
            shiftOut16(reg, data);
        } else {
            shiftOut16(REG_NOOP, 0x00);
        }
    }
    delayMicroseconds(10);
    digitalWrite(_pinCS, HIGH);
    delayMicroseconds(50);
}

void MAX7219::shiftOut16(uint8_t reg, uint8_t data)
{
    uint8_t bytes[2] = {reg, data};
    for (uint8_t b = 0; b < 2; b++) {
        for (int8_t i = 7; i >= 0; i--) {
            digitalWrite(_pinDIN, (bytes[b] >> i) & 0x01);
            delayMicroseconds(1);
            digitalWrite(_pinCLK, HIGH);
            delayMicroseconds(1);
            digitalWrite(_pinCLK, LOW);
            delayMicroseconds(1);
        }
    }
}
