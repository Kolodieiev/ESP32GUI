#pragma once
#include <Arduino.h>
#include <pgmspace.h>

const char STR_SELECT[] PROGMEM = "Вибір";
const char STR_OK[] PROGMEM = "Ок";
const char STR_BACK[] PROGMEM = "Назад";
const char STR_CANCEL[] PROGMEM = "Відміна";
const char STR_MENU[] PROGMEM = "Меню";

// 
const char MON_STR[] PROGMEM = "Понеділок";
const char TUE_STR[] PROGMEM = "Вівторок";
const char WED_STR[] PROGMEM = "Середа";
const char THU_STR[] PROGMEM = "Четвер";
const char FRI_STR[] PROGMEM = "П'ятниця";
const char SAT_STR[] PROGMEM = "Субота";
const char SUN_STR[] PROGMEM = "Неділя";

const char *const DAY_OF_WEEK[] PROGMEM = {
    MON_STR,
    TUE_STR,
    WED_STR,
    THU_STR,
    FRI_STR,
    SAT_STR,
    SUN_STR};

// MENU
const char STR_MUSIC[] PROGMEM = "Музика";
const char STR_FILES[] PROGMEM = "Файли";
const char STR_WATCH[] PROGMEM = "Годинник";
const char STR_PREF[] PROGMEM = "Налаштування";
const char STR_IMPORT[] PROGMEM = "Імпорт";
const char STR_EXPORT[] PROGMEM = "Експорт";
const char STR_FIRMWARE[] PROGMEM = "Прошивка";