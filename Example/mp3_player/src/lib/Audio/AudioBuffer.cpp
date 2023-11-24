// #define IS_DEBUG
#pragma GCC optimize("Ofast")
#include "AudioBuffer.h"

AudioBuffer::AudioBuffer()
{
}

AudioBuffer::~AudioBuffer()
{
    deleteBuffer();
}

void AudioBuffer::deleteBuffer()
{
    if (_in_buffer)
    {
        heap_caps_free(_in_buffer);
        _in_buffer = NULL;
    }

    if (_out_buffer)
    {
        heap_caps_free(_out_buffer);
        _out_buffer = NULL;
    }
}

bool AudioBuffer::init()
{
    deleteBuffer();

    _in_buffer = (uint8_t *)heap_caps_calloc(IN_BUFF_CHUNK_NUM, IN_BUFF_CHUNK_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    _out_buffer = (int16_t *)heap_caps_malloc(OUT_BUFF_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);

    if (!_in_buffer)
    {
        log_e("Input buffer BAD ALLOC");
        return false;
    }

    if (!_out_buffer)
    {
        log_e("Output buffer BAD ALLOC");
        return false;
    }

    _in_buff_size = IN_BUFF_CHUNK_NUM * IN_BUFF_CHUNK_SIZE - IN_BUFF_CHUNK_SIZE;

    m_f_init = true;
    resetInputBuffer();

    return true;
}

size_t AudioBuffer::freeSpace()
{
    if (m_readPtr >= m_writePtr)
    {
        m_freeSpace = (m_readPtr - m_writePtr);
    }
    else
    {
        m_freeSpace = (m_endPtr - m_writePtr) + (m_readPtr - _in_buffer);
    }
    if (m_f_start)
        m_freeSpace = _in_buff_size;
    return m_freeSpace - 1;
}

size_t AudioBuffer::writeSpace()
{
    if (m_readPtr >= m_writePtr)
    {
        m_writeSpace = (m_readPtr - m_writePtr - 1); // readPtr must not be overtaken
    }
    else
    {
        if (getReadPos() == 0)
            m_writeSpace = (m_endPtr - m_writePtr - 1);
        else
            m_writeSpace = (m_endPtr - m_writePtr);
    }
    if (m_f_start)
        m_writeSpace = _in_buff_size - 1;
    return m_writeSpace;
}

size_t AudioBuffer::bufferFilled()
{
    if (m_writePtr >= m_readPtr)
    {
        m_dataLength = (m_writePtr - m_readPtr);
    }
    else
    {
        m_dataLength = (m_endPtr - m_readPtr) + (m_writePtr - _in_buffer);
    }
    return m_dataLength;
}

void AudioBuffer::bytesWritten(size_t bw)
{
    m_writePtr += bw;
    if (m_writePtr == m_endPtr)
    {
        m_writePtr = _in_buffer;
    }
    if (bw && m_f_start)
        m_f_start = false;
}

void AudioBuffer::bytesWasRead(size_t br)
{
    m_readPtr += br;
    if (m_readPtr >= m_endPtr)
    {
        size_t tmp = m_readPtr - m_endPtr;
        m_readPtr = _in_buffer + tmp;
    }
}

uint8_t *AudioBuffer::getWritePtr()
{
    return m_writePtr;
}

uint8_t *AudioBuffer::getReadPtr()
{
    size_t len = m_endPtr - m_readPtr;
    if (len < IN_BUFF_CHUNK_SIZE)
    {                                                           // be sure the last frame is completed
        memcpy(m_endPtr, _in_buffer, IN_BUFF_CHUNK_SIZE - len); // cpy from _in_buffer to m_endPtr with len
    }
    return m_readPtr;
}

void AudioBuffer::resetInputBuffer()
{
    m_writePtr = _in_buffer;
    m_readPtr = _in_buffer;
    m_endPtr = _in_buffer + _in_buff_size;
    m_f_start = true;
}

uint32_t AudioBuffer::getWritePos()
{
    return m_writePtr - _in_buffer;
}

uint32_t AudioBuffer::getReadPos()
{
    return m_readPtr - _in_buffer;
}

int16_t AudioBuffer::getSample(uint16_t sample)
{
    return _out_buffer[sample];
}

int16_t *AudioBuffer::getOutBuffer()
{
    return _out_buffer;
}

void AudioBuffer::clearOutputBuffer()
{
    if (_out_buffer)
        memset(_out_buffer, 0, OUT_BUFF_SIZE);
}