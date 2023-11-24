#include "Input.h"

Input::Input()
{
    if (!READ_AS_TOUCH)
    {
        pinMode(KEY_CODE_OK, INPUT_PULLUP);
        pinMode(KEY_CODE_UP, INPUT_PULLUP);
        pinMode(KEY_CODE_DOWN, INPUT_PULLUP);
        pinMode(KEY_CODE_LEFT, INPUT_PULLUP);
        pinMode(KEY_CODE_RIGHT, INPUT_PULLUP);
    }
}

Input::KeyCode Input::readButtons()
{
    if (!digitalRead(KEY_CODE_OK))
        return KEY_CODE_OK;
    else if (!digitalRead(KEY_CODE_UP))
        return KEY_CODE_UP;
    else if (!digitalRead(KEY_CODE_DOWN))
        return KEY_CODE_DOWN;
    else if (!digitalRead(KEY_CODE_LEFT))
        return KEY_CODE_LEFT;
    else if (!digitalRead(KEY_CODE_RIGHT))
        return KEY_CODE_RIGHT;
    else
        return KEY_CODE_NONE;
}

Input::KeyCode Input::readSensor()
{
    if (touchRead(KEY_CODE_OK) < TRESHOLD)
        return KEY_CODE_OK;
    else if (touchRead(KEY_CODE_UP) < TRESHOLD)
        return KEY_CODE_UP;
    else if (touchRead(KEY_CODE_DOWN) < TRESHOLD)
        return KEY_CODE_DOWN;
    else if (touchRead(KEY_CODE_LEFT) < TRESHOLD)
        return KEY_CODE_LEFT;
    else if (touchRead(KEY_CODE_RIGHT) < TRESHOLD)
        return KEY_CODE_RIGHT;
    else
        return KEY_CODE_NONE;
}

#pragma region "don't touch this"
void Input::update()
{
    if ((millis() - _event_time) < DEBOUNCE_TIME)
        return;

    KeyCode code;

    if (READ_AS_TOUCH)
        code = readSensor();
    else
        code = readButtons();

    if (_event.code == KEY_CODE_NONE)
    {
        // Якщо не зафіксовано click
        if (code)
        {
            // Якщо зараз зафіксовано click
            _event.code = code;
            _event_time = millis();
        }
    }
    else if (code == KEY_CODE_NONE)
    {
        // Якщо раніше було зафіксовано click, але тепер кнопка відпущена
        if (_event.state == KEY_STATE_PRESSED)
        {
            _event.state = KEY_STATE_NONE;
            _event.code = KEY_CODE_NONE;
        }
        else
        {
            _event.state = KEY_STATE_CLICKED;
            _has_event = true;
        }

        _event_time = millis();
    }
    else if (_event.code == code)
    {
        // Якщо утримується одна і та ж кнопка
        if ((millis() - _event_time) > PRESS_DELAY_TIME)
        {
            _event.state = KEY_STATE_PRESSED;
            _event_time = millis();
            _has_event = true;
        }
    }
    else
    {
        // Зафіксувати новий click і час click-у
        _has_event = false;
        _event.state = KEY_STATE_NONE;
        _event.code = code;
        _event_time = millis();
    }
}

Input::KeyEvent Input::getEvent()
{
    KeyEvent event(_event);

    _has_event = false;
    _event.code = KEY_CODE_NONE;
    _event_time = millis();

    return event;
}
#pragma endregion "don't touch this"
