#pragma once
#include <Arduino.h>
#include <pgmspace.h>
#include <SD.h>
#include "lib/pref/PrefManager.h"
#include "lib/GUI/widget/menu/Menu.h"

const char ROOT_PATH[] PROGMEM = "/music";
const char PLAYLIST_NAME[] PROGMEM = "/playlist.lib";
const char TEMP_PLAYLIST_NAME[] PROGMEM = "/playlist_temp";

class PlaylistManager
{

public:
    PlaylistManager();
    ~PlaylistManager();

    bool updatePlaylists();
    void cancelUpdatePlaylistTask() { _update_playlist_task_params.task_canceled = true; }
    bool isUpdatePlaylistTaskComplpete() { return _update_playlist_task_params.task_completed; }
    bool isUpdatePlaylistTaskSuccess() { return _update_playlist_task_params.task_success; }

    void fillPlaylistMenu(Menu *menu_ptr, const Focusable *template_item, uint16_t from_id);

    bool loadMenuFileItems(std::vector<IWidget *> &items, const String &dir_name, uint16_t start_pos, uint16_t size, const Focusable *template_item);


    String getTrackName(const String &playlist_name, uint16_t track_pos);
    uint16_t getPlaylistSize(const String &playlist_name);

    bool removePlaylist(const char *playlist_name);
    bool connect();

protected:
    const uint8_t PIN_SD_CS{5};

    PrefManager _preference;

private:
    struct ParamsCreatePlaylistTask
    {
        bool task_canceled{false};
        bool task_completed{false};
        bool task_success{false};
    };

    String _current_playlist_name;
    String _current_track_name;

    ParamsCreatePlaylistTask _update_playlist_task_params;

    static void updatePlaylistsTask(void *params);
};
