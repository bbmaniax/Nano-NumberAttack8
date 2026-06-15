// Nano-NumberAttack8

#include <Arduino.h>

#include <DigitalButton.h>
#include "MAX7219.h"
#include "Random.h"
#include "Buzzer.h"
#include "Model.h"
#include "View.h"

constexpr uint8_t kPinButton1 = 6;
constexpr uint8_t kPinButton2 = A3;
constexpr uint8_t kPinBuzzer = 8;
constexpr uint8_t kPinLedsDataIn = A0;
constexpr uint8_t kPinLedsChipSelect = A1;
constexpr uint8_t kPinLedsClock = A2;

constexpr uint8_t kDeviceCount = 1;

constexpr uint16_t kResultToggleMs = 1500;
constexpr uint16_t kClearTotalMs = 3000;
constexpr uint16_t kIntroMs = 2000;
constexpr uint16_t kHurtShowMs = 2200;

MAX7219 display(kPinLedsDataIn, kPinLedsClock, kPinLedsChipSelect, kDeviceCount);
DigitalButton button_aim(kPinButton2, true);
DigitalButton button_fire(kPinButton1, true);
Buzzer buzzer(kPinBuzzer);

uint8_t lane[Model::kLaneLength];
Model model(lane);
View view(model, display, buzzer);

void setup()
{
  button_aim.begin();
  button_fire.begin();

  model.Begin();
  view.Begin();

  model.SetEasy(digitalRead(kPinButton1) == LOW && digitalRead(kPinButton2) == LOW);
}

void loop()
{
    Demo();
    if (Game())
    {
        Win();
    }
    else
    {
        Lose();
    }
}

void Demo()
{
    WaitClickToggling(true);
    rngSeed(millis());
}

bool Game()
{
    model.ResetScore();
    uint8_t max_loop = model.IsEasy() ? 1 : Model::kMaxLoop;
    for (uint8_t loop = 1; loop <= max_loop; loop++)
    {
        for (uint8_t part = 1; part <= Model::kMaxPart; part++)
        {
            model.SetPart(part);
            for (uint8_t pattern = 1; pattern <= Model::kMaxPattern; pattern++)
            {
                model.SetPattern(pattern);
                if (!RunPattern())
                {
                    model.CommitHighScore();
                    return false;
                }
                view.PlayClear();
                WaitClearToggling();
            }
        }
    }
    model.CommitHighScore();
    return true;
}

void Win()
{
    view.PlayWin();
    view.RenderResult();
    WaitClickShowing();
}

void Lose()
{
    view.PlayLose();
    view.RenderResult();
    WaitClickShowing();
}

bool RunPattern()
{
    view.RenderIntro();
    delay(kIntroMs);

    model.BeginPattern(millis());
    view.RenderBoard();

    for (;;)
    {
        button_aim.update();
        button_fire.update();

        if (model.IsEasy() && button_aim.isPressed() && button_fire.isPressed())
        {
            WaitClick();
            model.ResumeTimer(millis());
            continue;
        }

        Model::Status status = model.Step(button_aim.isClicked(), button_fire.isClicked(), millis());
        Model::Sound sound = model.ConsumeSound();
        view.PlaySound(sound);
        if (status == Model::Status::kCleared)
        {
            return true;
        }
        if (status == Model::Status::kGameOver)
        {
            return false;
        }
        if (sound == Model::Sound::kHurt)
        {
            view.RenderScore();
            delay(kHurtShowMs);
            view.RenderBoard();
            model.ResumeTimer(millis());
        }
        if (model.ConsumeDirty())
        {
            view.RenderBoard();
        }
    }
}

void WaitClickShowing()
{
    WaitRelease();

    for (;;)
    {
        button_aim.update();
        button_fire.update();
        if (button_aim.isClicked() || button_fire.isClicked())
            return;
    }
}

void WaitRelease()
{
    do
    {
        button_aim.update();
        button_fire.update();
    } while (button_aim.isPressed() || button_fire.isPressed());
}

void WaitClick()
{
    WaitRelease();

    for (;;)
    {
        button_aim.update();
        button_fire.update();
        if (button_aim.isClicked() || button_fire.isClicked())
            return;
    }
}

void RenderResultFace(bool demo, bool show_score)
{
    if (demo && show_score)
    {
        view.RenderHighScore();
    }
    else if (demo)
    {
        view.RenderHighPattern();
    }
    else if (show_score)
    {
        view.RenderScore();
    }
    else
    {
        view.RenderPattern();
    }
}

void WaitClickToggling(bool demo)
{
    WaitRelease();

    if (demo)
    {
        view.RenderHighScore();
        for (;;)
        {
            button_aim.update();
            button_fire.update();
            if (button_aim.isClicked() || button_fire.isClicked())
                return;
        }
    }

    bool show_score = false;
    RenderResultFace(demo, show_score);
    uint32_t toggle_time = millis();

    for (;;)
    {
        button_aim.update();
        button_fire.update();
        if (button_aim.isClicked() || button_fire.isClicked())
            return;
        if (millis() - toggle_time >= kResultToggleMs)
        {
            toggle_time = millis();
            show_score = !show_score;
            RenderResultFace(demo, show_score);
        }
    }
}

void WaitClearToggling()
{
    view.RenderResult();
    delay(kClearTotalMs);
}
