#pragma once
#include <Arduino.h>
#include "IWidgetContainer.h"

class EmptyLayout : public IWidgetContainer
{
public:
  EmptyLayout(uint16_t widget_ID, TFT_eSPI &tft);
  virtual ~EmptyLayout();
  virtual void onDraw();
  EmptyLayout *clone(uint16_t id) const;
};