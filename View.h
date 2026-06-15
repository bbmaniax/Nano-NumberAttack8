// View.h - View for NumberAttack

#pragma once

#ifndef VIEW_H
#define VIEW_H

#include <Arduino.h>

#include "Model.h"

class MAX7219;
class Buzzer;

class View
{
public:
    View(Model& model, MAX7219& display, Buzzer& buzzer);

    void Begin();

    void RenderBoard();
    void RenderPattern();
    void RenderScore();
    void RenderResult();
    void RenderIntro();
    void RenderHighPattern();
    void RenderHighScore();

    void PlaySound(Model::Sound sound);
    void PlayClear();
    void PlayWin();
    void PlayLose();

private:
    void DrawPattern(uint8_t part, uint8_t pattern);
    void DrawScore(uint16_t value);
    void DrawPatternScore(uint8_t pattern, uint16_t value);
    void DrawIntro(uint8_t invaders, uint8_t shots);
    uint8_t SegmentOf(uint8_t value) const;

    Model& model_;
    MAX7219& display_;
    Buzzer& buzzer_;
};

#endif // VIEW_H
