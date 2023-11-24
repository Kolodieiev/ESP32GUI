#include "AudioUtil.h"

void AudioUtil::strlower(char *str)
{
    unsigned char *p = (unsigned char *)str;
    while (*p)
    {
        *p = tolower((unsigned char)*p);
        p++;
    }
}

void AudioUtil::trim(char *s)
{
    // fb   trim in place
    char *pe;
    char *p = s;
    while (isspace(*p))
        p++; // left
    pe = p;  // right
    while (*pe != '\0')
        pe++;
    do
    {
        pe--;
    } while ((pe > p) && isspace(*pe));
    if (p == s)
    {
        *++pe = '\0';
    }
    else
    { // move
        while (p <= pe)
            *s++ = *p++;
        *s = '\0';
    }
}

bool AudioUtil::startsWith(const char *base, const char *str)
{
    // fb
    char c;
    while ((c = *str++) != '\0')
        if (c != *base++)
            return false;
    return true;
}

bool AudioUtil::endsWith(const char *base, const char *str)
{
    // fb
    int slen = strlen(str) - 1;
    const char *p = base + strlen(base) - 1;
    while (p > base && isspace(*p))
        p--; // rtrim
    p -= slen;
    if (p < base)
        return false;
    return (strncmp(p, str, slen) == 0);
}

int AudioUtil::indexOf(const char *base, const char *str, int startIndex)
{
    // fb
    const char *p = base;
    for (; startIndex > 0; startIndex--)
        if (*p++ == '\0')
            return -1;
    char *pos = strstr(p, str);
    if (pos == nullptr)
        return -1;
    return pos - base;
}

int AudioUtil::indexOf(const char *base, char ch, int startIndex)
{
    // fb
    const char *p = base;
    for (; startIndex > 0; startIndex--)
        if (*p++ == '\0')
            return -1;
    char *pos = strchr(p, ch);
    if (pos == nullptr)
        return -1;
    return pos - base;
}

int AudioUtil::lastIndexOf(const char *haystack, const char *needle)
{
    // fb
    int nlen = strlen(needle);
    if (nlen == 0)
        return -1;
    const char *p = haystack - nlen + strlen(haystack);
    while (p >= haystack)
    {
        int i = 0;
        while (needle[i] == p[i])
            if (++i == nlen)
                return p - haystack;
        p--;
    }
    return -1;
}

int AudioUtil::lastIndexOf(const char *haystack, const char needle)
{
    // fb
    const char *p = strrchr(haystack, needle);
    return (p ? p - haystack : -1);
}

int AudioUtil::specialIndexOf(const uint8_t *base, const char *str, int baselen, bool exact)
{
    int result; // seek for str in buffer or in header up to baselen, not nullterninated
    if (strlen(str) > baselen)
        return -1; // if exact == true seekstr in buffer must have "\0" at the end
    for (int i = 0; i < baselen - strlen(str); ++i)
    {
        result = i;
        for (int j = 0; j < strlen(str) + exact; ++j)
        {
            if (*(base + i + j) != *(str + j))
            {
                result = -1;
                break;
            }
        }
        if (result >= 0)
            break;
    }
    return result;
}

size_t AudioUtil::bigEndian(const uint8_t *base, uint8_t numBytes, uint8_t shiftLeft)
{
    uint64_t result = 0;
    if (numBytes < 1 || numBytes > 8)
        return 0;
    for (int i = 0; i < numBytes; ++i)
    {
        result += *(base + i) << (numBytes - i - 1) * shiftLeft;
    }
    if (result > SIZE_MAX)
    {
        result = 0;
    } // overflow
    return (size_t)result;
}
