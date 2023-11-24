#pragma once
#include <Arduino.h>
#include <pgmspace.h>

const char STR_AUTHOR[] PROGMEM = "Автор: Kolodieiev";
const char STR_SELECT[] PROGMEM = "Вибір";
const char STR_OK[] PROGMEM = "Ок";
const char STR_UPDATE[] PROGMEM = "Оновити";
const char STR_BACK[] PROGMEM = "Назад";
const char STR_CANCEL[] PROGMEM = "Відміна";
const char STR_UPDATING[] PROGMEM = "Оновлення...";
const char STR_PREPARE[] PROGMEM = "Підготовка...";
const char STR_OFFLINE[] PROGMEM = "Оффлайн";
const char STR_MOVE[] PROGMEM = "Перемістити";
const char STR_PASTE[] PROGMEM = "Вставити";
const char STR_ITEM_REMOVE[] PROGMEM = "Видалити";
const char STR_MENU[] PROGMEM = "Меню";

//
const char STR_NEW_DIR[] PROGMEM = "Новий кат.";
const char STR_TORCH[] PROGMEM = "Ліхтарик";

//
const char STR_EXPORT_SCREEN_TITLE[] PROGMEM = "Передати файл";
const char STR_IMPORT_SCREEN_TITLE[] PROGMEM = "Отримати файл";
const char STR_UPDATE_SCREEN_TITLE[] PROGMEM = "Прошивка";

// MUSIC
const char STR_CONTINUE[] PROGMEM = "Продовжити";
const char STR_UPDATE_PLAYLISTS[] PROGMEM = "Оновити плейлисти";
const char STR_STOPPED[] PROGMEM = "Зупинено";

const char STR_PLAY_BTN[] PROGMEM = ">";
const char STR_PAUSE_BTN[] PROGMEM = "::";
const char STR_PREV_BTN[] PROGMEM = "<<";
const char STR_NEXT_BTN[] PROGMEM = ">>";

const char STR_POS_TIME[] PROGMEM = "000:00";
const char STR_TRACK_TIME[] PROGMEM = "/000:00";

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

// PREF
const char STR_BRIGHT[] PROGMEM = "Яскравість";