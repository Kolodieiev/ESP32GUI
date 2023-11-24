#include "AudioManager.h"
#include "PlaylistManager.h"
#include <pgmspace.h>

const char STR_VOLUME_PREF[] PROGMEM = "Volume";
const char STR_PLAYLIST_PREF[] PROGMEM = "Playlist";
const char STR_TRACK_NUM_PREF[] PROGMEM = "Track";
const char STR_TRACK_POS_NUM_PREF[] PROGMEM = "Position";

AudioManager::AudioManager()
{
    pinMode(PIN_SD_CS, OUTPUT);

    _volume = atoi(_preference.getPref(STR_VOLUME_PREF).c_str());
    _track_number = atoi(_preference.getPref(STR_TRACK_NUM_PREF).c_str());
    _track_position = atoi(_preference.getPref(STR_TRACK_POS_NUM_PREF).c_str());
    _playlist_name = _preference.getPref(STR_PLAYLIST_PREF);

    if (!_volume)
        _volume = 5;

    _audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DOUT);
    _audio.setVolume(_volume);
}

AudioManager::~AudioManager()
{
}

bool AudioManager::playPlaylist(const String &name, uint16_t track_number, const uint32_t position)
{
    _audio.stopSong();

    _track_name = getTrackName(name, track_number);

    if (_track_name.equals(""))
    {
        log_v("Файл не знайдено");
        return false;
    }

    String track_path = ROOT_PATH;
    track_path += "/" + name + "/" + _track_name;

    if (!playTrack(track_path, position))
        return false;

    _playlist_name = name;
    _track_number = track_number;

    return true;
}

bool AudioManager::playTrack(const String &path, const uint32_t track_pos)
{
    if (SD.begin(PIN_SD_CS))
        return _audio.connecttoFS(SD, path.c_str(), track_pos);

    return false;
}

bool AudioManager::playNext()
{
    if (playPlaylist(_playlist_name, _track_number + 1, 0))
        return true;

    return false;
}

bool AudioManager::playPrev()
{
    if (_track_number)
        if (playPlaylist(_playlist_name, _track_number - 1, 0))
            return true;

    return false;
}

void AudioManager::volumeUp()
{
    if (_volume < _audio.maxVolume() - 2)
    {
        if (_volume < 10)
            _volume += 1;
        else
            _volume += 2;

        _audio.setVolume(_volume);
    }
}

void AudioManager::volumeDown()
{
    if (_volume > 1)
    {
        if (_volume > 10)
            _volume -= 2;
        else
            _volume -= 1;

        _audio.setVolume(_volume);
    }
}

void AudioManager::fastForward()
{
    uint32_t pos = _audio.getAudioCurrentTime();
    pos += 16;
    _audio.setAudioPlayPosition(pos);
}

void AudioManager::rewind()
{
    uint32_t pos = _audio.getAudioCurrentTime();
    if (pos > 16)
    {
        pos -= 16;
        _audio.setAudioPlayPosition(pos);
    }
}

void AudioManager::savePref()
{
    _preference.setPref(STR_VOLUME_PREF, String(_volume).c_str());
    _preference.setPref(STR_PLAYLIST_PREF, String(_playlist_name).c_str());
    _preference.setPref(STR_TRACK_NUM_PREF, String(_track_number).c_str());
    _preference.setPref(STR_TRACK_POS_NUM_PREF, String(_track_position).c_str());
}
