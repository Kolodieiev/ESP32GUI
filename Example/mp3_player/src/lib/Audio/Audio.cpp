// #define IS_DEBUG

/*
 * Audio.cpp
 *
 *  Created on: Oct 26.2018
 *  Version 3.0.1i
 *  Updated on: Mar 24.2023
 *      Author: Wolle (schreibfaul1)
 *
 */
#pragma GCC optimize("Ofast")
#include "Audio.h"
#include "mp3_decoder/mp3_decoder.h"

Audio::Audio(bool internalDAC /* = false */, uint8_t channelEnabled /* = I2S_DAC_CHANNEL_BOTH_EN */, uint8_t i2sPort)
{
    mutex_audio = xSemaphoreCreateMutex();

    //    build-in-DAC works only with ESP32 (ESP32-S3 has no build-in-DAC)
    //    build-in-DAC last working Arduino Version: 2.0.0-RC2
    //    possible values for channelEnabled are:
    //    I2S_DAC_CHANNEL_DISABLE  = 0,     Disable I2S built-in DAC signals
    //    I2S_DAC_CHANNEL_RIGHT_EN = 1,     Enable I2S built-in DAC right channel, maps to DAC channel 1 on GPIO25
    //    I2S_DAC_CHANNEL_LEFT_EN  = 2,     Enable I2S built-in DAC left  channel, maps to DAC channel 2 on GPIO26
    //    I2S_DAC_CHANNEL_BOTH_EN  = 0x3,   Enable both of the I2S built-in DAC channels.
    //    I2S_DAC_CHANNEL_MAX      = 0x4,   I2S built-in DAC mode max index

    m_f_channelEnabled = channelEnabled;
    m_f_internalDAC = internalDAC;
    // i2s configuration
    m_i2s_num = i2sPort; // i2s port number
    m_i2s_config.sample_rate = 16000;
    m_i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    m_i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    m_i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1; // interrupt priority
    m_i2s_config.dma_buf_count = 16;
    m_i2s_config.dma_buf_len = 512;
    m_i2s_config.use_apll = 0;              // must be disabled in V2.0.1-RC1
    m_i2s_config.tx_desc_auto_clear = true; // new in V1.0.1
    m_i2s_config.fixed_mclk = I2S_PIN_NO_CHANGE;

    if (internalDAC)
    {

#ifdef CONFIG_IDF_TARGET_ESP32 // ESP32S3 has no DAC

        m_i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);

#if ESP_ARDUINO_VERSION_MAJOR >= 2
#if ESP_ARDUINO_VERSION_PATCH == 0
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S); // vers == 2.0.0
#else
        // V2.0.1 ... V2.0.4 will not work
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_MSB); // vers >= 2.0.5
#endif
#else
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S_MSB);
#endif

        i2s_driver_install((i2s_port_t)m_i2s_num, &m_i2s_config, 0, NULL);
        i2s_set_dac_mode((i2s_dac_mode_t)m_f_channelEnabled);
        if (m_f_channelEnabled != I2S_DAC_CHANNEL_BOTH_EN)
        {
            m_f_forceMono = true;
        }

#endif
    }
    else
    {
        m_i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);

#if ESP_ARDUINO_VERSION_MAJOR >= 2
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S); // Arduino vers. > 2.0.0
#else
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB);
#endif

        i2s_driver_install((i2s_port_t)m_i2s_num, &m_i2s_config, 0, NULL);
        m_f_forceMono = false;
    }

    i2s_zero_dma_buffer((i2s_port_t)m_i2s_num);

    for (int i = 0; i < 3; ++i)
    {
        m_filter[i].a0 = 1;
        m_filter[i].a1 = 0;
        m_filter[i].a2 = 0;
        m_filter[i].b1 = 0;
        m_filter[i].b2 = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
esp_err_t Audio::I2Sstart(uint8_t i2s_num)
{
    // It is not necessary to call this function after i2s_driver_install() (it is started automatically),
    // however it is necessary to call it after i2s_stop()
    return i2s_start((i2s_port_t)i2s_num);
}

esp_err_t Audio::I2Sstop(uint8_t i2s_num)
{
    return i2s_stop((i2s_port_t)i2s_num);
}
//---------------------------------------------------------------------------------------------------------------------
Audio::~Audio()
{
    stopSong();
    i2s_driver_uninstall((i2s_port_t)m_i2s_num); // #215 free I2S buffer
    vSemaphoreDelete(mutex_audio);
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::setDefaults()
{
    stopSong();

    if (!_audio_buff.isInitialized())
        _audio_buff.init(); // initialize InputBuffer if not already done

    _audio_buff.resetInputBuffer();
    MP3Decoder_FreeBuffers();

    _loop_counter = 0;
    m_f_playing = false;
    m_f_firstCall = true; // InitSequence for processLokalFile
    m_f_running = false;
    m_f_loop = false;   // Set if audio file should loop
    m_f_unsync = false; // set within ID3 tag but not used
    m_f_exthdr = false; // ID3 extended header

    m_audioCurrentTime = 0; // Reset playtimer
    m_audioFileDuration = 0;
    m_audioDataStart = 0;
    m_audioDataSize = 0;
    m_avr_bitrate = 0;     // the same as m_bitrate if CBR, median if VBR
    m_bitRate = 0;         // Bitrate still unknown
    m_bytesNotDecoded = 0; // counts all not decodable bytes
    _cur_sample = 0;
    m_controlCounter = 0; // Status within readID3data() and readWaveHeader()
    m_channels = 2;       // assume stereo #209
    m_file_size = 0;
    m_ID3Size = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void Audio::setFileLoop(bool state)
{
    m_f_loop = state;
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::UTF8toASCII(char *str)
{
    const uint8_t ascii[60] = {
        // 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148  // UTF8(C3)
        //                Ä    Å    Æ    Ç         É                                       Ñ                  // CHAR
        000, 000, 000, 142, 143, 146, 128, 000, 144, 000, 000, 000, 000, 000, 000, 000, 165, 000, 000, 000, // ASCII
                                                                                                            // 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168
        //      Ö                             Ü              ß    à                   ä    å    æ         è
        000, 153, 000, 000, 000, 000, 000, 154, 000, 000, 225, 133, 000, 000, 000, 132, 134, 145, 000, 138,
        // 169, 170, 171, 172. 173. 174. 175, 176, 177, 179, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188
        //      ê    ë    ì         î    ï         ñ    ò         ô         ö              ù         û    ü
        000, 136, 137, 141, 000, 140, 139, 000, 164, 149, 000, 147, 000, 148, 000, 000, 151, 000, 150, 129};

    uint16_t i = 0, j = 0, s = 0;
    bool f_C3_seen = false;

    while (str[i] != 0)
    { // convert UTF8 to ASCII
        if (str[i] == 195)
        { // C3
            ++i;
            f_C3_seen = true;
            continue;
        }
        str[j] = str[i];
        if (str[j] > 128 && str[j] < 189 && f_C3_seen == true)
        {
            s = ascii[str[j] - 129];
            if (s != 0)
                str[j] = s; // found a related ASCII sign
            f_C3_seen = false;
        }
        ++i;
        ++j;
    }
    str[j] = 0;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::connecttoSD(const char *path, uint32_t resumeFilePos)
{
    return connecttoFS(SD, path, resumeFilePos);
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::connecttoFS(fs::FS &fs, const char *path, uint32_t resumeFilePos)
{
    if (strlen(path) > 255)
        return false;

    xSemaphoreTakeRecursive(mutex_audio, portMAX_DELAY); // #3

    m_resumeFilePos = resumeFilePos;
    char audioName[256];
    setDefaults(); // free buffers an set defaults
    memcpy(audioName, path, strlen(path) + 1);
    if (audioName[0] != '/')
    {
        for (int i = 255; i > 0; --i)
        {
            audioName[i] = audioName[i - 1];
        }
        audioName[0] = '/';
    }
    vTaskDelay(2);

    if (fs.exists(audioName))
    {
        audiofile = fs.open(audioName); // #86
    }
    else
    {
        UTF8toASCII(audioName);
        if (fs.exists(audioName))
        {
            audiofile = fs.open(audioName);
        }
    }

    if (!audiofile)
    {
        log_e("Failed to open file for reading");

        xSemaphoreGiveRecursive(mutex_audio);
        return false;
    }

    m_file_size = audiofile.size(); // TEST loop

    char *afn = NULL; // audioFileName

#ifdef SDFATFS_USED
    audiofile.getName(m_chbuf, m_chbufSize); // #426
    afn = strdup(m_chbuf);
#else
    afn = strdup(audiofile.name());
#endif

    uint8_t dotPos = _util.lastIndexOf(afn, ".");
    for (uint8_t i = dotPos + 1; i < strlen(afn); ++i)
    {
        afn[i] = toLowerCase(afn[i]);
    }

#ifdef IS_DEBUG
    if (!_util.endsWith(afn, ".mp3"))
    {
        log_e("The file format is not supported");
    }
#endif

    if (afn)
    {
        free(afn);
        afn = NULL;
    }

    bool ret = initializeDecoder();
    if (ret)
        m_f_running = true;
    else
        audiofile.close();
    xSemaphoreGiveRecursive(mutex_audio);
    return ret;
}

//---------------------------------------------------------------------------------------------------------------------
size_t Audio::readAudioHeader(uint32_t bytes)
{
    size_t bytesReaded = 0;

    int res = read_ID3_Header(_audio_buff.getReadPtr(), bytes);

    if (res < 0)
        // error, skip header
        m_controlCounter = 100;
    else
        bytesReaded = res;

    if (!isRunning())
    {
        log_e("Processing stopped due to invalid audio header");
        return 0;
    }
    return bytesReaded;
}

//---------------------------------------------------------------------------------------------------------------------
int Audio::read_ID3_Header(const uint8_t *data, size_t len)
{
    static size_t id3Size;
    static size_t remainingHeaderBytes;
    static size_t universal_tmp = 0;
    static uint8_t ID3version;
    static int ehsz = 0;
    static char tag[5];
    static char frameid[5];
    static size_t framesize = 0;
    static bool compressed = false;
    static bool APIC_seen = false;
    static size_t APIC_size = 0;
    static uint32_t APIC_pos = 0;
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 0)
    { /* read ID3 tag and ID3 header size */

        ID3version = 0;
        m_contentlength = getFileSize();

        m_controlCounter++;
        APIC_seen = false;
        remainingHeaderBytes = 0;
        ehsz = 0;
        if (_util.specialIndexOf(data, "ID3", 4) != 0)
        { // ID3 not found
            m_audioDataSize = m_contentlength;
            return -1; // error, no ID3 signature found
        }
        ID3version = *(data + 3);
        switch (ID3version)
        {
        case 2:
            m_f_unsync = (*(data + 5) & 0x80);
            m_f_exthdr = false;
            break;
        case 3:
        case 4:
            m_f_unsync = (*(data + 5) & 0x80); // bit7
            m_f_exthdr = (*(data + 5) & 0x40); // bit6 extended header
            break;
        };
        id3Size = _util.bigEndian(data + 6, 4, 7); //  ID3v2 size  4 * %0xxxxxxx (shift left seven times!!)
        id3Size += 10;

        if (ID3version == 2)
        {
            m_controlCounter = 10;
        }
        remainingHeaderBytes = id3Size;
        m_ID3Size = id3Size;
        remainingHeaderBytes -= 10;

        return 10;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 1)
    { // compute extended header size if exists
        m_controlCounter++;
        if (m_f_exthdr)
        {
            ehsz = _util.bigEndian(data, 4);
            remainingHeaderBytes -= 4;
            ehsz -= 4;
            return 4;
        }
        else
            return 0;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 2)
    { // skip extended header if exists
        if (ehsz > 256)
        {
            ehsz -= 256;
            remainingHeaderBytes -= 256;
            return 256;
        } // Throw it away
        else
        {
            m_controlCounter++;
            remainingHeaderBytes -= ehsz;
            return ehsz;
        } // Throw it away
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 3)
    { // read a ID3 frame, get the tag
        if (remainingHeaderBytes == 0)
        {
            m_controlCounter = 99;
            return 0;
        }
        m_controlCounter++;
        frameid[0] = *(data + 0);
        frameid[1] = *(data + 1);
        frameid[2] = *(data + 2);
        frameid[3] = *(data + 3);
        frameid[4] = 0;
        for (uint8_t i = 0; i < 4; ++i)
            tag[i] = frameid[i]; // tag = frameid

        remainingHeaderBytes -= 4;
        if (frameid[0] == 0 && frameid[1] == 0 && frameid[2] == 0 && frameid[3] == 0)
        {
            // We're in padding
            m_controlCounter = 98; // all ID3 metadata processed
        }
        return 4;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 4)
    { // get the frame size
        m_controlCounter = 6;

        if (ID3version == 4)
        {
            framesize = _util.bigEndian(data, 4, 7); // << 7
        }
        else
        {
            framesize = _util.bigEndian(data, 4); // << 8
        }
        remainingHeaderBytes -= 4;
        uint8_t flag = *(data + 4); // skip 1st flag
        (void)flag;
        remainingHeaderBytes--;
        compressed = (*(data + 5)) & 0x80; // Frame is compressed using [#ZLIB zlib] with 4 bytes for 'decompressed
                                           // size' appended to the frame header.
        remainingHeaderBytes--;
        uint32_t decompsize = 0;
        if (compressed)
        {
            decompsize = _util.bigEndian(data + 6, 4);
            remainingHeaderBytes -= 4;
            (void)decompsize;
            return 6 + 4;
        }
        return 6;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 5)
    { // If the frame is larger than 512 bytes, skip the rest
        if (framesize > 512)
        {
            framesize -= 512;
            remainingHeaderBytes -= 512;
            return 512;
        }
        else
        {
            m_controlCounter = 3; // check next frame
            remainingHeaderBytes -= framesize;
            return framesize;
        }
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 6)
    {                         // Read the value
        m_controlCounter = 5; // only read 256 bytes
        char ch = *(data + 0);

        if (_util.startsWith(tag, "APIC"))
        { // a image embedded in file, passing it to external function

            APIC_seen = true;
            APIC_pos = id3Size - remainingHeaderBytes;
            APIC_size = framesize;

            return 0;
        }

        size_t fs = framesize;
        if (fs > 512)
            fs = 512;

        framesize -= fs;
        remainingHeaderBytes -= fs;

        return fs;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // --- section V2.2 only , greater Vers above ----
    // see https://mutagen-specs.readthedocs.io/en/latest/id3/id3v2.2.html
    if (m_controlCounter == 10)
    { // frames in V2.2, 3bytes identifier, 3bytes size descriptor

        if (universal_tmp > 0)
        {
            if (universal_tmp > 256)
            {
                universal_tmp -= 256;
                return 256;
            }
            else
            {
                uint8_t t = universal_tmp;
                universal_tmp = 0;
                return t;
            }
        }

        frameid[0] = *(data + 0);
        frameid[1] = *(data + 1);
        frameid[2] = *(data + 2);
        frameid[3] = 0;
        for (uint8_t i = 0; i < 4; ++i)
            tag[i] = frameid[i]; // tag = frameid
        remainingHeaderBytes -= 3;
        size_t len = _util.bigEndian(data + 3, 3);
        universal_tmp = len;
        remainingHeaderBytes -= 3;
        char value[256];
        if (len > 249)
        {
            len = 249;
        }
        memcpy(value, (data + 7), len);
        value[len + 1] = 0;

        if (_util.startsWith(tag, "PIC"))
        { // image embedded in header

            APIC_seen = true; // #460
            APIC_pos = id3Size - remainingHeaderBytes;
            APIC_size = universal_tmp;
        }

        remainingHeaderBytes -= universal_tmp;
        universal_tmp -= len;

        if (len == 0)
            m_controlCounter = 98;
        if (remainingHeaderBytes == 0)
            m_controlCounter = 98;

        return 3 + 3 + len;
    }
    // -- end section V2.2 -----------

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 98)
    { // skip all ID3 metadata (mostly spaces)
        if (remainingHeaderBytes > 256)
        {
            remainingHeaderBytes -= 256;
            return 256;
        } // Throw it away
        else
        {
            m_controlCounter = 99;
            return remainingHeaderBytes;
        } // Throw it away
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 99)
    { //  exist another ID3tag?
        m_audioDataStart += id3Size;
        vTaskDelay(30);
        if ((*(data + 0) == 'I') && (*(data + 1) == 'D') && (*(data + 2) == '3'))
        {
            m_controlCounter = 0;
            return 0;
        }
        else
        {
            m_controlCounter = 100; // ok
            m_audioDataSize = m_contentlength - m_audioDataStart;
            return 0;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::stopSong()
{
    uint32_t pos = 0;

    if (m_f_running)
    {
        m_f_running = false;

        pos = getFilePos() - inBufferFilled();
        audiofile.close();
    }
    if (audiofile)
    {
        // added this before putting 'm_f_localfile = false' in stopSong(); shoulf never occur....
        audiofile.close();
        log_v("Stop song");
    }
    clearOutput();

    return pos;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::pauseResume()
{
    xSemaphoreTake(mutex_audio, portMAX_DELAY);
    bool retVal = false;

    m_f_running = !m_f_running;
    retVal = true;
    if (!m_f_running)
        clearOutput();

    xSemaphoreGive(mutex_audio);
    return retVal;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::playChunk()
{
    // If we've got data, try and pump it out..
    int16_t sample[2];
    if (getBitsPerSample() == 8)
    {
        if (getChannels() == 1)
        {
            while (m_validSamples)
            {
                uint8_t x = _audio_buff.getSample(_cur_sample) & 0x00FF;
                uint8_t y = (_audio_buff.getSample(_cur_sample) & 0xFF00) >> 8;

                sample[LEFTCHANNEL] = x;
                sample[RIGHTCHANNEL] = x;
                while (1)
                {
                    if (playSample(sample))
                        break;
                } // Can't send?
                sample[LEFTCHANNEL] = y;
                sample[RIGHTCHANNEL] = y;
                while (1)
                {
                    if (playSample(sample))
                        break;
                } // Can't send?
                m_validSamples--;
                _cur_sample++;
            }
        }
        if (getChannels() == 2)
        {
            while (m_validSamples)
            {
                uint8_t x = _audio_buff.getSample(_cur_sample) & 0x00FF;
                uint8_t y = (_audio_buff.getSample(_cur_sample) & 0xFF00) >> 8;
                if (!m_f_forceMono)
                { // stereo mode
                    sample[LEFTCHANNEL] = x;
                    sample[RIGHTCHANNEL] = y;
                }
                else
                { // force mono
                    uint8_t xy = (x + y) / 2;
                    sample[LEFTCHANNEL] = xy;
                    sample[RIGHTCHANNEL] = xy;
                }

                while (1)
                {
                    if (playSample(sample))
                        break;
                } // Can't send?
                m_validSamples--;
                _cur_sample++;
            }
        }
        _cur_sample = 0;
        return true;
    }
    if (getBitsPerSample() == 16)
    {
        if (getChannels() == 1)
        {
            while (m_validSamples)
            {
                sample[LEFTCHANNEL] = _audio_buff.getSample(_cur_sample);
                sample[RIGHTCHANNEL] = sample[LEFTCHANNEL];
                if (!playSample(sample))
                    return false;
                // Can't send
                m_validSamples--;
                _cur_sample++;
            }
        }
        if (getChannels() == 2)
        {
            _cur_sample = 0;
            while (m_validSamples)
            {
                if (!m_f_forceMono)
                { // stereo mode
                    sample[LEFTCHANNEL] = _audio_buff.getSample(_cur_sample * 2);
                    sample[RIGHTCHANNEL] = _audio_buff.getSample(_cur_sample * 2 + 1);
                }
                else
                { // mono mode, #100
                    int16_t xy = (_audio_buff.getSample(_cur_sample * 2) + _audio_buff.getSample(_cur_sample * 2 + 1)) / 2;
                    sample[LEFTCHANNEL] = xy;
                    sample[RIGHTCHANNEL] = xy;
                }
                playSample(sample);
                m_validSamples--;
                _cur_sample++;
            }
        }
        _cur_sample = 0;
        return true;
    }

    log_e("BitsPer Sample must be 8 or 16!");

    m_validSamples = 0;
    stopSong();
    return false;
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::loop()
{
    if (!m_f_running)
        return;

    _loop_counter++;

    xSemaphoreTake(mutex_audio, portMAX_DELAY);

    processLocalFile();

    xSemaphoreGive(mutex_audio);
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::processLocalFile()
{
    if (!(audiofile && m_f_running))
        return; // guard

    static uint32_t ctime = 0;
    const uint32_t timeout = 2500; // ms
    static bool f_stream;
    static bool f_fileDataComplete;
    static uint32_t byteCounter; // count received data
    uint32_t availableBytes = 0;

    if (m_f_firstCall)
    { // runs only one time per connection, prepare for start
        m_f_firstCall = false;
        f_stream = false;
        f_fileDataComplete = false;
        byteCounter = 0;
        ctime = millis();
        return;
    }

    availableBytes = 16 * 1024; // set some large value

    availableBytes = min(availableBytes, (uint32_t)_audio_buff.writeSpace());
    availableBytes = min(availableBytes, audiofile.size() - byteCounter);
    if (m_contentlength)
    {
        if (m_contentlength > getFilePos())
            availableBytes = min(availableBytes, m_contentlength - getFilePos());
    }
    if (m_audioDataSize)
    {
        availableBytes = min(availableBytes, m_audioDataSize + m_audioDataStart - byteCounter);
    }

    int32_t bytesAddedToBuffer = audiofile.read(_audio_buff.getWritePtr(), availableBytes);

    if (bytesAddedToBuffer > 0)
    {
        byteCounter += bytesAddedToBuffer; // Pull request #42
        _audio_buff.bytesWritten(bytesAddedToBuffer);
    }
    if (!f_stream)
    {
        if ((millis() - ctime) > timeout)
        {
            m_f_running = false;
            return;
        }
        if (m_controlCounter != 100)
        {
            if (_audio_buff.bufferFilled() > _audio_buff.IN_BUFF_CHUNK_SIZE)
            { // read the file header first
                _audio_buff.bytesWasRead(readAudioHeader(_audio_buff.bufferFilled()));
            }
            return;
        }
        else
        {
            if ((_audio_buff.freeSpace() > _audio_buff.IN_BUFF_CHUNK_SIZE) && (m_file_size - byteCounter) > _audio_buff.IN_BUFF_CHUNK_SIZE)
            {
                // fill the buffer before playing
                return;
            }

            f_stream = true;
        }
    }

    if (m_resumeFilePos)
    {
        if (m_resumeFilePos < m_audioDataStart)
            m_resumeFilePos = m_audioDataStart;
        if (m_resumeFilePos > m_file_size)
            m_resumeFilePos = m_file_size;

        // must be divisible by four
        m_resumeFilePos = mp3_correctResumeFilePos(m_resumeFilePos);

        if (m_avr_bitrate)
            m_audioCurrentTime = ((m_resumeFilePos - m_audioDataStart) / m_avr_bitrate) * 8;
        audiofile.seek(m_resumeFilePos);
        _audio_buff.resetInputBuffer();
        byteCounter = m_resumeFilePos;
        m_resumeFilePos = 0;
    }

    // end of file reached? - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (f_fileDataComplete && _audio_buff.bufferFilled() < _audio_buff.IN_BUFF_CHUNK_SIZE)
    {
        if (_audio_buff.bufferFilled())
        {
            if (!readID3V1Tag())
            {
                int bytesDecoded = sendBytes(_audio_buff.getReadPtr(), _audio_buff.bufferFilled());

                if (bytesDecoded <= _audio_buff.bufferFilled())
                { // avoid buff overrun (can be if file is corrupt)
                    if (m_f_playing)
                    {
                        if (bytesDecoded > 2)
                        {
                            _audio_buff.bytesWasRead(bytesDecoded);
                            return;
                        }
                    }
                }

                log_e("audio file is corrupt --> send EOF");
            }
        }

        if (m_f_loop && f_stream)
        { // eof
            // AUDIO_INFO("loop from: %u to: %u", getFilePos(), m_audioDataStart); // TEST loop
            setFilePos(m_audioDataStart);
            /*
                The current time of the loop mode is not reset,
                which will cause the total audio duration to be exceeded.
                For example: current time   ====progress bar====>  total audio duration
                                3:43        ====================>        3:33
            */
            m_audioCurrentTime = 0;
            byteCounter = m_audioDataStart;
            f_fileDataComplete = false;
            return;
        } // TEST loop

        m_f_running = false;
        audiofile.close();

        log_v("Closing audio file");

        MP3Decoder_FreeBuffers();
        return;
    }

    if (byteCounter == audiofile.size())
    {
        f_fileDataComplete = true;
    }
    if (byteCounter == m_audioDataSize + m_audioDataStart)
    {
        f_fileDataComplete = true;
    }

    // play audio data - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (f_stream)
    {
        static uint8_t cnt = 0;
        const uint8_t compression = 3;
        cnt++;
        if (cnt == compression)
        {
            playAudioData();
            cnt = 0;
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::playAudioData()
{
    if (_audio_buff.bufferFilled() < _audio_buff.IN_BUFF_CHUNK_SIZE)
        return; // guard

    int bytesDecoded = sendBytes(_audio_buff.getReadPtr(), _audio_buff.IN_BUFF_CHUNK_SIZE);

    if (bytesDecoded < 0)
    { // no syncword found or decode error, try next chunk

#ifdef IS_DEBUG
        Serial.print(F("err bytesDecoded: "));
        Serial.println(bytesDecoded);
#endif
        uint8_t next = 200;
        if (_audio_buff.bufferFilled() < next)
            next = _audio_buff.bufferFilled();
        _audio_buff.bytesWasRead(next); // try next chunk
        m_bytesNotDecoded += next;
    }
    else
    {
        if (bytesDecoded > 0)
        {
            _audio_buff.bytesWasRead(bytesDecoded);
            return;
        }
        if (bytesDecoded == 0)
            return; // syncword at pos0 found
    }

    return;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::initializeDecoder()
{
    if (!MP3Decoder_AllocateBuffers())
    {
        log_e("Codec could not be initialized");

        stopSong();
        return false;
    }

#ifdef IS_DEBUG
    uint32_t gfH = ESP.getFreeHeap();
    uint32_t hWM = uxTaskGetStackHighWaterMark(NULL);
    Serial.print(F("MP3Decoder has been initialized, "));
    Serial.print(F("free Heap: "));
    Serial.print(gfH);
    Serial.print(F(" free stack: "));
    Serial.println(hWM);
#endif

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void Audio::showCodecParams()
{

#ifdef IS_DEBUG
    Serial.print(F("Channels: "));
    Serial.println(getChannels());
    Serial.print(F("SampleRate: "));
    Serial.println(getSampleRate());

    Serial.print(F("BitsPerSample: "));
    Serial.println(getBitsPerSample());

    Serial.print(F("BitRate: "));
    uint32_t bitRate = getBitRate();
    if (bitRate)
        Serial.println(bitRate);
    else
        Serial.println(F("N/A"));

#endif
}

//---------------------------------------------------------------------------------------------------------------------
void Audio::setDecoderItems()
{
    setChannels(MP3GetChannels());
    setSampleRate(MP3GetSampRate());
    setBitsPerSample(MP3GetBitsPerSample());
    setBitrate(MP3GetBitrate());

    showCodecParams();
}
//---------------------------------------------------------------------------------------------------------------------
int Audio::sendBytes(uint8_t *data, size_t len)
{
    int bytesLeft;
    static bool f_setDecodeParamsOnce = true;

    if (!m_f_playing)
    {
        f_setDecodeParamsOnce = true;
        int nextSync = MP3FindSyncWord(data, len);

        if (nextSync == 0)
            m_f_playing = true;

        return nextSync;
    }
    // m_f_playing is true at this pos
    bytesLeft = len;
    int ret = 0;
    int bytesDecoded = 0;

    ret = MP3Decode(data, &bytesLeft, _audio_buff.getOutBuffer(), 0);

    bytesDecoded = len - bytesLeft;

    if (bytesDecoded == 0 && ret == 0)
    {
        m_f_playing = false; // seek for new syncword
        // we're here because there was a wrong sync word
        // so skip two sync bytes and seek for next
        return 1;
    }
    if (ret < 0)
    { // Error, skip the frame...
        if (getChannels() && ret != -2)
        {
            printDecodeError(ret);
            // m_f_playing = false; // seek for new syncword
            stopSong(); // Невідомий кодек.
        }

        if (!bytesDecoded)
            bytesDecoded = 2;

        return bytesDecoded;
    }
    else
    { // ret>=0
        m_validSamples = MP3GetOutputSamps() / getChannels();

        if (f_setDecodeParamsOnce)
        {
            f_setDecodeParamsOnce = false;
            setDecoderItems();
            m_PlayingStartTime = millis();
        }
    }
    compute_audioCurrentTime(bytesDecoded);

    while (m_validSamples)
    {
        playChunk();
    }
    return bytesDecoded;
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::compute_audioCurrentTime(int bd)
{
    static uint16_t loop_counter = 0;
    static int old_bitrate = 0;
    static uint64_t sum_bitrate = 0;
    static boolean f_CBR = true; // constant bitrate

    setBitrate(MP3GetBitrate());

    // if not CBR, bitrate can be changed
    if (!getBitRate())
        return;

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_avr_bitrate == 0)
    { // first time
        loop_counter = 0;
        old_bitrate = 0;
        sum_bitrate = 0;
        f_CBR = true;
        m_avr_bitrate = getBitRate();
        old_bitrate = getBitRate();
    }
    if (!m_avr_bitrate)
        return;
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (loop_counter < 1000)
        loop_counter++;

    if ((old_bitrate != getBitRate()) && f_CBR)
        f_CBR = false; // variable bitrate

    old_bitrate = getBitRate();

    if (!f_CBR)
    {
        if (loop_counter > 20 && loop_counter < 200)
        {
            // if VBR: m_avr_bitrate is average of the first values of m_bitrate
            sum_bitrate += getBitRate();
            m_avr_bitrate = sum_bitrate / (loop_counter - 20);
            if (loop_counter == 199 && m_resumeFilePos)
            {
                m_audioCurrentTime = ((getFilePos() - m_audioDataStart - inBufferFilled()) / m_avr_bitrate) * 8; // #293
            }
        }
    }
    else
    {
        if (loop_counter == 2)
        {
            m_avr_bitrate = getBitRate();
            if (m_resumeFilePos)
            {                                                                                                    // if connecttoFS() is called with resumeFilePos != 0
                m_audioCurrentTime = ((getFilePos() - m_audioDataStart - inBufferFilled()) / m_avr_bitrate) * 8; // #293
            }
        }
    }
    m_audioCurrentTime += ((float)bd / m_avr_bitrate) * 8;
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::printDecodeError(int r)
{
    const char *e;

    switch (r)
    {
    case ERR_MP3_NONE:
        e = "NONE";
        break;
    case ERR_MP3_INDATA_UNDERFLOW:
        e = "INDATA_UNDERFLOW";
        break;
    case ERR_MP3_MAINDATA_UNDERFLOW:
        e = "MAINDATA_UNDERFLOW";
        break;
    case ERR_MP3_FREE_BITRATE_SYNC:
        e = "FREE_BITRATE_SYNC";
        break;
    case ERR_MP3_OUT_OF_MEMORY:
        e = "OUT_OF_MEMORY";
        break;
    case ERR_MP3_NULL_POINTER:
        e = "NULL_POINTER";
        break;
    case ERR_MP3_INVALID_FRAMEHEADER:
        e = "INVALID_FRAMEHEADER";
        break;
    case ERR_MP3_INVALID_SIDEINFO:
        e = "INVALID_SIDEINFO";
        break;
    case ERR_MP3_INVALID_SCALEFACT:
        e = "INVALID_SCALEFACT";
        break;
    case ERR_MP3_INVALID_HUFFCODES:
        e = "INVALID_HUFFCODES";
        break;
    case ERR_MP3_INVALID_DEQUANTIZE:
        e = "INVALID_DEQUANTIZE";
        break;
    case ERR_MP3_INVALID_IMDCT:
        e = "INVALID_IMDCT";
        break;
    case ERR_MP3_INVALID_SUBBAND:
        e = "INVALID_SUBBAND";
        break;
    default:
        e = "ERR_UNKNOWN";
    }
#ifdef IS_DEBUG
    Serial.print(F("MP3 decode error "));
    Serial.print(r);
    Serial.print(" : ");
    Serial.println(e);
#endif
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setPinout(uint8_t BCLK, uint8_t LRC, uint8_t DOUT, int8_t DIN, int8_t MCK)
{
    m_pin_config.bck_io_num = BCLK;
    m_pin_config.ws_io_num = LRC; //  wclk
    m_pin_config.data_out_num = DOUT;
    m_pin_config.data_in_num = DIN;
#if (ESP_IDF_VERSION_MAJOR >= 4 && ESP_IDF_VERSION_MINOR >= 4)
    m_pin_config.mck_io_num = MCK;
#endif

    const esp_err_t result = i2s_set_pin((i2s_port_t)m_i2s_num, &m_pin_config);
    return (result == ESP_OK);
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getFileSize()
{
    if (!audiofile)
        return 0;
    return audiofile.size();
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getFilePos()
{
    if (!audiofile)
        return 0;
    return audiofile.position();
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioDataStartPos()
{
    if (!audiofile)
        return 0;
    return m_audioDataStart;
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioFileDuration()
{
    if (!audiofile)
        return 0;

    if (!m_avr_bitrate)
        return 0;

    m_audioFileDuration = 8 * (m_audioDataSize / m_avr_bitrate); // #289

    return m_audioFileDuration;
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioCurrentTime()
{ // return current time in seconds
    return (uint32_t)m_audioCurrentTime;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setAudioPlayPosition(uint16_t sec)
{
    // Jump to an absolute position in time within an audio file
    // e.g. setAudioPlayPosition(300) sets the pointer at pos 5 min
    if (sec > getAudioFileDuration())
        sec = getAudioFileDuration();
    uint32_t filepos = m_audioDataStart + (m_avr_bitrate * sec / 8);
    return setFilePos(filepos);
}
//---------------------------------------------------------------------------------------------------------------------
uint8_t Audio::maxVolume()
{
    return m_vol_steps;
};
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getTotalPlayingTime()
{
    // Is set to zero by a connectToXXX() and starts as soon as the first audio data is available,
    // the time counting is not interrupted by a 'pause / resume' and is not reset by a fileloop
    return millis() - m_PlayingStartTime;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setTimeOffset(int sec)
{
    // fast forward or rewind the current position in seconds
    // audiosource must be a mp3

    if (!audiofile || !m_avr_bitrate)
        return false;

    uint32_t oneSec = m_avr_bitrate / 8;                 // bytes decoded in one sec
    int32_t offset = oneSec * sec;                       // bytes to be wind/rewind
    uint32_t startAB = m_audioDataStart;                 // audioblock begin
    uint32_t endAB = m_audioDataStart + m_audioDataSize; // audioblock end

    int32_t pos = getFilePos() - inBufferFilled();
    pos += offset;
    if (pos < (int32_t)startAB)
        pos = startAB;
    if (pos >= (int32_t)endAB)
        pos = endAB;
    setFilePos(pos);
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setFilePos(uint32_t pos)
{
    // xSemaphoreTakeRecursive(mutex_audio, portMAX_DELAY); //#497
    if (!audiofile)
        return false;
    if (pos < m_audioDataStart)
        pos = m_audioDataStart; // issue #96
    if (pos > m_file_size)
        pos = m_file_size;
    m_resumeFilePos = pos;
    // xSemaphoreGiveRecursive(mutex_audio);
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::audioFileSeek(const float speed)
{
    // 0.5 is half speed
    // 1.0 is normal speed
    // 1.5 is one and half speed
    if ((speed > 1.5f) || (speed < 0.25f))
        return false;

    uint32_t srate = getSampleRate() * speed;
    i2s_set_sample_rates((i2s_port_t)m_i2s_num, srate);
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setSampleRate(uint32_t sampRate)
{
    if (!sampRate)
        sampRate = 16000; // fuse, if there is no value -> set default #209
    i2s_set_sample_rates((i2s_port_t)m_i2s_num, sampRate);
    m_sampleRate = sampRate;
    IIR_calculateCoefficients(m_gain0, m_gain1, m_gain2); // must be recalculated after each samplerate change
    return true;
}

uint32_t Audio::getSampleRate()
{
    return m_sampleRate;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setBitsPerSample(int bits)
{
    if ((bits != 16) && (bits != 8))
        return false;
    m_bitsPerSample = bits;
    return true;
}

uint8_t Audio::getBitsPerSample()
{
    return m_bitsPerSample;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setChannels(int ch)
{
    if ((ch < 1) || (ch > 2))
        return false;
    m_channels = ch;
    return true;
}
uint8_t Audio::getChannels()
{
    if (m_channels == 0)
    { // this should not happen! #209
        m_channels = 2;
    }
    return m_channels;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::setBitrate(int br)
{
    m_bitRate = br;
    if (br)
        return true;
    return false;
}
uint32_t Audio::getBitRate(bool avg)
{
    if (avg)
        return m_avr_bitrate;
    return m_bitRate;
}
//---------------------------------------------------------------------------------------------------------------------
bool Audio::playSample(int16_t sample[2])
{
    if (getBitsPerSample() == 8)
    { // Upsample from unsigned 8 bits to signed 16 bits
        sample[LEFTCHANNEL] = ((sample[LEFTCHANNEL] & 0xff) - 128) << 8;
        sample[RIGHTCHANNEL] = ((sample[RIGHTCHANNEL] & 0xff) - 128) << 8;
    }

    // set a correction factor if filter have positive amplification
    if (m_corr > 1)
    {
        sample[LEFTCHANNEL] = sample[LEFTCHANNEL] / m_corr;
        sample[RIGHTCHANNEL] = sample[RIGHTCHANNEL] / m_corr;
    }

    // Filterchain, can commented out if not used
    // sample = IIR_filterChain0(sample);
    // sample = IIR_filterChain1(sample);
    // sample = IIR_filterChain2(sample);
    //-------------------------------------------

    uint32_t s32 = Gain(sample); // vosample2lume;

    if (m_f_internalDAC)
        s32 += 0x80008000;

    m_i2s_bytesWritten = 0;

    esp_err_t err = i2s_write((i2s_port_t)m_i2s_num, (const char *)&s32, sizeof(uint32_t), &m_i2s_bytesWritten, 100);

    if (err != ESP_OK)
    {
#ifdef IS_DEBUG
        Serial.print(F("ESP32 i2s_write error: "));
        Serial.println(err);
#endif
        return false;
    }
    if (m_i2s_bytesWritten < 4)
    {
        log_e("Can't stuff any more in I2S...");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::setTone(int8_t gainLowPass, int8_t gainBandPass, int8_t gainHighPass)
{
    // see https://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/
    // values can be between -40 ... +6 (dB)

    m_gain0 = gainLowPass;
    m_gain1 = gainBandPass;
    m_gain2 = gainHighPass;

    // gain, attenuation (set in digital filters)
    int db = max(m_gain0, max(m_gain1, m_gain2));
    m_corr = pow10f((float)db / 20);

    IIR_calculateCoefficients(m_gain0, m_gain1, m_gain2);

    /*
        This will cause a clicking sound when adjusting the EQ.
        Because when the EQ is adjusted, the IIR filter will be cleared and played,
        mixed in the audio data frame, and a click-like sound will be produced.
    */

    int16_t tmp[2];
    tmp[0] = 0;
    tmp[1] = 0;

    IIR_filterChain0(tmp, true); // flush the filter
    IIR_filterChain1(tmp, true); // flush the filter
    IIR_filterChain2(tmp, true); // flush the filter
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::forceMono(bool m)
{                      // #100 mono option
    m_f_forceMono = m; // false stereo, true mono
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::setBalance(int8_t bal)
{ // bal -16...16
    if (bal < -16)
        bal = -16;
    if (bal > 16)
        bal = 16;
    m_balance = bal;
}
//---------------------------------------------------------------------------------------------------------------------
void Audio::setVolume(uint8_t vol)
{
    if (vol > m_vol_steps)
        vol = m_vol_steps;
    m_vol = vol * vol;
    return;
}
//---------------------------------------------------------------------------------------------------------------------
uint8_t Audio::getVolume()
{
    uint8_t vol = sqrt(m_vol);
    return vol;
}
//---------------------------------------------------------------------------------------------------------------------
uint8_t Audio::getI2sPort()
{
    return m_i2s_num;
}
//---------------------------------------------------------------------------------------------------------------------
int32_t Audio::Gain(int16_t s[2])
{
    int32_t v[2];
    int32_t l = m_vol, r = m_vol;

    /* balance is left -16...+16 right */
    /* TODO: logarithmic scaling of balance, too? */
    if (m_balance < 0)
    {
        r -= (int32_t)m_vol * abs(m_balance) / 16;
    }
    else if (m_balance > 0)
    {
        l -= (int32_t)m_vol * abs(m_balance) / 16;
    }

    /* important: these multiplications must all be signed ints, or the result will be invalid */
    v[LEFTCHANNEL] = (s[LEFTCHANNEL] * l) / m_vol_step_div;
    v[RIGHTCHANNEL] = (s[RIGHTCHANNEL] * r) / m_vol_step_div;

    return (v[LEFTCHANNEL] << 16) | (v[RIGHTCHANNEL] & 0xffff);
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferFilled()
{
    // current audio input buffer fillsize in bytes
    return _audio_buff.bufferFilled();
}
//---------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferFree()
{
    // current audio input buffer free space in bytes
    return _audio_buff.freeSpace();
}
//---------------------------------------------------------------------------------------------------------------------
//            ***     D i g i t a l   b i q u a d r a t i c     f i l t e r     ***
//---------------------------------------------------------------------------------------------------------------------
void Audio::IIR_calculateCoefficients(int8_t G0, int8_t G1, int8_t G2)
{ // Infinite Impulse Response (IIR) filters

    // G1 - gain low shelf   set between -40 ... +6 dB
    // G2 - gain peakEQ      set between -40 ... +6 dB
    // G3 - gain high shelf  set between -40 ... +6 dB
    // https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/

    if (getSampleRate() < 1000)
        return; // fuse

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (G0 < -40)
        G0 = -40; // -40dB -> Vin*0.01
    if (G0 > 6)
        G0 = 6; // +6dB -> Vin*2
    if (G1 < -40)
        G1 = -40;
    if (G1 > 6)
        G1 = 6;
    if (G2 < -40)
        G2 = -40;
    if (G2 > 6)
        G2 = 6;

    const float FcLS = 500;    // Frequency LowShelf[Hz]
    const float FcPKEQ = 3000; // Frequency PeakEQ[Hz]
    const float FcHS = 6000;   // Frequency HighShelf[Hz]

    float K, norm, Q, Fc, V;

    // LOWSHELF
    Fc = (float)FcLS / (float)getSampleRate(); // Cutoff frequency
    K = tanf((float)PI * Fc);
    V = powf(10, fabs(G0) / 20.0);

    if (G0 >= 0)
    { // boost
        norm = 1 / (1 + sqrtf(2) * K + K * K);
        m_filter[LOWSHELF].a0 = (1 + sqrtf(2 * V) * K + V * K * K) * norm;
        m_filter[LOWSHELF].a1 = 2 * (V * K * K - 1) * norm;
        m_filter[LOWSHELF].a2 = (1 - sqrtf(2 * V) * K + V * K * K) * norm;
        m_filter[LOWSHELF].b1 = 2 * (K * K - 1) * norm;
        m_filter[LOWSHELF].b2 = (1 - sqrtf(2) * K + K * K) * norm;
    }
    else
    { // cut
        norm = 1 / (1 + sqrtf(2 * V) * K + V * K * K);
        m_filter[LOWSHELF].a0 = (1 + sqrtf(2) * K + K * K) * norm;
        m_filter[LOWSHELF].a1 = 2 * (K * K - 1) * norm;
        m_filter[LOWSHELF].a2 = (1 - sqrtf(2) * K + K * K) * norm;
        m_filter[LOWSHELF].b1 = 2 * (V * K * K - 1) * norm;
        m_filter[LOWSHELF].b2 = (1 - sqrtf(2 * V) * K + V * K * K) * norm;
    }

    // PEAK EQ
    Fc = (float)FcPKEQ / (float)getSampleRate(); // Cutoff frequency
    K = tanf((float)PI * Fc);
    V = powf(10, fabs(G1) / 20.0);
    Q = 2.5; // Quality factor
    if (G1 >= 0)
    { // boost
        norm = 1 / (1 + 1 / Q * K + K * K);
        m_filter[PEAKEQ].a0 = (1 + V / Q * K + K * K) * norm;
        m_filter[PEAKEQ].a1 = 2 * (K * K - 1) * norm;
        m_filter[PEAKEQ].a2 = (1 - V / Q * K + K * K) * norm;
        m_filter[PEAKEQ].b1 = m_filter[PEAKEQ].a1;
        m_filter[PEAKEQ].b2 = (1 - 1 / Q * K + K * K) * norm;
    }
    else
    { // cut
        norm = 1 / (1 + V / Q * K + K * K);
        m_filter[PEAKEQ].a0 = (1 + 1 / Q * K + K * K) * norm;
        m_filter[PEAKEQ].a1 = 2 * (K * K - 1) * norm;
        m_filter[PEAKEQ].a2 = (1 - 1 / Q * K + K * K) * norm;
        m_filter[PEAKEQ].b1 = m_filter[PEAKEQ].a1;
        m_filter[PEAKEQ].b2 = (1 - V / Q * K + K * K) * norm;
    }

    // HIGHSHELF
    Fc = (float)FcHS / (float)getSampleRate(); // Cutoff frequency
    K = tanf((float)PI * Fc);
    V = powf(10, fabs(G2) / 20.0);
    if (G2 >= 0)
    { // boost
        norm = 1 / (1 + sqrtf(2) * K + K * K);
        m_filter[HIFGSHELF].a0 = (V + sqrtf(2 * V) * K + K * K) * norm;
        m_filter[HIFGSHELF].a1 = 2 * (K * K - V) * norm;
        m_filter[HIFGSHELF].a2 = (V - sqrtf(2 * V) * K + K * K) * norm;
        m_filter[HIFGSHELF].b1 = 2 * (K * K - 1) * norm;
        m_filter[HIFGSHELF].b2 = (1 - sqrtf(2) * K + K * K) * norm;
    }
    else
    {
        norm = 1 / (V + sqrtf(2 * V) * K + K * K);
        m_filter[HIFGSHELF].a0 = (1 + sqrtf(2) * K + K * K) * norm;
        m_filter[HIFGSHELF].a1 = 2 * (K * K - 1) * norm;
        m_filter[HIFGSHELF].a2 = (1 - sqrtf(2) * K + K * K) * norm;
        m_filter[HIFGSHELF].b1 = 2 * (K * K - V) * norm;
        m_filter[HIFGSHELF].b2 = (V - sqrtf(2 * V) * K + K * K) * norm;
    }
}
//---------------------------------------------------------------------------------------------------------------------
int16_t *Audio::IIR_filterChain0(int16_t iir_in[2], bool clear)
{ // Infinite Impulse Response (IIR) filters

    uint8_t z1 = 0, z2 = 1;
    enum : uint8_t
    {
        in = 0,
        out = 1
    };
    float inSample[2];
    float outSample[2];
    static int16_t iir_out[2];

    if (clear)
    {
        memset(m_filterBuff, 0, sizeof(m_filterBuff)); // zero IIR filterbuffer
        iir_out[0] = 0;
        iir_out[1] = 0;
        iir_in[0] = 0;
        iir_in[1] = 0;
    }

    inSample[LEFTCHANNEL] = (float)(iir_in[LEFTCHANNEL]);
    inSample[RIGHTCHANNEL] = (float)(iir_in[RIGHTCHANNEL]);

    outSample[LEFTCHANNEL] = m_filter[0].a0 * inSample[LEFTCHANNEL] + m_filter[0].a1 * m_filterBuff[0][z1][in][LEFTCHANNEL] + m_filter[0].a2 * m_filterBuff[0][z2][in][LEFTCHANNEL] - m_filter[0].b1 * m_filterBuff[0][z1][out][LEFTCHANNEL] - m_filter[0].b2 * m_filterBuff[0][z2][out][LEFTCHANNEL];

    m_filterBuff[0][z2][in][LEFTCHANNEL] = m_filterBuff[0][z1][in][LEFTCHANNEL];
    m_filterBuff[0][z1][in][LEFTCHANNEL] = inSample[LEFTCHANNEL];
    m_filterBuff[0][z2][out][LEFTCHANNEL] = m_filterBuff[0][z1][out][LEFTCHANNEL];
    m_filterBuff[0][z1][out][LEFTCHANNEL] = outSample[LEFTCHANNEL];
    iir_out[LEFTCHANNEL] = (int16_t)outSample[LEFTCHANNEL];

    outSample[RIGHTCHANNEL] = m_filter[0].a0 * inSample[RIGHTCHANNEL] + m_filter[0].a1 * m_filterBuff[0][z1][in][RIGHTCHANNEL] + m_filter[0].a2 * m_filterBuff[0][z2][in][RIGHTCHANNEL] - m_filter[0].b1 * m_filterBuff[0][z1][out][RIGHTCHANNEL] - m_filter[0].b2 * m_filterBuff[0][z2][out][RIGHTCHANNEL];

    m_filterBuff[0][z2][in][RIGHTCHANNEL] = m_filterBuff[0][z1][in][RIGHTCHANNEL];
    m_filterBuff[0][z1][in][RIGHTCHANNEL] = inSample[RIGHTCHANNEL];
    m_filterBuff[0][z2][out][RIGHTCHANNEL] = m_filterBuff[0][z1][out][RIGHTCHANNEL];
    m_filterBuff[0][z1][out][RIGHTCHANNEL] = outSample[RIGHTCHANNEL];
    iir_out[RIGHTCHANNEL] = (int16_t)outSample[RIGHTCHANNEL];

    return iir_out;
}
//---------------------------------------------------------------------------------------------------------------------
int16_t *Audio::IIR_filterChain1(int16_t iir_in[2], bool clear)
{ // Infinite Impulse Response (IIR) filters

    uint8_t z1 = 0, z2 = 1;
    enum : uint8_t
    {
        in = 0,
        out = 1
    };
    float inSample[2];
    float outSample[2];
    static int16_t iir_out[2];

    if (clear)
    {
        memset(m_filterBuff, 0, sizeof(m_filterBuff)); // zero IIR filterbuffer
        iir_out[0] = 0;
        iir_out[1] = 0;
        iir_in[0] = 0;
        iir_in[1] = 0;
    }

    inSample[LEFTCHANNEL] = (float)(iir_in[LEFTCHANNEL]);
    inSample[RIGHTCHANNEL] = (float)(iir_in[RIGHTCHANNEL]);

    outSample[LEFTCHANNEL] = m_filter[1].a0 * inSample[LEFTCHANNEL] + m_filter[1].a1 * m_filterBuff[1][z1][in][LEFTCHANNEL] + m_filter[1].a2 * m_filterBuff[1][z2][in][LEFTCHANNEL] - m_filter[1].b1 * m_filterBuff[1][z1][out][LEFTCHANNEL] - m_filter[1].b2 * m_filterBuff[1][z2][out][LEFTCHANNEL];

    m_filterBuff[1][z2][in][LEFTCHANNEL] = m_filterBuff[1][z1][in][LEFTCHANNEL];
    m_filterBuff[1][z1][in][LEFTCHANNEL] = inSample[LEFTCHANNEL];
    m_filterBuff[1][z2][out][LEFTCHANNEL] = m_filterBuff[1][z1][out][LEFTCHANNEL];
    m_filterBuff[1][z1][out][LEFTCHANNEL] = outSample[LEFTCHANNEL];
    iir_out[LEFTCHANNEL] = (int16_t)outSample[LEFTCHANNEL];

    outSample[RIGHTCHANNEL] = m_filter[1].a0 * inSample[RIGHTCHANNEL] + m_filter[1].a1 * m_filterBuff[1][z1][in][RIGHTCHANNEL] + m_filter[1].a2 * m_filterBuff[1][z2][in][RIGHTCHANNEL] - m_filter[1].b1 * m_filterBuff[1][z1][out][RIGHTCHANNEL] - m_filter[1].b2 * m_filterBuff[1][z2][out][RIGHTCHANNEL];

    m_filterBuff[1][z2][in][RIGHTCHANNEL] = m_filterBuff[1][z1][in][RIGHTCHANNEL];
    m_filterBuff[1][z1][in][RIGHTCHANNEL] = inSample[RIGHTCHANNEL];
    m_filterBuff[1][z2][out][RIGHTCHANNEL] = m_filterBuff[1][z1][out][RIGHTCHANNEL];
    m_filterBuff[1][z1][out][RIGHTCHANNEL] = outSample[RIGHTCHANNEL];
    iir_out[RIGHTCHANNEL] = (int16_t)outSample[RIGHTCHANNEL];

    return iir_out;
}
//---------------------------------------------------------------------------------------------------------------------
int16_t *Audio::IIR_filterChain2(int16_t iir_in[2], bool clear)
{ // Infinite Impulse Response (IIR) filters

    uint8_t z1 = 0, z2 = 1;
    enum : uint8_t
    {
        in = 0,
        out = 1
    };
    float inSample[2];
    float outSample[2];
    static int16_t iir_out[2];

    if (clear)
    {
        memset(m_filterBuff, 0, sizeof(m_filterBuff)); // zero IIR filterbuffer
        iir_out[0] = 0;
        iir_out[1] = 0;
        iir_in[0] = 0;
        iir_in[1] = 0;
    }

    inSample[LEFTCHANNEL] = (float)(iir_in[LEFTCHANNEL]);
    inSample[RIGHTCHANNEL] = (float)(iir_in[RIGHTCHANNEL]);

    outSample[LEFTCHANNEL] = m_filter[2].a0 * inSample[LEFTCHANNEL] + m_filter[2].a1 * m_filterBuff[2][z1][in][LEFTCHANNEL] + m_filter[2].a2 * m_filterBuff[2][z2][in][LEFTCHANNEL] - m_filter[2].b1 * m_filterBuff[2][z1][out][LEFTCHANNEL] - m_filter[2].b2 * m_filterBuff[2][z2][out][LEFTCHANNEL];

    m_filterBuff[2][z2][in][LEFTCHANNEL] = m_filterBuff[2][z1][in][LEFTCHANNEL];
    m_filterBuff[2][z1][in][LEFTCHANNEL] = inSample[LEFTCHANNEL];
    m_filterBuff[2][z2][out][LEFTCHANNEL] = m_filterBuff[2][z1][out][LEFTCHANNEL];
    m_filterBuff[2][z1][out][LEFTCHANNEL] = outSample[LEFTCHANNEL];
    iir_out[LEFTCHANNEL] = (int16_t)outSample[LEFTCHANNEL];

    outSample[RIGHTCHANNEL] = m_filter[2].a0 * inSample[RIGHTCHANNEL] + m_filter[2].a1 * m_filterBuff[2][z1][in][RIGHTCHANNEL] + m_filter[2].a2 * m_filterBuff[2][z2][in][RIGHTCHANNEL] - m_filter[2].b1 * m_filterBuff[2][z1][out][RIGHTCHANNEL] - m_filter[2].b2 * m_filterBuff[2][z2][out][RIGHTCHANNEL];

    m_filterBuff[2][z2][in][RIGHTCHANNEL] = m_filterBuff[2][z1][in][RIGHTCHANNEL];
    m_filterBuff[2][z1][in][RIGHTCHANNEL] = inSample[RIGHTCHANNEL];
    m_filterBuff[2][z2][out][RIGHTCHANNEL] = m_filterBuff[2][z1][out][RIGHTCHANNEL];
    m_filterBuff[2][z1][out][RIGHTCHANNEL] = outSample[RIGHTCHANNEL];
    iir_out[RIGHTCHANNEL] = (int16_t)outSample[RIGHTCHANNEL];

    return iir_out;
}
//----------------------------------------------------------------------------------------------------------------------
bool Audio::readID3V1Tag()
{
    if ((_audio_buff.bufferFilled() == 128 && _util.startsWith((const char *)_audio_buff.getReadPtr(), "TAG")) ||
        (_audio_buff.bufferFilled() == 227 && _util.startsWith((const char *)_audio_buff.getReadPtr(), "TAG+")))
        return true;

    return false;
}
//----------------------------------------------------------------------------------------------------------------------
uint32_t Audio::mp3_correctResumeFilePos(uint32_t resumeFilePos)
{
    // The starting point is the next MP3 syncword
    uint8_t p1, p2;
    boolean found = false;
    uint32_t pos = resumeFilePos;
    audiofile.seek(pos);

    p1 = audiofile.read();
    p2 = audiofile.read();
    pos += 2;
    while (!found || pos == m_file_size)
    {
        if (p1 == 0xFF && (p2 & 0xF0) == 0xF0)
        {
            found = true;
            break;
        }
        p1 = p2;
        p2 = audiofile.read();
        pos++;
    }
    MP3Decoder_ClearBuffer();
    if (found)
        return (pos - 2);
    return m_audioDataStart;
}

void Audio::clearOutput()
{
    _audio_buff.clearOutputBuffer();
    i2s_zero_dma_buffer((i2s_port_t)m_i2s_num);
}

//----------------------------------------------------------------------------------------------------------------------
