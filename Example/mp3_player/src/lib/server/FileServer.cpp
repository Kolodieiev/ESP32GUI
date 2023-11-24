#include "FileServer.h"
#include "SD.h"
#include <pgmspace.h>

const char HEAD_TOP_STR[] PROGMEM = "<!DOCTYPE html>"
                                    "<html><head>"
                                    "<meta name=\"viewport\" charset=\"UTF-8\" "
                                    "content=\"width=device-width, initial-scale=1.0, user-scalable=no\">"
                                    "<title>";
const char HEAD_MID_STR[] PROGMEM = "</title></head><body>";
const char FOOT_STR[] PROGMEM = "</body></html>";

const char EXPORT_TITLE_STR[] PROGMEM = "Експорт файлу";
const char IMPORT_TITLE_STR[] PROGMEM = "Імпорт файлу";
const char P_N_F_TITLE_STR[] PROGMEM = "Сторінка не знайдена";
const char NOT_FOUND_CONTENT_STR[] PROGMEM = "<h1>Запитану сторінку не знайдено</h1><br>"
                                             "<a href=\"http://192.168.4.1/\">На головну</a>";

const char IMPORT_FORM_STR[] PROGMEM = "<form action=\"/upload\" method = \"post\" enctype = \"multipart/form-data\">"
                                       "<div> <label for=\"mp3_file\">Оберіть .mp3 файл для імпорту:</label>"
                                       "<br><input type=\"file\" id=\"mp3_file\" name=\"mp3_file\" accept=\"audio/mp3\">"
                                       "<br></div><div><button>Імпортувати</button></div></form>";

const char STR_DOWNLOAD_PATH[] PROGMEM = "/download";
const char STR_UPLOAD_PATH[] PROGMEM = "/upload";
const char STR_AP_NAME[] PROGMEM = "Колодєєв mp3";

bool FileServer::_can_work = true;
size_t FileServer::_buffer_size = 80 * 1024;
WebServer *FileServer::_server;

FileServer::FileServer()
{
    _server = &_web_server;
}

FileServer::~FileServer()
{
    if (_server != nullptr)
        _server->stop();

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

bool FileServer::start(bool is_master)
{
    const uint8_t PIN_SD_CS{5};

    if (!SD.begin(PIN_SD_CS))
    {
        log_e("SD не примонтовано.");
        return false;
    }

    if (!WiFi.softAP(STR_AP_NAME, NULL, 1, 0, 1))
    {
        log_e("Помилка запуску softAP");
        return false;
    }

    _can_work = true;
    _task_params.is_master = is_master;
    xTaskCreatePinnedToCore(fileServerTask, "fileServerTask", _buffer_size, &_task_params, 2, NULL, 0);

    return true;
}

void FileServer::stop()
{
    _task_params.is_running = false;
}

void FileServer::fileServerTask(void *params)
{
    ServerTaskParams *t_params = (ServerTaskParams *)params;
    t_params->is_running = true;

    // Відправка файлу
    if (t_params->is_master)
        _server->on("/", handleOnExportConnect);
    // Прийом файлу
    else
    {
        _server->on("/", handleOnImportConnect);
        _server->on(
            "/upload", HTTP_POST, []() {},
            handleOnUpload);
    }

    _server->onNotFound(handleOn404Connect);
    _server->begin();

    while (t_params->is_running)
    {
        if (_can_work)
        {
            _server->handleClient();
            vTaskDelay(1);
        }
        else
            break;
    }

    _server->stop();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

// Відобразити форму для завантаження файлу на сервер
void FileServer::handleOnImportConnect()
{
    String html = HEAD_TOP_STR;
    html += IMPORT_TITLE_STR; // Заголовок
    html += HEAD_MID_STR;
    html += IMPORT_FORM_STR; // Форма
    html += FOOT_STR;

    _server->send(200, "text/html", html);
}

// Отримати файл від клієнта
void FileServer::handleOnUpload()
{
    static File input_file;

    HTTPUpload &uploadfile = _server->upload();

    _server->setContentLength(CONTENT_LENGTH_UNKNOWN);

    if (!uploadfile.filename.endsWith(F(".mp3")))
        _server->send(403);
    else if (uploadfile.status == UPLOAD_FILE_START) // Ініціація завантаження файлу на сервер
    {
        // Повне ім'я файлу
        String file_name = STR_DOWNLOAD_PATH;
        file_name += "/" + uploadfile.filename;

        if (!SD.exists(STR_DOWNLOAD_PATH))
            SD.mkdir(STR_DOWNLOAD_PATH);

        if (SD.exists(file_name))
            SD.remove(file_name);

        input_file = SD.open(file_name, FILE_APPEND);

        if (!input_file)
        {
            _can_work = false;
            log_e("Не можу відкрити файл на запис");
        }
    }
    else if (uploadfile.status == UPLOAD_FILE_WRITE) // Отримання файлу
    {
        if (input_file)
            input_file.write(uploadfile.buf, uploadfile.currentSize);

        vTaskDelay(1);
    }
    else if (uploadfile.status == UPLOAD_FILE_END || uploadfile.status == UPLOAD_FILE_ABORTED) // Файл отримано, або клієнт відвалився.
    {
        if (input_file)
            input_file.close();

        String html = HEAD_TOP_STR;
        html += IMPORT_TITLE_STR; // Заголовок
        html += HEAD_MID_STR;
        html += F("<a href=\"http://192.168.4.1/\">На головну</a>");
        html += FOOT_STR;

        _server->send(200, "text/html", html);
    }
}

// Відобразити список файлів на сервері в директорії /upload
void FileServer::handleOnExportConnect()
{
    String html = HEAD_TOP_STR;
    html += EXPORT_TITLE_STR; // Заголовок
    html += HEAD_MID_STR;

    if (!SD.exists(STR_UPLOAD_PATH))
        SD.mkdir(STR_UPLOAD_PATH);

    // Відкрити директорію з файлами на SD
    File root = SD.open(STR_UPLOAD_PATH);

    if (!root || !root.isDirectory())
    {
        _can_work = false;
        html += F("<br>Не можу відкрити директорію на SD");
    }
    else if (_server->args() > 0)
    {
        // Сформувати повне ім'я файлу на SD
        String path = STR_UPLOAD_PATH;
        path += "/" + _server->arg(0);

        File file = SD.open(path);

        // Почати передачу файлу до клієнта
        if (file)
        {
            _server->sendHeader("Content-Type", "text/text");
            _server->sendHeader("Content-Disposition", "attachment; filename=" + _server->arg(0));
            _server->streamFile(file, "application/octet-stream");
            file.close();
        }
        else
        {
            String html = HEAD_TOP_STR;
            html += IMPORT_TITLE_STR; // Заголовок
            html += HEAD_MID_STR;
            html += F("Не можу прочитати файл");
            html += F("<br><a href=\"http://192.168.4.1/\">На головну</a>");
            html += FOOT_STR;

            _server->send(200, "text/html", html);
        }
    }
    // Якщо відсутні параметри, відобразити список файлів в директорії
    else
    {
        File file;
        while ((file = root.openNextFile()))
        {
            if (!file.isDirectory())
            {
                html += F("<br><br> <a href=\"/?file_name=");
                html += file.name();
                html += F("\">");
                html += file.name();
                html += F("</a>");
            }
            vTaskDelay(1);
        }
        file.close();
    }

    html += FOOT_STR;
    _server->send(200, "text/html", html);
}

// 404
void FileServer::handleOn404Connect()
{
    String html = HEAD_TOP_STR;
    html += P_N_F_TITLE_STR; // Заголовок
    html += HEAD_MID_STR;
    html += NOT_FOUND_CONTENT_STR;
    html += FOOT_STR;
    _server->send(404, "text/html", html);
}
