#pragma once
#include <Arduino.h>

class Input
{
public:
    enum KeyCode
    {
        KEY_CODE_NONE = 0,
        // Піни кнопок, які необхідно замінити на свої.
        KEY_CODE_OK = 14,
        KEY_CODE_UP = 13,
        KEY_CODE_DOWN = 27,
        KEY_CODE_LEFT = 33,
        KEY_CODE_RIGHT = 32
    };

#pragma region "don't touch this"
    enum KeyState
    {
        KEY_STATE_NONE = 0,
        KEY_STATE_CLICKED,
        KEY_STATE_PRESSED,
    };

    struct KeyEvent
    {
        KeyCode code = KEY_CODE_NONE;
        KeyState state = KEY_STATE_NONE;
    };

    Input();
    void update();
    bool hasEvent() const { return _has_event; }
    KeyEvent getEvent();
#pragma endregion "don't touch this"

private:
    const bool READ_AS_TOUCH{true}; // Прослуховувати пін, як сенсор.

#pragma region "don't touch this"
    const uint16_t PRESS_DELAY_TIME{1000}; // Час утримання піну, після якого KEY_STATE_CLICKED перейде до KEY_STATE_PRESSED.
    const uint16_t DEBOUNCE_TIME{150};     // Час антибрязчяння контакту.

    bool _has_event{false};
    KeyEvent _event;
    unsigned long _event_time;

    KeyCode readButtons();

    const uint8_t TRESHOLD{70}; // Чутливість сенсорних пінів.
    KeyCode readSensor();

#pragma endregion "don't touch this"
};