#include "HAL/HAL.h"
#include "App/Utils/ButtonEvent/ButtonEvent.h"
#include "App/Accounts/Account_Master.h"

static ButtonEvent ButtonPushA(5000), ButtonPushB(5000), ButtonPushC(5000);

static bool ButtonEnable = true;

Account* actButtons;

static void Button_PushHandlerA(ButtonEvent* btn, int event)
{
    if (event == ButtonEvent::EVENT_PRESSED)
    {
        HAL::Buzz_Tone(500, 20);

    } else if (event == ButtonEvent::EVENT_RELEASED)
    {
        HAL::Buzz_Tone(700, 20);

    } else if (event == ButtonEvent::EVENT_LONG_PRESSED)
    {

    }
}

static void Button_PushHandlerB(ButtonEvent* btn, int event)
{
    if (event == ButtonEvent::EVENT_PRESSED)
    {
        HAL::Buzz_Tone(500, 20);

    } else if (event == ButtonEvent::EVENT_RELEASED)
    {
        HAL::Buzz_Tone(700, 20);

    } else if (event == ButtonEvent::EVENT_LONG_PRESSED)
    {
        HAL::Audio_PlayMusic("Shutdown");
        HAL::Power_Shutdown();
    }
}

static void Button_PushHandlerC(ButtonEvent* btn, int event)
{
    if (event == ButtonEvent::EVENT_PRESSED)
    {
        HAL::Buzz_Tone(500, 20);

    } else if (event == ButtonEvent::EVENT_RELEASED)
    {
        HAL::Buzz_Tone(700, 20);

    } else if (event == ButtonEvent::EVENT_LONG_PRESSED)
    {

    }
}

static void Button_RotateHandler(int16_t diff)
{
    HAL::Buzz_Tone(300, 5);
    actButtons->Commit((const void*) &diff, sizeof(int16_t));
    actButtons->Publish();
}

bool HAL::Button_GetIsPushA()
{
    if (!ButtonEnable)
    {
        return false;
    }
    // return the right button
    return (digitalRead(CONFIG_BUTTON_A) == LOW);

}

bool HAL::Button_GetIsPushB()
{
    if (!ButtonEnable)
    {
        return false;
    }
    // return the right button
    return (digitalRead(CONFIG_BUTTON_B) == LOW);

}

bool HAL::Button_GetIsPushC()
{
    if (!ButtonEnable)
    {
        return false;
    }
    // return the right button
    return (digitalRead(CONFIG_BUTTON_C) == LOW);

}

void HAL::Button_Init()
{
    pinMode(CONFIG_BUTTON_A, INPUT_PULLUP);
    pinMode(CONFIG_BUTTON_B, INPUT_PULLUP);
    pinMode(CONFIG_BUTTON_C, INPUT_PULLUP);

    // attachInterrupt(CONFIG_ENCODER_A_PIN, Encoder_IrqHandler, CHANGE);

    ButtonPushA.EventAttach(Button_PushHandlerA);
    ButtonPushB.EventAttach(Button_PushHandlerB);
    ButtonPushC.EventAttach(Button_PushHandlerC);

    actButtons = new Account("Encoder", AccountSystem::Broker(), sizeof(int16_t), nullptr);

}

void HAL::Button_Update()
{
    ButtonPushA.EventMonitor(Button_GetIsPush());
}

int8_t HAL::Button_GetIsPush()
{
    if (!ButtonEnable)
    {
        return false;
    }
    if (digitalRead(CONFIG_BUTTON_A) == LOW)
        return 1;
    // return the right button
    else if (digitalRead(CONFIG_BUTTON_B) == LOW)
        return 2;
    else if (digitalRead(CONFIG_BUTTON_C) == LOW)
        return 3;
    else
        return -1;
}

void HAL::Button_SetEnable(bool en)
{
    ButtonEnable = en;
}
