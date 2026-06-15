// Model.h - Model for NumberAttack

#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <Arduino.h>

class Model
{
public:
    static constexpr uint8_t kLaneLength = 6;
    static constexpr uint8_t kInvaderNone = 0x0F;
    static constexpr uint8_t kUfoNumber = 10;
    static constexpr uint8_t kInitialLives = 3;
    static constexpr uint8_t kPatternInvaders = 16;
    static constexpr uint8_t kPatternShots = 30;
    static constexpr uint8_t kMaxPattern = 9;
    static constexpr uint8_t kMaxPart = 2;
    static constexpr uint8_t kMaxLoop = 10;

    enum class Status : uint8_t
    {
        kPlaying,
        kCleared,
        kGameOver,
    };

    enum class Sound : uint8_t
    {
        kNone,
        kHit,
        kMiss,
        kHurt,
        kUfo,
    };

    Model(uint8_t (&lane)[kLaneLength]);

    void Begin();

    void ResetScore();
    void SetPart(uint8_t value);
    void SetPattern(uint8_t value);
    void SetEasy(bool value);
    bool IsEasy() const;

    void BeginPattern(uint32_t now);
    void ResumeTimer(uint32_t now);
    Status Step(bool aim_clicked, bool fire_clicked, uint32_t now);
    bool ConsumeDirty();
    Sound ConsumeSound();

    void CommitHighScore();

    uint8_t GetCell(uint8_t index) const;
    uint8_t GetAim() const;
    uint8_t GetLives() const;
    uint16_t GetScore() const;
    uint8_t GetPart() const;
    uint8_t GetPattern() const;
    uint16_t GetHighScore() const;
    uint8_t GetHighProgress() const;

private:
    static constexpr uint8_t kAimStates = 11;
    static constexpr uint16_t kUfoScore = 300;
    static constexpr uint16_t kSpeedBase = 1220;
    static constexpr uint16_t kSpeedRange = 830;
    static constexpr uint16_t kSpeedRangeEasy = 440;

    void Fire();
    bool Advance();

    uint8_t (&lane_)[kLaneLength];
    uint8_t aim_;
    uint8_t lives_;
    uint16_t score_;
    uint8_t part_;
    uint8_t pattern_;
    uint16_t high_score_;
    uint8_t high_progress_;
    uint32_t last_move_time_;
    uint16_t move_interval_;
    uint8_t target_count_;
    uint8_t hit_;
    uint8_t shot_count_;
    uint8_t sum_;
    uint8_t sum_old_;
    uint8_t ufo_stack_;
    bool easy_;
    Sound last_sound_;
    bool dirty_;
};

#endif // MODEL_H
