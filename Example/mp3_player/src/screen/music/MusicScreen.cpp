#include "MusicScreen.h"
#include "screen/resources/string.h"
#include "lib/GUI/widget/layout/EmptyLayout.h"
#include "lib/GUI/widget/navbar/NavBar.h"
#include "lib/GUI/util/Util.h"

MusicScreen::MusicScreen(TFT_eSPI &tft): IScreen{tft}
{
    showPlaylists();

    _brightness = atoi(_preference.getPref(STR_BRIGHT_PREF).c_str());

    if (!_brightness)
        _brightness = 100;
}

void MusicScreen::loop()
{
    _audio.loop();
}

void MusicScreen::doGUIWork()
{
    if (_mode == AUDIO_PLAY)
    {
        if (_audio.isRunning())
        {
            if (_is_locked)
                return;

            if (_is_new_track)
            {
                // Поміняти заголовок
                _track_name_txt->setText(_track_name);
                _is_new_track = false;
            }

            if (!_is_duration_calculated)
                if (updateTrackDuration())
                    _is_duration_calculated = true;

            if ((millis() - _last_time_update) > UPD_TIME_INTERVAL)
            {
                // Оновити поточну позицію треку
                updateTrackPos();
                _last_time_update = millis();
            }
        }
        // Якщо трек скінчився самостійно
        else if (_is_playing)
        {
            // Намагаємося перемкнути
            if (playNext())
            {
                _is_new_track = true;
                _is_duration_calculated = false;
            }
            // Якщо не вдалося змінити трек з 3х спроб, зупинити плеєр
            else if (_attempt_to_play_next_counter == 3)
                setStopState();
            else
                _attempt_to_play_next_counter++;
        }
    }
    else if (_mode == PLAYLIST_UPDATE)
    {
        if (isUpdatePlaylistTaskComplpete())
            showPlaylists();
    }
}

bool MusicScreen::updateTrackDuration()
{
    if (!_audio.hasCorrectData())
        return false;

    uint32_t duration = _audio.getAudioFileDuration();

    if (!duration)
        return false;

    uint32_t minutes = floor((float)duration / 60);
    uint32_t sec = duration - minutes * 60;

    String track_time = "/";

    if (minutes < 100)
        track_time += "0";

    if (minutes < 10)
        track_time += "0";

    track_time += String(minutes);

    track_time += ":";

    sec = duration - minutes * 60;

    if (sec < 10)
        track_time += "0";

    track_time += String(sec);
    _track_time_txt->setText(track_time);

    return true;
}

void MusicScreen::updateTrackPos()
{
    uint32_t position = _audio.getAudioCurrentTime();

    uint32_t minutes = floor((float)position / 60);
    uint32_t sec = position - minutes * 60;

    String track_pos;

    if (minutes < 100)
        track_pos += "0";

    if (minutes < 10)
        track_pos += "0";

    track_pos += String(minutes);

    track_pos += ":";

    sec = position - minutes * 60;

    if (sec < 10)
        track_pos += "0";

    track_pos += String(sec);

    _track_pos_txt->setText(track_pos);
}

void MusicScreen::setPlayState()
{
    _is_playing = true;
    _is_new_track = true;
    _is_duration_calculated = false;
    _play_btn->setText(STR_PAUSE_BTN);
}

void MusicScreen::setStopState()
{
    _audio.stopSong();

    _is_playing = false;

    _track_number = 0;
    _track_name = "";

    _track_name_txt->setText(STR_STOPPED);
    _play_btn->setText(STR_PLAY_BTN);
    _track_pos_txt->setText(STR_POS_TIME);
    _track_time_txt->setText(STR_TRACK_TIME);
}

void MusicScreen::showPlaying()
{
    _mode = AUDIO_PLAY;

    _play_btn = new Label(1, _tft);
    _play_btn->setText(STR_PAUSE_BTN);
    _play_btn->setFontID(2);
    _play_btn->setTextSize(2);
    _play_btn->setAlign(IWidget::ALIGN_CENTER);
    _play_btn->setGravity(IWidget::GRAVITY_CENTER);
    _play_btn->setBackColor(COLOR_BLACK);
    _play_btn->setTextColor(COLOR_ORANGE);
    _play_btn->setCornerRadius(15);
    _play_btn->setWidth(35);
    _play_btn->setHeight(35);
    _play_btn->setPos((_tft.width() - 35) / 2, (_tft.height() - 35) / 2);

    Label *prev_btn = _play_btn->clone(2);
    prev_btn->setTextSize(1);
    prev_btn->setText(STR_PREV_BTN);
    prev_btn->setWidth(30);
    prev_btn->setHeight(30);
    prev_btn->setPos(10, (_tft.height() - 24) / 2);

    Label *next_btn = prev_btn->clone(3);
    next_btn->setText(STR_NEXT_BTN);
    next_btn->setPos(_tft.width() - 40, (_tft.height() - 24) / 2);

    _track_name_txt = new Label(4, _tft);
    _track_name_txt->setText(_track_name);
    _track_name_txt->setFontID(4);
    _track_name_txt->setAlign(IWidget::ALIGN_CENTER);
    _track_name_txt->setGravity(IWidget::GRAVITY_CENTER);
    _track_name_txt->setBackColor(COLOR_BLACK);
    _track_name_txt->setTextColor(COLOR_ORANGE);
    _track_name_txt->setWidth(_tft.width());
    _track_name_txt->setHeight(15);
    _track_name_txt->setIsTicker(true);

    _track_pos_txt = _track_name_txt->clone(5);
    _track_pos_txt->setFontID(2);
    _track_pos_txt->setText(STR_POS_TIME);
    _track_pos_txt->setAlign(IWidget::ALIGN_END);
    _track_pos_txt->setWidthToFit();
    _track_pos_txt->setBackColor(COLOR_MAIN_BACK);
    _track_pos_txt->setPos(0, _tft.height() - 15);
    _track_pos_txt->setIsTicker(false);

    _track_time_txt = _track_pos_txt->clone(6);
    _track_time_txt->setText(STR_TRACK_TIME);
    _track_time_txt->setAlign(IWidget::ALIGN_START);
    _track_time_txt->setWidthToFit();
    _track_time_txt->setPos(_track_pos_txt->getWidth(), _tft.height() - 15);
    _track_time_txt->setIsTicker(false);

    _volume_txt = new Label(7, _tft);
    _volume_txt->setText(String(_volume));
    _volume_txt->setAlign(IWidget::ALIGN_END);
    _volume_txt->setGravity(IWidget::GRAVITY_CENTER);
    _volume_txt->setHeight(15);
    _volume_txt->setWidth(20);
    _volume_txt->setPos(_tft.width() - 20, _tft.height() - 15);
    _volume_txt->setBackColor(COLOR_MAIN_BACK);
    _volume_txt->setTextColor(COLOR_ORANGE);

    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    layout->addWidget(_play_btn);
    layout->addWidget(prev_btn);
    layout->addWidget(next_btn);
    layout->addWidget(_track_name_txt);
    layout->addWidget(_track_pos_txt);
    layout->addWidget(_track_time_txt);
    layout->addWidget(_volume_txt);

    setPlayState();
}

void MusicScreen::showUpdating()
{
    _mode = PLAYLIST_UPDATE;

    NavBar *navbar = _creator.getNavbar(4, "", "", STR_CANCEL);

    Label *message = new Label(1, _tft);
    message->setAlign(IWidget::ALIGN_CENTER);
    message->setGravity(IWidget::GRAVITY_CENTER);
    message->setBackColor(COLOR_MAIN_BACK);
    message->setTextColor(COLOR_WHITE);
    message->setText(STR_UPDATING);
    message->setHeight(16);
    message->setWidthToFit();
    message->setPos((float)(_tft.width() - message->getWidth()) / 2, (float)(_tft.height() - NAVBAR_HEIGHT - message->getHeight()) / 2);

    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    layout->addWidget(message);
    layout->addWidget(navbar);
}

void MusicScreen::showPlaylists()
{
    _mode = PLAYLIST_SELECT;

    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    NavBar *navbar = _creator.getNavbar(4, STR_SELECT, "", STR_BACK);
    layout->addWidget(navbar);

    _fixed_menu = new FixedMenu(3, _tft);
    _fixed_menu->setBackColor(COLOR_MENU_ITEM);
    _fixed_menu->setWidth(_tft.width() - SCROLLBAR_WIDTH);
    _fixed_menu->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _fixed_menu->setItemsHeight((_tft.height() - NAVBAR_HEIGHT) / 3);
    layout->addWidget(_fixed_menu);

    _scrollbar = new ScrollBar(2, _tft);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _scrollbar->setPos(_tft.width() - SCROLLBAR_WIDTH, 0);
    layout->addWidget(_scrollbar);

    Focusable *update_item = _creator.getFocusableItem(1, STR_UPDATE_PLAYLISTS);

    if (!_playlist_name.equals(""))
    {
        Focusable *continue_item = update_item->clone(2);
        continue_item->setText(STR_CONTINUE);
        _fixed_menu->addWidget(continue_item);
    }

    fillPlaylistMenu(_fixed_menu, update_item, 3);

    _fixed_menu->addWidget(update_item);
    _scrollbar->setMax(_fixed_menu->getSize());
}

void MusicScreen::showTracks(const String &playlist_name, uint16_t pos)
{
    _playlist_name = playlist_name;
    _mode = TRACK_SELECT;

    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);
    layout->addWidget(_creator.getNavbar(1, STR_SELECT, "", STR_BACK));

    _dynamic_menu = _creator.getDynamicMenu(2, this);
    _dynamic_menu->setWidth(_tft.width() - SCROLLBAR_WIDTH);
    _dynamic_menu->setIsTickerInFocus(true);

    _scrollbar = new ScrollBar(3, _tft);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_tft.height() - NAVBAR_HEIGHT);
    _scrollbar->setPos(_tft.width() - SCROLLBAR_WIDTH, 0);
    _scrollbar->setMax(getPlaylistSize(playlist_name));
    _scrollbar->setValue(pos);

    layout->addWidget(_dynamic_menu);
    layout->addWidget(_scrollbar);

    Focusable *template_item = _creator.getFocusableItem(1, "");
    std::vector<IWidget *> widgets;
    if (loadMenuFileItems(widgets, _playlist_name, pos, _dynamic_menu->getItemsNumberOnScreen(), template_item))
        for (int i{0}; i < widgets.size(); ++i)
            _dynamic_menu->addWidget(widgets[i]);
}

void MusicScreen::okClickedHandler()
{
    if (_mode == PLAYLIST_SELECT)
    {
        uint16_t item_ID = _fixed_menu->getCurrentItemID();

        if (item_ID == 1)
        {
            if (updatePlaylists())
                showUpdating();
        }
        else if (item_ID == 2)
        {
            if (!_playlist_name.equals(""))
                if (playPlaylist(_playlist_name, _track_number, _track_position))
                    showPlaying();
        }
        else
        {
            Focusable *item = _fixed_menu->findItemByID(item_ID);
            String playlist_name = item->getText();
            showTracks(playlist_name);
        }
    }
    else if (_mode == TRACK_SELECT)
    {
        uint16_t item_ID = _dynamic_menu->getCurrentItemID();
        if (playPlaylist(_playlist_name, item_ID - 1, 0))
            showPlaying();
    }
    else if (_mode == AUDIO_PLAY)
    {
        if (_track_name != "")
        {
            _audio.pauseResume();

            if (_audio.isRunning())
            {
                _play_btn->setText(STR_PAUSE_BTN);
                _is_playing = true;
            }
            else
            {
                _play_btn->setText(STR_PLAY_BTN);
                _is_playing = false;
            }
        }
    }
}

void MusicScreen::upClickedHandler()
{
    if (_mode == PLAYLIST_SELECT)
    {
        _fixed_menu->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_mode == TRACK_SELECT)
    {
        _dynamic_menu->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_mode == AUDIO_PLAY)
    {
        volumeUp();
        _volume_txt->setText(String(_volume));
    }
}

void MusicScreen::downClickedHandler()
{
    if (_mode == PLAYLIST_SELECT)
    {
        _fixed_menu->focusDown();
        _scrollbar->scrollDown();
    }
    else if (_mode == TRACK_SELECT)
    {
        _dynamic_menu->focusDown();
        _scrollbar->scrollDown();
    }
    else if (_mode == AUDIO_PLAY)
    {
        volumeDown();
        _volume_txt->setText(String(_volume));
    }
}

void MusicScreen::leftClickedHandler()
{
    if (_mode == AUDIO_PLAY && !_is_locked)
    {
        if (playPrev())
            setPlayState();
        else
            setStopState();
    }
    else if (_mode == PLAYLIST_SELECT)
    {
        okClickedHandler();
    }
}

void MusicScreen::rightClickedHandler()
{
    if (_mode == PLAYLIST_SELECT)
        openScreenByID(ID_SCREEN_MENU);
    else if (_mode == TRACK_SELECT)
        showPlaylists();
    else if (_mode == AUDIO_PLAY && !_is_locked)
    {
        if (playNext())
            setPlayState();
        else
            setStopState();
    }
    else if (_mode == PLAYLIST_UPDATE)
        cancelUpdatePlaylistTask();
}

void MusicScreen::okPressedHandler()
{
    if (_mode == AUDIO_PLAY && !_is_locked)
    {
        _track_position = _audio.getAudioCurrentTime();
        _audio.stopSong();
        savePref();
        //
        showTracks(_playlist_name, _track_number);
    }
}

void MusicScreen::upPressedHandler()
{
    if (_mode == AUDIO_PLAY && _is_locked)
    {
        Util::setBrightness(_brightness);
        _is_locked = false;
        _track_name_txt->setIsTicker(true);
    }
}

void MusicScreen::downPressedHandler()
{
    if (_mode == AUDIO_PLAY && !_is_locked)
    {
        Util::setBrightness(0);
        _is_locked = true;
        _track_name_txt->setIsTicker(false);
    }
}

bool MusicScreen::loadNext(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID)
{
    if (!current_ID)
        return false;

    if (!connect())
        return false;

    Focusable *template_item = _creator.getFocusableItem(1, "");

    return loadMenuFileItems(widgets, _playlist_name, current_ID, size, template_item);
}

bool MusicScreen::loadPrev(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID)
{
    // Не отримали ID
    if (!current_ID)
        return false;

    uint16_t item_pos = current_ID - 1;

    // Вже перший елемент
    if (!item_pos)
        return false;

    if (!connect())
        return false;

    // Вирахувати першу позицію звідки потрібно читати файл.
    if (current_ID > size)
        item_pos = current_ID - size - 1;
    else
    {
        item_pos = 0;
        // Вирівняти скролбар, якщо меню було завантажене не з першого елемента
        _scrollbar->setValue(current_ID);
    }

    Focusable *template_item = _creator.getFocusableItem(1, "");

    return loadMenuFileItems(widgets, _playlist_name, item_pos, size, template_item);
}

void MusicScreen::leftPressedHandler()
{
    rewind();
}

void MusicScreen::rightPressedHandler()
{
    fastForward();
}