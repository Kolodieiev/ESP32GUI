#include "ESP32GUI.h"

// ------- Екрани
#include "lib/GUI/IScreen.h"
#include "screen/home/HomeScreen.h"
#include "screen/menu/MenuScreen.h"

ESP32GUI GUI;

void ESP32GUI::run()
{
    // Ініціалізація та налаштування драйвера дисплею.
    _tft.init();
    _tft.setRotation(1);

    // Встановлення стартового екрану.
    IScreen *screen = new HomeScreen(_tft);
    screen->show();

    // Основний цикл GUI.
    while (1)
    {
        if (!screen->isReleased())
            screen->tick();
        else
        {
            delete screen;
            switch (screen->getNextScreenID())
            {
            // Відредагуй відповідно зі своєю задачею.
            // ---------------------------------------
            case IScreen::ID_SCREEN_HOME:
                screen = new HomeScreen(_tft);
                break;
            case IScreen::ID_SCREEN_MENU:
                screen = new MenuScreen(_tft);
                break;
                // case IScreen::ID_SCREEN_SOME:
                //     screen = new SomeScreen(_tft);
                //     break;
                // ------------------------------------
            }
            screen->show();
        }
    }
}
