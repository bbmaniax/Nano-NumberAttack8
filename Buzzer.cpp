// Buzzer.cpp - Buzzer driver

#include "Buzzer.h"

#include <Arduino.h>

Buzzer::Buzzer(uint8_t pin)
    : pin_(pin)
{
}

void Buzzer::Play(uint16_t frequency, uint16_t duration_ms)
{
    tone(pin_, frequency, duration_ms);
    delay(duration_ms);
}
