#pragma once
#include <Arduino.h>
#include "../IWidget.h"

class ProgressBar : public IWidget
{

    // TODO Додати вертикальну орієнтацію.

public:
    ProgressBar(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~ProgressBar();
    virtual void onDraw();
    virtual ProgressBar *clone(uint16_t id) const override;

    /*!
     * @brief
     *       Задати максимальне значення, яке може бути встановлено в елементі.
     * @param  max
     *       Максимальне значення.
     */
    void setMax(uint16_t max);

    /*!
     * @brief
     *       Задати поточне значення прогресу.
     * @param  progress
     *       Поточне значення прогресу.
     */
    void setProgress(uint16_t progress);

    /*!
     * @brief
     *       Задати колір полоси прогресу.
     * @param  color
     *       Колір полоси прогресу.
     */
    void setProgressColor(uint16_t color)
    {
        _progress_color = color;
        _is_changed = true;
    }

    /*!
     * @brief
     *       Скинути значення прогресу до 1.
     */
    void reset();

    uint16_t getProgress() const { return _progress; }
    uint16_t getMax() { return _max; }

private:
    uint16_t _progress{1};
    uint16_t _max{1};
    uint16_t _progress_color{0xFFFF};

    // opt
    bool _is_first_draw{true};
    uint16_t _prev_progress{1};
};
