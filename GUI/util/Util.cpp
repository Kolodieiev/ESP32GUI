#include "Util.h"

// Зміни пін у функції, який відповідає за яскравість дисплея
void Util::setBrightness(uint8_t value)
{
    // const uint8_t PWM_PIN{17};
    const uint8_t PWM_PIN{4};

    const uint16_t PWM_FREQ{3000};
    const uint8_t PWM_CHANEL{0};
    const uint8_t PWM_RESOLUTION{8};

    ledcSetup(PWM_CHANEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANEL);
    ledcWrite(PWM_CHANEL, value);
}