#pragma once
#include <Arduino.h>
#include "PlaylistManager.h"
#include "lib/Audio/Audio.h"

class AudioManager : public PlaylistManager
{
public:
    AudioManager();
    ~AudioManager();

    bool playPlaylist(const String &name, uint16_t track_number, const uint32_t position);
    bool playTrack(const String &path, const uint32_t track_pos);
    bool playNext();
    bool playPrev();

    void volumeUp();
    void volumeDown();

    void fastForward();
    void rewind();

protected:
    Audio _audio;
    uint8_t _volume;
    String _playlist_name;
    String _track_name;
    uint16_t _track_number{0};
    uint16_t _track_position{0};

    void savePref();

private:
    const uint8_t PIN_I2S_LRC{26};
    const uint8_t PIN_I2S_DOUT{25};
    const uint8_t PIN_I2S_BCLK{12};
};
