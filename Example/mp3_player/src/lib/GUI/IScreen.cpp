#include "IScreen.h"
#include "widget/layout/EmptyLayout.h"

void IScreen::tick()
{
#pragma region "don't touch this"
    loop();

    // Регулювання частоти оновлення екрану і зчитування кнопок
    if ((millis() - _last_upd_time) > DELAY_BTW_GUI_UPDATE)
    {
        _last_upd_time = millis();

#pragma endregion "don't touch this"

        // Ви можете написати свій варінт взаємодії з мікроконтроллером та інтегрувати його нижче.

        _input.update();

        if (_input.hasEvent())
        {
            Input::KeyEvent event = _input.getEvent();

            if (event.state == Input::KEY_STATE_CLICKED)
            {
                switch (event.code)
                {
                case Input::KEY_CODE_OK:
                    okClickedHandler();
                    break;
                case Input::KEY_CODE_UP:
                    upClickedHandler();
                    break;
                case Input::KEY_CODE_DOWN:
                    downClickedHandler();
                    break;
                case Input::KEY_CODE_LEFT:
                    leftClickedHandler();
                    break;
                case Input::KEY_CODE_RIGHT:
                    rightClickedHandler();
                    break;
                default:
                    break;
                }
            }
            else if (event.state == Input::KEY_STATE_PRESSED)
            {
                switch (event.code)
                {
                case Input::KEY_CODE_OK:
                    okPressedHandler();
                    break;
                case Input::KEY_CODE_UP:
                    upPressedHandler();
                    break;
                case Input::KEY_CODE_DOWN:
                    downPressedHandler();
                    break;
                case Input::KEY_CODE_LEFT:
                    leftPressedHandler();
                    break;
                case Input::KEY_CODE_RIGHT:
                    rightPressedHandler();
                    break;
                default:
                    break;
                }
            }
        }

#pragma region "don't touch this"
        else
            doGUIWork();

        _tft.startWrite();
        _layout->onDraw();
        _tft.endWrite();
    }
#pragma endregion "don't touch this"
}

#pragma region "don't touch this"

/// @brief
void IScreen::show()
{
    if (_layout != nullptr)
    {
        _tft.startWrite();
        _layout->onDraw();
        _tft.endWrite();
    }
    else
        log_e("Layout не встановлено.");
}

IScreen::IScreen(TFT_eSPI &tft) : _tft{tft}
{
    _layout = new EmptyLayout(1, _tft);
    _layout->setBackColor(TFT_YELLOW);
    _layout->setWidth(_tft.width());
    _layout->setHeight(_tft.height());
}

IScreen::~IScreen()
{
    delete _layout;
}

void IScreen::setLayout(IWidgetContainer *layout)
{
    if (layout == nullptr)
    {
        log_e("Спроба встановити NULL-layout.");
        return;
    }

    if (_layout == layout)
        return;

    delete _layout;
    _layout = layout;
}

void IScreen::openScreenByID(ScreenID screen_ID)
{
    _next_screen_ID = screen_ID;
    _is_released = true;
}

#pragma endregion "don't touch this"