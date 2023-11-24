#include "FileManager.h"
#include <pgmspace.h>

FileManager::~FileManager()
{
    SD.end();
}

bool FileManager::connect()
{
    if (!SD.begin(PIN_SD_CS))
    {
        log_e("SD не примонтовано.");
        return false;
    }

    return true;
}

bool FileManager::fileExist(const char *name)
{
    if (!connect())
        return false;

    if (!SD.exists(name))
    {
        log_e("Файл не існує.");
        return false;
    }

    File root = SD.open(name);

    if (!root)
    {
        log_e("Помилка відкриття файлу.");
        return false;
    }

    if (root.isDirectory())
    {
        log_e("Шлях не вказує на файл.");
        root.close();
        return false;
    }

    root.close();
    return true;
}

bool FileManager::dirExist(const char *name)
{
    if (!connect())
        return false;

    if (!SD.exists(name))
    {
        log_e("Директорія не існує.");
        return false;
    }

    File root = SD.open(name);

    if (!root)
    {
        log_e("Помилка відкриття директорії.");
        return false;
    }

    if (!root.isDirectory())
    {
        log_e("Шлях не вказує на директорію.");
        root.close();
        return false;
    }

    root.close();
    return true;
}

bool FileManager::createDir(const char *name)
{
    if (!connect())
        return false;

    if (!SD.mkdir(name))
    {
        log_e("Помилка під час створення директорії.");
        return false;
    }
    else
        return true;
}

bool FileManager::remove(const char *name)
{
    if (!connect())
        return false;

    if (!SD.exists(name))
    {
        log_e("Файл не знайдено.");
        return false;
    }

    File file = SD.open(name);

    if (!file)
    {
        log_e("Не отримано дескриптор файлу.");
        return false;
    }

    bool is_dir = file.isDirectory();
    file.close();

    if (!is_dir)
        return SD.remove(name);
    else
    {
        clearDir(name);
        return SD.rmdir(name);
    }
}

void FileManager::clearDir(const char *path)
{
    File root = SD.open(path);

    File temp;
    while (temp = root.openNextFile())
    {
        String path = temp.path();

        if (temp.isDirectory())
        {
            temp.close();
            clearDir(path.c_str());
            SD.rmdir(path.c_str());
        }
        else
        {
            temp.close();
            SD.remove(path);
        }
    }

    root.close();
}

bool FileManager::rename(const char *old_name, const char *new_name)
{
    if (!connect())
        return false;

    if (!SD.exists(old_name))
    {
        log_e("Файл не знайдено.");
        return false;
    }

    return SD.rename(old_name, new_name);
}

String FileManager::getPathFromBreadcrumbs()
{
    String ret_str;

    if (_breadcrumbs.size() > 0)
    {
        for (uint8_t i{0}; i < _breadcrumbs.size(); ++i)
            ret_str += "/" + _breadcrumbs[i];
    }
    else
        ret_str = "/";

    return ret_str;
}

bool FileManager::loadMenuFileItems(std::vector<IWidget *> &items, const String &path, uint16_t start_pos, uint16_t size, const Focusable *template_item)
{
    if (!connect())
        return false;

    String file_name = path;

    if (file_name == "/")
        file_name += FILELIST_NAME;
    else
    {
        file_name += "/";
        file_name += FILELIST_NAME;
    }

    File filelist = SD.open(file_name, "r");

    if (!filelist || filelist.isDirectory())
    {
        log_e("Не можу відкрити список файлів");
        filelist.close();
        return false;
    }

    uint16_t file_counter{0};
    String cur_str;
    String temp;

    while (filelist.available())
    {
        temp = filelist.readStringUntil('|');

        if (file_counter == start_pos)
        {
            cur_str = temp;
            break;
        }

        file_counter++;
    }

    for (uint16_t i{start_pos}; i < size + start_pos; ++i)
    {
        if (cur_str.equals(""))
            break;

        Focusable *item = template_item->clone(i + 1);
        item->setText(cur_str);
        items.push_back(item);

        if (!filelist.available())
            break;

        cur_str = filelist.readStringUntil('|');
    }

    filelist.close();

    return items.size() > 0;
}

bool FileManager::updateDir(String &dir_path)
{
    if (!connect())
        return false;

    _upd_dir_task_params.dir_path = dir_path;
    _upd_dir_task_params.filelist_path = dir_path;

    if (dir_path != "/")
        _upd_dir_task_params.filelist_path += "/";

    _upd_dir_task_params.filelist_path += FILELIST_NAME;

    xTaskCreatePinnedToCore(updateDirTask, "updateDirTask", _upd_dir_task_params.task_heap_size, &_upd_dir_task_params, 1, NULL, 0);

    return true;
}

void FileManager::updateDirTask(void *task_params)
{
    ParamsUpdateDirTask *params = (ParamsUpdateDirTask *)task_params;

    params->task_canceled = false;
    params->task_completed = false;
    params->task_success = false;

    File dir;
    File filelist;

    dir = SD.open(params->dir_path);
    if (dir && dir.isDirectory())
    {
        if (SD.exists(params->filelist_path))
            SD.remove(params->filelist_path);

        filelist = SD.open(params->filelist_path, "a");

        if (filelist && !filelist.isDirectory())
        {
            File file;
            while ((file = dir.openNextFile()) && !params->task_canceled)
            {
                String f_name;

                if (!file.isDirectory())
                    f_name = STR_FILE_PREFIX;
                else
                    f_name = STR_DIR_PREFIX;

                f_name += file.name();
                f_name += "|";
                filelist.print(f_name);
                file.close();
                vTaskDelay(1);
            }
        }
        else
            log_e("Не можу відкрити filelist.");
    }
    else
        log_e("Не можу відкрити директорію.");

    params->task_completed = true;

    if (!params->task_canceled)
        params->task_success = true;

    dir.close();
    filelist.close();

    vTaskDelete(NULL);
}

bool FileManager::copyFile(String &path_from, String &path_to)
{
    if (!connect())
        return false;

    _copy_file_task_params.from = path_from;
    _copy_file_task_params.to = path_to;

    xTaskCreatePinnedToCore(copyFileTask, "copyFileTask", _copy_file_task_params.task_heap_size, &_copy_file_task_params, 1, NULL, 0);

    return true;
}

void FileManager::copyFileTask(void *task_params)
{
    ParamsCopyFileTask *params = (ParamsCopyFileTask *)task_params;

    params->task_canceled = false;
    params->task_completed = false;
    params->task_success = false;
    params->progress = 0;

    if (SD.exists(params->to))
        SD.remove(params->to);

    File old_file = SD.open(params->from, FILE_READ);
    File new_file = SD.open(params->to, FILE_APPEND);

    if (!old_file || !new_file)
    {
        log_e("Не можу відкрити файл для копіювання.");
        params->task_completed = true;
    }
    else
    {
        uint8_t buffer[params->task_heap_size - 1024 * 3];

        size_t file_size = old_file.size();
        size_t writed_bytes_counter{0};

        while (old_file.available() && !params->task_canceled)
        {
            size_t bytesRead = old_file.read(buffer, sizeof(buffer));
            new_file.write(buffer, bytesRead);
            writed_bytes_counter += bytesRead;
            params->progress = (uint8_t)(((float)writed_bytes_counter / file_size) * 100);
            vTaskDelay(1);
        }
    }

    params->task_completed = true;

    if (!params->task_canceled)
        params->task_success = true;

    old_file.close();
    new_file.close();

    vTaskDelete(NULL);
}
