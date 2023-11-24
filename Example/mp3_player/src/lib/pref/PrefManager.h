/*
 * Бібліотека дозволяє зберігати налаштування на SD.
 * Ідея полягає в тому, що кожна пара ключ-значення зберігаються в окремому файлі.
 *
 * При великій кількості налаштувань, швидкість обробки не висока.
 *
 * Author: Kolodieiev
 */

#pragma once
#include <Arduino.h>
#include <SD.h>

const char STR_BRIGHT_PREF[] PROGMEM = "Brightness";

class PrefManager
{

public:
    PrefManager() {}
    ~PrefManager() {}

    bool setPref(const char* file_name, const char* value);
    String getPref(const char* pref_name);

private:
    const uint8_t PIN_SD_CS{5};

    File *getPrefFile(const char* file_name, const char *mode);

    const char *PREF_DIR_NAME{"/Preferences"};
};
