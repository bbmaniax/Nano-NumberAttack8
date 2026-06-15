// Buzzer.h - Buzzer driver

#pragma once

#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer
{
public:
    Buzzer(uint8_t pin);

    void Play(uint16_t frequency, uint16_t duration_ms);

private:
    uint8_t pin_;
};

#endif // BUZZER_H
