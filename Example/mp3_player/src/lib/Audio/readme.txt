/*
 * Щиро дякую Wolle за чудову бібліотеку! (https://github.com/schreibfaul1/ESP32-audioI2S)
 *
 * Відредаговано by Kolodieiev.
 *
 * Задля економії памяті, вирізав підтримку всіх кодеків, окрім mp3.
 * Вирізав підтримку всіх конектів, окрім локального файлу на SD-карті.
 * Видалена підтримка PSRAM, так як в популярних недорогих модулях ESP32 цей тип пам'яті не розпаяно.
 * Вирізав код, що був непотрібний для відтворення mp3.
 * Деякі інші зміни.
 *
 */