#include <Arduino.h>
#include "lib/GUI/util/InitSerial.h" // Ініціалізація Serial раніше за інші класи.
#include "lib/pref/PrefManager.h"    // Управління налаштуваннями
#include "lib/GUI/util/Util.h"       //Управління яскравістю дисплея
#include "lib/GUI/ESP32GUI.h"           // Підключити GUI.

void screenTask(void *params)
{
  // Налаштування параметрів дисплею, які не відносяться безпосередньо до інтерфейсу.
  PrefManager pref;
  String bright = pref.getPref(STR_BRIGHT_PREF);

  if (bright.equals(""))
    Util::setBrightness(100);
  else
    Util::setBrightness(atoi(bright.c_str()));

  // Необхідно налаштувати GUI перед запуском. Читай README.
  GUI.run();
}

void setup()
{
  // Створювати задачу для GUI рекомендується саме на ядрі 1. Так як на 0 працює wifi і bt.
  xTaskCreatePinnedToCore(screenTask, "screenTask", 1024 * 15, NULL, 10, NULL, 1);
}

void loop()
{
  vTaskDelete(NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}