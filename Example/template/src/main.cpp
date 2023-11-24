#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft;

void setup()
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_RED);
}

void loop()
{
}

// #include <Arduino.h>
// #include "lib/GUI/util/InitSerial.h" // Ініціалізація Serial раніше за інші класи.
// #include "lib/GUI/ESP32GUI.h"        // Підключити GUI.

// void screenTask(void *params)
// {
//   GUI.run();
// }

// void setup()
// {
//   // Створювати задачу для GUI рекомендується саме на ядрі 1. Так як на 0 працює wifi і bt.
//   xTaskCreatePinnedToCore(screenTask, "screenTask", 1024 * 15, NULL, 10, NULL, 1);
// }

// void loop()
// {
//   vTaskDelete(NULL);
//   vTaskDelay(1000 / portTICK_PERIOD_MS);
// }