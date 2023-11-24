#include "PrefManager.h"

bool PrefManager::setPref(const char* file_name, const char* value)
{
    File *pref_file = getPrefFile(file_name, "w");

    if (!pref_file)
    {
        log_e("Помилка отримання дескриптору файлу.");
        SD.end();
        return false;
    }

    pref_file->print(value);
    pref_file->close();

    SD.end();

    return true;
}

String PrefManager::getPref(const char* pref_name)
{
    File *pref_file = getPrefFile(pref_name, "r");

    if (!pref_file)
    {
        log_e("Помилка отримання дескриптору файлу.");
        SD.end();
        return "";
    }

    String temp = pref_file->readString();
    pref_file->close();

    SD.end();

    return temp;
}

File *PrefManager::getPrefFile(const char* file_name, const char *mode)
{
    if (!SD.begin(PIN_SD_CS))
    {
        log_e("Карту пам'яті не примонтовано");
        return NULL;
    }

    if (!SD.exists(PREF_DIR_NAME))
        SD.mkdir(PREF_DIR_NAME);

    File pref_dir = SD.open(PREF_DIR_NAME, mode);

    if (!pref_dir)
    {
        log_e("Помилка читання директорії налаштувань.");
        return NULL;
    }

    if (!pref_dir.isDirectory())
    {
        log_e("Помилка. Дескриптор не є директорією.");
        pref_dir.close();
        return NULL;
    }

    pref_dir.close();

    String pref_file_name = PREF_DIR_NAME;
    pref_file_name += "/";
    pref_file_name += file_name;

    File pref_file = SD.open(pref_file_name, mode);

    if (!pref_file)
    {
        log_e("Помилка відкриття файлу налаштувань.");
        return NULL;
    }

    if (pref_file.isDirectory())
    {
        log_e("Помилка. Дескриптор файлу є каталогом.");
        pref_file.close();
        return NULL;
    }

    return new File(pref_file);
}
