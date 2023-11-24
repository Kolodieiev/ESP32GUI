#pragma GCC optimize("Ofast")

#include "PlaylistManager.h"

PlaylistManager::PlaylistManager()
{
}

PlaylistManager::~PlaylistManager()
{
    SD.end();
}

bool PlaylistManager::connect()
{
    if (!SD.begin(PIN_SD_CS))
    {
        log_e("SD не примонтовано.");
        return false;
    }

    return true;
}

bool PlaylistManager::updatePlaylists()
{
    if (!connect())
        return false;

    uint8_t result{0};

    result = xTaskCreatePinnedToCore(updatePlaylistsTask, "updatePlaylistsTask", 1024 * 70, &_update_playlist_task_params, 1, NULL, 0);

    return result;
}

void PlaylistManager::updatePlaylistsTask(void *task_params)
{
    ParamsCreatePlaylistTask *params = (ParamsCreatePlaylistTask *)task_params;

    params->task_canceled = false;
    params->task_completed = false;
    params->task_success = false;

    File root = SD.open(ROOT_PATH);

    if (!root || !root.isDirectory())
    {
        log_e("Не можу відкрити /Music");
        root.close();
        SD.end();
        vTaskDelete(NULL);
        return;
    }

    String playlists_path = ROOT_PATH;
    playlists_path += PLAYLIST_NAME;

    SD.remove(playlists_path);
    File file_playlists = SD.open(playlists_path, "a", true);

    if (!file_playlists || file_playlists.isDirectory())
    {
        log_e("Не можу відкрити /Music/Playlist");
        file_playlists.close();
        SD.end();
        vTaskDelete(NULL);
        return;
    }

    // Знайти всі папки в основній директорії
    File file;
    while ((file = root.openNextFile()) && !params->task_canceled)
    {
        if (file.isDirectory())
        {
            String f_name = file.name();
            f_name += "|";
            file_playlists.print(f_name);
        }
        file.close();
        vTaskDelay(1);
    }

    file.close();
    root.close();
    file_playlists.close();

    if (params->task_canceled)
    {
        vTaskDelete(NULL);
        return;
    }

    file_playlists = SD.open(playlists_path, "r");

    if (!file_playlists)
    {
        log_e("Не можу відкрити плейлист");
        vTaskDelete(NULL);
        return;
    }

    // Прочитати назву плейлиста
    while (file_playlists.available() && !params->task_canceled)
    {
        String playlist_path = ROOT_PATH;
        playlist_path += "/";
        playlist_path += file_playlists.readStringUntil('|');

        root = SD.open(playlist_path);

        if (root)
        {
            String file_list = playlist_path;
            file_list += PLAYLIST_NAME;

            SD.remove(file_list);

            File playlist = SD.open(file_list, "a");

            if (playlist)
            {
                // Знайти всі mp3 файли в плейлисті
                while ((file = root.openNextFile()) && !params->task_canceled)
                {
                    if (!file.isDirectory())
                    {
                        String file_name = file.name();

                        if (file_name.endsWith(F(".mp3")))
                        {
                            file_name += "|";
                            playlist.print(file_name);
                        }
                    }

                    file.close();
                    vTaskDelay(1);
                }
                file.close();
                playlist.close();
            }
        }

        root.close();
        vTaskDelay(1);
    }

    file_playlists.close();

    params->task_completed = true;
    if (!params->task_canceled)
        params->task_success = true;

    SD.end();
    vTaskDelete(NULL);
}

void PlaylistManager::fillPlaylistMenu(Menu *menu_ptr, const Focusable *template_ptr, uint16_t from_id)
{
    if (!connect())
        return;

    if (!SD.exists(ROOT_PATH))
        SD.mkdir(ROOT_PATH);

    String root_playlist = ROOT_PATH;
    root_playlist += PLAYLIST_NAME;

    File playlists = SD.open(root_playlist, "r");

    if (playlists)
    {
        uint16_t item_ID_counter = from_id;
        while (playlists.available())
        {
            Focusable *item = template_ptr->clone(item_ID_counter);
            item->setText(playlists.readStringUntil('|'));
            menu_ptr->addWidget(item);
            item_ID_counter++;
        }

        playlists.close();
    }
}

bool PlaylistManager::loadMenuFileItems(std::vector<IWidget *> &items, const String &dir_name, uint16_t start_pos, uint16_t size, const Focusable *template_item)
{
    if (!connect())
        return false;

    String file_path = ROOT_PATH;
    file_path += "/" + dir_name;
    file_path += PLAYLIST_NAME;

    File playlist = SD.open(file_path, "r");

    if (!playlist || playlist.isDirectory())
    {
        log_e("Не можу відкрити список файлів");
        playlist.close();
        return false;
    }

    uint16_t file_counter{0};
    String cur_str;
    String temp;

    while (playlist.available())
    {
        temp = playlist.readStringUntil('|');

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

        if (!playlist.available())
            break;

        cur_str = playlist.readStringUntil('|');
    }

    playlist.close();

    return items.size() > 0;
}

bool PlaylistManager::removePlaylist(const char *playlist_name)
{
    if (!connect())
        return false;

    String playlist_file_name = ROOT_PATH;
    playlist_file_name += PLAYLIST_NAME;

    String temp_playlist_file_name = ROOT_PATH;
    temp_playlist_file_name += TEMP_PLAYLIST_NAME;

    SD.remove(temp_playlist_file_name);

    File playlist = SD.open(playlist_file_name, FILE_READ);
    File temp_playlist = SD.open(temp_playlist_file_name, FILE_APPEND);

    if (!playlist || !temp_playlist)
    {
        log_e("Can`t open playlists file");
        playlist.close();
        temp_playlist.close();
        return false;
    }

    while (playlist.available())
    {
        String playlist_str = playlist.readStringUntil('|');

        if (strcmp(playlist_str.c_str(), playlist_name) == 0)
            continue;

        playlist_str += "|";

        temp_playlist.print(playlist_str);
    }

    playlist.close();
    temp_playlist.close();

    SD.remove(playlist_file_name);
    SD.rename(temp_playlist_file_name, playlist_file_name);

    return true;
}

String PlaylistManager::getTrackName(const String &playlist_name, uint16_t track_pos)
{
    if (!connect())
        return "";

    String file_path = ROOT_PATH;
    file_path += "/" + playlist_name;
    file_path += PLAYLIST_NAME;

    File playlist = SD.open(file_path, FILE_READ);

    if (!playlist || playlist.isDirectory())
    {
        log_e("Не можу прочитати плейлист");
        return "";
    }

    uint16_t track_counter{0};
    String temp;
    String ret_str;

    while (playlist.available())
    {
        temp = playlist.readStringUntil('|');

        if (track_counter == track_pos)
        {
            ret_str = temp;
            break;
        }

        track_counter++;
    }

    playlist.close();
    return ret_str;
}

uint16_t PlaylistManager::getPlaylistSize(const String &playlist_name)
{
    if (!connect())
        return 0;

    String file_path = ROOT_PATH;
    file_path += "/" + playlist_name;
    file_path += PLAYLIST_NAME;

    File playlist = SD.open(file_path, FILE_READ);

    if (!playlist || playlist.isDirectory())
    {
        log_e("Не можу прочитати плейлист");
        return 0;
    }

    uint16_t track_counter{0};

    while (playlist.available())
    {
        playlist.readStringUntil('|');
        track_counter++;
    }

    playlist.close();
    return track_counter;
}
