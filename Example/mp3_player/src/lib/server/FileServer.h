/*
 * Дуже простий файловий http сервер.
 * Дозволяє передавати і приймати mp3 файли по wifi.
 * Взаємодія відбувається через браузер клієнта.
 * 
 * Author: Kolodieiev
 * 
 * Щоб функція вивантаження файлів працювала коректно в контексті задач,
 * Необхідно відредагувати метод size_t WiFiClient::write(Stream &stream);
 * В кінці циклу while(available) потрібно додати vTaskDelay(1);
 * Якщо цього не зробити, при спробі завантажити файл клієнтом, буде викликатися виключення.
 */

#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "WebServer.h"

class FileServer
{

public:
    FileServer();
    ~FileServer();

    bool start(bool is_master = false);
    void stop();

    bool isRunning() { return _task_params.is_running; }
    IPAddress getIP() { return  WiFi.softAPIP(); }

private:
    struct ServerTaskParams
    {
        bool is_running = false;
        bool is_master;
    } _task_params;

    static bool _can_work;
    static size_t _buffer_size;
    static WebServer *_server;

    WebServer _web_server{80};

    static void fileServerTask(void *params);
    //-------------------------------
    static void handleOnImportConnect();
    static void handleOnUpload();
    static void handleOnExportConnect();
    static void handleOn404Connect();
};
