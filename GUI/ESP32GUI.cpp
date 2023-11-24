#include "ESP32GUI.h"

// ------- Екрани
#include "lib/GUI/IScreen.h"
#include "screen/home/HomeScreen.h"
#include "screen/menu/MenuScreen.h"
#include "screen/music/MusicScreen.h"
#include "screen/pref/PrefScreen.h"
#include "screen/watch/WatchScreen.h"
#include "screen/files/FilesScreen.h"
#include "screen/import/ImportScreen.h"
#include "screen/export/ExportScreen.h"
#include "screen/update/UpdateScreen.h"
#include "screen/flashlight/FlashScreen.h"

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
            case IScreen::ID_SCREEN_FLASH:
                screen = new FlashScreen(_tft);
                break;
            case IScreen::ID_SCREEN_MUSIC:
                screen = new MusicScreen(_tft);
                break;
            case IScreen::ID_SCREEN_PREF:
                screen = new PrefScreen(_tft);
                break;
            case IScreen::ID_SCREEN_WATCH:
                screen = new WatchScreen(_tft);
                break;
            case IScreen::ID_SCREEN_FILES:
                screen = new FilesScreen(_tft);
                break;
            case IScreen::ID_SCREEN_IMPORT:
                screen = new ImportScreen(_tft);
                break;
            case IScreen::ID_SCREEN_EXPORT:
                screen = new ExportScreen(_tft);
                break;
            case IScreen::ID_SCREEN_UPDATE:
                screen = new UpdateScreen(_tft);
                break;
            default:
                break;
                // ------------------------------------
            }
            screen->show();
        }
    }
}
