#pragma once
#include <Arduino.h>
#include <SD.h>
#include "lib/GUI/widget/menu/Menu.h"

const char FILELIST_NAME[] PROGMEM = "files.lib";
const char STR_DIR_PREFIX[] PROGMEM = "[D] ";
const char STR_FILE_PREFIX[] PROGMEM = "[F] ";

class FileManager
{

public:
    FileManager() {}
    ~FileManager();

    bool connect();

    bool fileExist(const char *name);
    bool dirExist(const char *name);

    bool createDir(const char *name);
    bool remove(const char *name);
    bool rename(const char *old_name, const char *new_name);

    bool updateDir(String &dir_path);
    void cancelDirUpdating()
    {
        _upd_dir_task_params.task_canceled = true;
    }
    bool isUpdateDirTaskComplete() { return _upd_dir_task_params.task_completed; }
    bool isUpdateDirTaskSuccess() { return _upd_dir_task_params.task_success; }

    bool copyFile(String &path_from, String &path_to);
    void cancelFileCopying()
    {
        _copy_file_task_params.task_canceled = true;
    }
    bool isCopyFileTaskComplete() { return _copy_file_task_params.task_completed; }
    bool isCopyFileTaskSuccess() { return _copy_file_task_params.task_success; }
    uint8_t copyFileProgress() { return _copy_file_task_params.progress; }

protected:
    std::vector<String> _breadcrumbs;

    String getPathFromBreadcrumbs();
    bool loadMenuFileItems(std::vector<IWidget *> &items, const String &path, uint16_t start_pos, uint16_t size, const Focusable *template_item);

private:
    const uint8_t PIN_SD_CS{5};

    struct ParamsCopyFileTask
    {
        const uint32_t task_heap_size{55 * 1024};

        volatile bool task_canceled{false};
        bool task_completed{false};
        bool task_success{false};
        uint8_t progress{0};
        //
        String from;
        String to;
    };

    struct ParamsUpdateDirTask
    {
        const uint32_t task_heap_size{30 * 1024};

        volatile bool task_canceled{false};
        bool task_completed{false};
        bool task_success{false};

        String dir_path;
        String filelist_path;
    };

    ParamsCopyFileTask _copy_file_task_params;
    ParamsUpdateDirTask _upd_dir_task_params;

    static void updateDirTask(void *params);
    static void copyFileTask(void *params);

    void clearDir(const char *path);
};
