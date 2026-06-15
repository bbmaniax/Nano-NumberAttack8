// Model.cpp - Model for NumberAttack

#include "Model.h"

#include <Arduino.h>
#include <EEPROM.h>

#include "Random.h"

constexpr int kEepromMagic = 0;
constexpr int kEepromHighScore = 2;
constexpr int kEepromHighProgress = 4;
constexpr uint16_t kEepromMagicValue = 0xA55A;

Model::Model(uint8_t (&lane)[kLaneLength])
    : lane_(lane), aim_(0), lives_(0), score_(0),
      part_(1), pattern_(1), high_score_(0), high_progress_(0),
      last_move_time_(0), move_interval_(0), target_count_(0), hit_(0),
      shot_count_(0), sum_(0), sum_old_(0), ufo_stack_(0), easy_(false),
      last_sound_(Sound::kNone), dirty_(false)
{
    for (uint8_t i = 0; i < kLaneLength; i++)
    {
        lane_[i] = kInvaderNone;
    }
}

void Model::Begin()
{
    uint16_t magic;
    EEPROM.get(kEepromMagic, magic);
    if (magic != kEepromMagicValue)
    {
        high_score_ = 0;
        high_progress_ = 0;
        EEPROM.put(kEepromMagic, (uint16_t)kEepromMagicValue);
        EEPROM.put(kEepromHighScore, high_score_);
        EEPROM.write(kEepromHighProgress, high_progress_);
        return;
    }
    EEPROM.get(kEepromHighScore, high_score_);
    high_progress_ = EEPROM.read(kEepromHighProgress);
}

void Model::ResetScore()
{
    score_ = 0;
}

void Model::SetPart(uint8_t value)
{
    part_ = value;
}

void Model::SetPattern(uint8_t value)
{
    pattern_ = value;
}

void Model::SetEasy(bool value)
{
    easy_ = value;
}

bool Model::IsEasy() const
{
    return easy_;
}

void Model::BeginPattern(uint32_t now)
{
    aim_ = 0;
    lives_ = kInitialLives;
    target_count_ = 0;
    hit_ = 0;
    shot_count_ = 0;
    sum_ = 0;
    sum_old_ = 0;
    ufo_stack_ = 0;
    for (uint8_t i = 0; i < kLaneLength; i++)
    {
        lane_[i] = kInvaderNone;
    }
    move_interval_ = kSpeedBase - (uint16_t)((easy_ ? kSpeedRangeEasy : kSpeedRange) * (pattern_ - 1) / 8);
    last_move_time_ = now;
    dirty_ = false;
}

void Model::ResumeTimer(uint32_t now)
{
    last_move_time_ = now;
}

Model::Status Model::Step(bool aim_clicked, bool fire_clicked, uint32_t now)
{
    dirty_ = false;
    last_sound_ = Sound::kNone;

    if (aim_clicked)
    {
        aim_ = (aim_ + 1) % kAimStates;
        dirty_ = true;
    }

    bool acted = false;

    if (fire_clicked)
    {
        Fire();
        acted = true;
    }

    if (now - last_move_time_ >= move_interval_)
    {
        last_move_time_ = now;
        if (!Advance())
        {
            return Status::kGameOver;
        }
        acted = true;
    }

    if (acted)
    {
        if (hit_ >= kPatternInvaders)
        {
            return Status::kCleared;
        }
        if (shot_count_ >= kPatternShots)
        {
            return Status::kGameOver;
        }
        dirty_ = true;
    }

    return Status::kPlaying;
}

bool Model::ConsumeDirty()
{
    bool was_dirty = dirty_;
    dirty_ = false;
    return was_dirty;
}

Model::Sound Model::ConsumeSound()
{
    Sound s = last_sound_;
    last_sound_ = Sound::kNone;
    return s;
}

void Model::Fire()
{
    shot_count_++;
    uint8_t pos = kInvaderNone;
    for (uint8_t i = 0; i < kLaneLength; i++)
    {
        if (lane_[i] != kInvaderNone && lane_[i] == aim_)
        {
            pos = i;
            break;
        }
    }
    if (pos == kInvaderNone)
    {
        last_sound_ = Sound::kMiss;
        return;
    }
    uint8_t killed_number = lane_[pos];
    for (uint8_t i = pos; i > 0; i--)
    {
        lane_[i] = lane_[i - 1];
    }
    lane_[0] = kInvaderNone;
    score_ += (uint16_t)(pos + 1) * 10 * part_;
    if (killed_number == kUfoNumber)
    {
        score_ += kUfoScore;
        last_sound_ = Sound::kUfo;
    }
    else
    {
        sum_ += killed_number;
        if (sum_ > sum_old_ && sum_ % 10 == 0)
        {
            ufo_stack_++;
        }
        sum_old_ = sum_;
        last_sound_ = Sound::kHit;
    }
    hit_++;
}

bool Model::Advance()
{
    if (lane_[0] != kInvaderNone)
    {
        last_sound_ = Sound::kHurt;
        if (lives_ <= 1)
        {
            lives_ = 0;
            return false;
        }
        lives_--;
        for (uint8_t i = 0; i < kLaneLength; i++)
        {
            lane_[i] = kInvaderNone;
        }
        aim_ = 0;
        target_count_ = hit_;
        return true;
    }
    for (uint8_t i = 0; i < kLaneLength - 1; i++)
    {
        lane_[i] = lane_[i + 1];
    }
    uint8_t spawn_position = kLaneLength - part_;
    for (uint8_t i = spawn_position + 1; i < kLaneLength; i++)
    {
        lane_[i] = kInvaderNone;
    }
    if (target_count_ < kPatternInvaders)
    {
        if (ufo_stack_ > 0)
        {
            lane_[spawn_position] = kUfoNumber;
            ufo_stack_--;
        }
        else
        {
            lane_[spawn_position] = rngBelow(10);
        }
        target_count_++;
    }
    else
    {
        lane_[spawn_position] = kInvaderNone;
    }
    return true;
}

uint8_t Model::GetCell(uint8_t index) const
{
    return lane_[index];
}

uint8_t Model::GetAim() const
{
    return aim_;
}

uint8_t Model::GetLives() const
{
    return lives_;
}

uint16_t Model::GetScore() const
{
    return score_;
}

uint8_t Model::GetPart() const
{
    return part_;
}

uint8_t Model::GetPattern() const
{
    return pattern_;
}

uint16_t Model::GetHighScore() const
{
    return high_score_;
}

uint8_t Model::GetHighProgress() const
{
    return high_progress_;
}

void Model::CommitHighScore()
{
    if (score_ <= high_score_)
    {
        return;
    }
    high_score_ = score_;
    high_progress_ = (uint8_t)(part_ << 4) | pattern_;
    EEPROM.put(kEepromHighScore, high_score_);
    EEPROM.write(kEepromHighProgress, high_progress_);
}
