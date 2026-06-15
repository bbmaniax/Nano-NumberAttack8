// View.cpp - View for NumberAttack

#include "View.h"

#include <Arduino.h>

#include "MAX7219.h"
#include "Buzzer.h"
#include "Model.h"

constexpr uint8_t kDisplayAim = 0;
constexpr uint8_t kDisplayLife = 1;
constexpr uint8_t kDisplayLaneStart = 2;

constexpr uint8_t kSegmentLifeBase = 10;
constexpr uint8_t kCellDash = 14;

constexpr uint16_t kToneHit = 1157;
constexpr uint16_t kToneMiss = 543;
constexpr uint16_t kToneHurt = 814;
constexpr uint16_t kBeepMs = 25;
constexpr uint16_t kHurtMs = 1750;
constexpr uint16_t kUfoSweepHigh = 2276;
constexpr uint16_t kUfoSweepLow = 1723;

const uint8_t kSegmentTable[16] PROGMEM = {
    0x7E, //  0
    0x30, //  1
    0x6D, //  2
    0x79, //  3
    0x33, //  4
    0x5B, //  5
    0x5F, //  6
    0x70, //  7
    0x7F, //  8
    0x7B, //  9
    0x15, // 10: UFO
    0x08, // 11: 1 life
    0x09, // 12: 2 lives
    0x49, // 13: 3 lives
    0x01, // 14: dash
    0x00, // 15: blank (kInvaderNone)
};

View::View(Model& model, MAX7219& display, Buzzer& buzzer)
    : model_(model), display_(display), buzzer_(buzzer)
{
}

void View::Begin()
{
    display_.begin();
}

void View::PlaySound(Model::Sound sound)
{
    switch (sound)
    {
    case Model::Sound::kHit:
        buzzer_.Play(kToneHit, kBeepMs);
        break;
    case Model::Sound::kMiss:
        buzzer_.Play(kToneMiss, kBeepMs);
        break;
    case Model::Sound::kHurt:
        buzzer_.Play(kToneHurt, kHurtMs);
        break;
    case Model::Sound::kUfo:
        for (uint8_t i = 0; i < 13; i++)
        {
            buzzer_.Play(kUfoSweepHigh - (uint16_t)((kUfoSweepHigh - kUfoSweepLow) * i / 12), 25);
        }
        break;
    case Model::Sound::kNone:
    default:
        break;
    }
}

void View::PlayClear()
{
    buzzer_.Play(1568, 140);
    buzzer_.Play(2093, 140);
    buzzer_.Play(3136, 220);
}

void View::PlayWin()
{
    for (uint8_t i = 0; i < 13; i++)
    {
        buzzer_.Play(1568 + (uint16_t)((3136u - 1568u) * i / 12), 45);
    }
}

void View::PlayLose()
{
    for (uint8_t i = 0; i < 6; i++)
    {
        buzzer_.Play((i % 2 == 0) ? 2341 : 815, 195);
    }
}

uint8_t View::SegmentOf(uint8_t value) const
{
    return pgm_read_byte(&kSegmentTable[value]);
}

void View::DrawPattern(uint8_t part, uint8_t pattern)
{
    display_.setDigit(0, SegmentOf(part));
    display_.setDigit(1, SegmentOf(kCellDash));
    display_.setDigit(2, SegmentOf(pattern));
    for (uint8_t d = 3; d < 8; d++)
    {
        display_.setDigit(d, SegmentOf(Model::kInvaderNone));
    }
}

void View::DrawScore(uint16_t value)
{
    display_.setDigit(0, SegmentOf(Model::kInvaderNone));
    display_.setDigit(1, SegmentOf(Model::kInvaderNone));
    display_.setDigit(7, SegmentOf(0));
    for (int8_t d = 6; d >= 2; d--)
    {
        display_.setDigit(d, SegmentOf(value % 10));
        value /= 10;
    }
}

void View::RenderHighPattern()
{
    uint8_t progress = model_.GetHighProgress();
    if (progress == 0)
    {
        for (uint8_t d = 0; d < 8; d++)
        {
            display_.setDigit(d, SegmentOf(Model::kInvaderNone));
        }
    }
    else
    {
        DrawPattern(progress >> 4, progress & 0x0F);
    }
}

void View::RenderHighScore()
{
    DrawScore(model_.GetHighScore());
}

void View::RenderBoard()
{
    display_.setDigit(kDisplayAim, SegmentOf(model_.GetAim()));
    uint8_t lives = model_.GetLives();
    display_.setDigit(kDisplayLife, SegmentOf(lives == 0 ? Model::kInvaderNone : kSegmentLifeBase + lives));
    for (uint8_t i = 0; i < Model::kLaneLength; i++)
    {
        display_.setDigit(kDisplayLaneStart + i, SegmentOf(model_.GetCell(i)));
    }
}

void View::RenderPattern()
{
    DrawPattern(model_.GetPart(), model_.GetPattern());
}

void View::RenderScore()
{
    DrawScore(model_.GetScore());
}

void View::DrawPatternScore(uint8_t pattern, uint16_t value)
{
    display_.setDigit(0, SegmentOf(pattern));
    display_.setDigit(1, SegmentOf(kCellDash));
    display_.setDigit(7, SegmentOf(0));
    for (int8_t d = 6; d >= 2; d--)
    {
        display_.setDigit(d, SegmentOf(value % 10));
        value /= 10;
    }
}

void View::RenderResult()
{
    DrawPatternScore(model_.GetPattern(), model_.GetScore());
}

void View::DrawIntro(uint8_t invaders, uint8_t shots)
{
    display_.setDigit(0, SegmentOf(Model::kInvaderNone));
    display_.setDigit(1, SegmentOf(Model::kInvaderNone));
    display_.setDigit(2, SegmentOf(Model::kInvaderNone));
    display_.setDigit(3, SegmentOf(invaders / 10));
    display_.setDigit(4, SegmentOf(invaders % 10));
    display_.setDigit(5, SegmentOf(kCellDash));
    display_.setDigit(6, SegmentOf(shots / 10));
    display_.setDigit(7, SegmentOf(shots % 10));
}

void View::RenderIntro()
{
    DrawIntro(Model::kPatternInvaders, Model::kPatternShots);
}
