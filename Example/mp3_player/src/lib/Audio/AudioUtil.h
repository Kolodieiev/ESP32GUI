#pragma once
#pragma GCC optimize("Ofast")
#include <Arduino.h>

class AudioUtil
{
public:
    void strlower(char *str);
    void trim(char *s);
    bool startsWith(const char *base, const char *str);
    bool endsWith(const char *base, const char *str);
    int indexOf(const char *base, const char *str, int startIndex = 0);
    int indexOf(const char *base, char ch, int startIndex = 0);
    int lastIndexOf(const char *haystack, const char *needle);
    int lastIndexOf(const char *haystack, const char needle);
    int specialIndexOf(const uint8_t *base, const char *str, int baselen, bool exact = false);
    size_t bigEndian(const uint8_t *base, uint8_t numBytes, uint8_t shiftLeft = 8);
};