#pragma once
#include <Arduino.h>
#include "screen/WidgetCreator.h"
#include "AudioManager.h"
#include "lib/GUI/IScreen.h"
#include "lib/GUI/widget/scrollbar/ScrollBar.h"
#include "lib/GUI/widget/menu/FixedMenu.h"
#include "lib/GUI/widget/menu/DynamicMenu.h"
#include "lib/GUI/widget/menu/IItemsLoader.h"

#include "lib/GUI/widget/text/Label.h"

class MusicScreen : AudioManager, public IItemsLoader, public IScreen
{

public:
    MusicScreen(TFT_eSPI &tft);
    virtual ~MusicScreen(){}

    virtual bool loadNext(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID);
    virtual bool loadPrev(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID);

protected:
    // screen
    virtual void loop() override;
    virtual void doGUIWork() override;
    //
    virtual void okClickedHandler() override;
    virtual void upClickedHandler() override;
    virtual void downClickedHandler() override;
    virtual void leftClickedHandler() override;
    virtual void rightClickedHandler() override;
    virtual void okPressedHandler() override;
    virtual void upPressedHandler() override;
    virtual void downPressedHandler() override;
    virtual void leftPressedHandler() override;
    virtual void rightPressedHandler() override;

private:
    enum Mode
    {
        PLAYLIST_SELECT,
        PLAYLIST_UPDATE,
        TRACK_SELECT,
        AUDIO_PLAY
    };

    WidgetCreator _creator{_tft};

    uint8_t _brightness;

    bool _is_locked{false};

    Mode _mode{PLAYLIST_SELECT};

    ScrollBar *_scrollbar;
    FixedMenu *_fixed_menu;
    DynamicMenu *_dynamic_menu;

    // show playing
    const uint16_t UPD_TIME_INTERVAL{1000};
    unsigned long _last_time_update{0};

    Label *_track_name_txt;
    Label *_play_btn;
    Label *_track_pos_txt;
    Label *_track_time_txt;
    Label *_volume_txt;
    //
    uint8_t _attempt_to_play_next_counter{0};
    bool _is_new_track{false};
    bool _is_playing{false};
    uint32_t _current_duration{0};
    bool _is_duration_calculated{false};
    //

    void showPlaying();
    void showUpdating();
    void showPlaylists();
    void showTracks(const String &playlist_name, uint16_t pos = 0);

    void setPlayState();
    void setStopState();

    bool updateTrackDuration();
    void updateTrackPos();
};
