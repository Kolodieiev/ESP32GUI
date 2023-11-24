#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include <vector>

class IItemsLoader
{
public:
    IItemsLoader(){};
    virtual ~IItemsLoader(){};

    virtual bool loadNext(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID) = 0;
    virtual bool loadPrev(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID) = 0;
};
