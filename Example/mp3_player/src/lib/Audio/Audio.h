/*
 *  Version 3.0.1i
 *  Author: Wolle (schreibfaul1)
 *
 *
 * Відредаговано by Kolodieiev.
 * Список змін в readme.
 */

#pragma once
#pragma GCC optimize("Ofast")
#include <Arduino.h>
#include <libb64/cencode.h>
#include <esp32-hal-log.h>

#include <driver/i2s.h>

#include <SD.h>
#include <SD_MMC.h>
#include <SPIFFS.h>
#include <FS.h>

#include "AudioUtil.h"
#include "AudioBuffer.h"

class Audio
{

public:
    Audio(bool internalDAC = false, uint8_t channelEnabled = 3, uint8_t i2sPort = I2S_NUM_0); // #99
    ~Audio();

    bool hasCorrectData() { return _loop_counter > 300; }

    void loop();
    uint32_t stopSong();
    bool connecttoFS(fs::FS &fs, const char *path, uint32_t resumeFilePos = 0);
    bool connecttoSD(const char *path, uint32_t resumeFilePos = 0);
    void setFileLoop(bool state);
    bool setAudioPlayPosition(uint16_t sec);
    bool setFilePos(uint32_t pos);
    bool audioFileSeek(const float speed);
    bool setTimeOffset(int sec);
    bool setPinout(uint8_t BCLK, uint8_t LRC, uint8_t DOUT, int8_t DIN = I2S_PIN_NO_CHANGE, int8_t MCK = I2S_PIN_NO_CHANGE);
    bool pauseResume();
    bool isRunning() { return m_f_running; }
    void setTone(int8_t gainLowPass, int8_t gainBandPass, int8_t gainHighPass);

    void forceMono(bool m);
    void setBalance(int8_t bal = 0);
    void setVolume(uint8_t vol);
    uint8_t getVolume();
    uint8_t maxVolume();
    uint8_t getI2sPort();

    uint32_t getAudioDataStartPos();
    uint32_t getFileSize();
    uint32_t getFilePos();
    uint32_t getSampleRate();
    uint8_t getBitsPerSample();
    uint8_t getChannels();
    uint32_t getBitRate(bool avg = false);
    uint32_t getAudioFileDuration();
    uint32_t getAudioCurrentTime();
    uint32_t getTotalPlayingTime();

    uint32_t inBufferFilled(); // returns the number of stored bytes in the inputbuffer
    uint32_t inBufferFree();   // returns the number of free bytes in the inputbuffer
private:
    enum : int
    {
        APLL_AUTO = -1,
        APLL_ENABLE = 1,
        APLL_DISABLE = 0
    };
    enum : int
    {
        EXTERNAL_I2S = 0,
        INTERNAL_DAC = 1,
        INTERNAL_PDM = 2
    };

    enum : int
    {
        CODEC_NONE = 0,
        CODEC_MP3
    };

    typedef enum
    {
        LEFTCHANNEL = 0,
        RIGHTCHANNEL = 1
    } SampleIndex;

    typedef enum
    {
        LOWSHELF = 0,
        PEAKEQ = 1,
        HIFGSHELF = 2
    } FilterType;

    typedef struct _filter
    {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
    } filter_t;

    typedef struct _pis_array
    {
        int number;
        int pids[4];
    } pid_array;

    unsigned long _loop_counter{0};

    AudioUtil _util;
    AudioBuffer _audio_buff; // instance of input buffer

    File audiofile; // @suppress("Abstract class cannot be instantiated")
    SemaphoreHandle_t mutex_audio;
    i2s_config_t m_i2s_config = {}; // stores values for I2S driver
    i2s_pin_config_t m_pin_config = {};

    filter_t m_filter[3]; // digital filters
    uint32_t m_sampleRate = 16000;
    uint32_t m_bitRate = 0;                             // current bitrate given from decoder
    uint32_t m_avr_bitrate = 0;                         // average bitrate, median computed by VBR
    int m_controlCounter = 0;                           // Status within readID3data() and readWaveHeader()
    int8_t m_balance = 0;                               // -16 (mute left) ... +16 (mute right)
    uint16_t m_vol = 64;                                // volume
    uint8_t m_vol_steps = 64;                           // default
    int32_t m_vol_step_div = m_vol_steps * m_vol_steps; //
    uint8_t m_bitsPerSample = 16;                       // bitsPerSample
    uint8_t m_channels = 2;
    uint8_t m_i2s_num = I2S_NUM_0; // I2S_NUM_0 or I2S_NUM_1
    uint8_t m_ID3Size = 0;         // lengt of ID3frame - ID3header
    int16_t m_validSamples = 0;
    int16_t _cur_sample = 0;
    uint32_t m_contentlength = 0;    // Stores the length if the stream comes from fileserver
    uint32_t m_bytesNotDecoded = 0;  // pictures or something else that comes with the stream
    uint32_t m_PlayingStartTime = 0; // Stores the milliseconds after the start of the audio
    uint32_t m_resumeFilePos = 0;    // the return value from stopSong() can be entered here
    bool m_f_unsync = false;         // set within ID3 tag but not used
    bool m_f_exthdr = false;         // ID3 extended header
    bool m_f_running = false;
    bool m_f_firstCall = true;      // InitSequence for processWebstream and processLokalFile
    bool m_f_playing = false;       // valid mp3 stream recognized
    bool m_f_loop = false;          // Set if audio file should loop
    bool m_f_forceMono = false;     // if true stereo -> mono
    bool m_f_internalDAC = false;   // false: output vis I2S, true output via internal DAC
    uint8_t m_f_channelEnabled = 3; // internal DAC, both channels
    uint32_t m_audioFileDuration = 0;
    float m_audioCurrentTime = 0;
    uint32_t m_audioDataStart = 0;  // in bytes
    size_t m_audioDataSize = 0;     //
    float m_filterBuff[3][2][2][2]; // IIR filters memory for Audio DSP
    float m_corr = 1.0;             // correction factor for level adjustment
    size_t m_i2s_bytesWritten = 0;  // set in i2s_write() but not used
    size_t m_file_size = 0;         // size of the file
    int8_t m_gain0 = 0;             // cut or boost filters (EQ)
    int8_t m_gain1 = 0;
    int8_t m_gain2 = 0;

    void UTF8toASCII(char *str);
    void setDefaults(); // free buffers and set defaults
    void processLocalFile();
    void playAudioData();
    void showCodecParams();

    int sendBytes(uint8_t *data, size_t len);
    void setDecoderItems();
    void compute_audioCurrentTime(int bd);
    void printDecodeError(int r);
    size_t readAudioHeader(uint32_t bytes);
    int read_ID3_Header(const uint8_t *data, size_t len);
    bool setSampleRate(uint32_t hz);
    bool setBitsPerSample(int bits);
    bool setChannels(int channels);
    bool setBitrate(int br);
    bool playChunk();
    bool playSample(int16_t sample[2]);
    int32_t Gain(int16_t s[2]);
    bool initializeDecoder();
    esp_err_t I2Sstart(uint8_t i2s_num);
    esp_err_t I2Sstop(uint8_t i2s_num);
    int16_t *IIR_filterChain0(int16_t iir_in[2], bool clear = false);
    int16_t *IIR_filterChain1(int16_t *iir_in, bool clear = false);
    int16_t *IIR_filterChain2(int16_t *iir_in, bool clear = false);
    void IIR_calculateCoefficients(int8_t G1, int8_t G2, int8_t G3);

    bool readID3V1Tag();
    uint32_t mp3_correctResumeFilePos(uint32_t resumeFilePos);

    void clearOutput();
};