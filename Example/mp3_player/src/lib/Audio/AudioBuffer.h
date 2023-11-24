#pragma once
#pragma GCC optimize("Ofast")
#include <Arduino.h>

class AudioBuffer
{
public:
    const uint8_t IN_BUFF_CHUNK_NUM{5};
    const uint16_t IN_BUFF_CHUNK_SIZE{1600};

    AudioBuffer();
    ~AudioBuffer();
    bool init();
    bool isInitialized() { return m_f_init; };
    size_t freeSpace();           // number of free bytes to overwrite
    size_t writeSpace();          // space fom writepointer to bufferend
    size_t bufferFilled();        // returns the number of filled bytes
    void bytesWritten(size_t bw); // update writepointer
    void bytesWasRead(size_t br); // update readpointer
    uint8_t *getWritePtr();       // returns the current writepointer
    uint8_t *getReadPtr();        // returns the current readpointer
    uint32_t getWritePos();       // write position relative to the beginning
    uint32_t getReadPos();        // read position relative to the beginning
    void resetInputBuffer();      // restore defaults

    int16_t getSample(uint16_t sample);
    int16_t *getOutBuffer();
    void clearOutputBuffer();

private:
    const uint32_t OUT_BUFF_SIZE{2048 * 2 * sizeof(int16_t)};

    uint8_t *_in_buffer = NULL;
    int16_t *_out_buffer = NULL;

    size_t _in_buff_size = 0;
    size_t m_freeSpace = 0;
    size_t m_writeSpace = 0;
    size_t m_dataLength = 0;
    uint8_t *m_writePtr = NULL;
    uint8_t *m_readPtr = NULL;
    uint8_t *m_endPtr = NULL;
    bool m_f_start = true;
    bool m_f_init = false;

    void deleteBuffer();
};
