#pragma once
#include <Arduino.h>
#include "../text/Focusable.h"

class SpinBox : public Focusable
{

public:
    SpinBox(uint16_t widget_id, TFT_eSPI &tft);
    virtual ~SpinBox() {}
    virtual SpinBox *clone(uint16_t id) const override;

    /*!
     * @brief
     *       Збільшити значення, що зберігається в елементі на 1, якщо можливо.
     *
     */
    void up();

    /*!
     * @brief
     *       Зменшити значення, що зберігається в елементі на 1, якщо можливо.
     *
     */
    void down();

    /*!
     * @brief
     *       Задати мінімальне значення, яке може бути встановлено в елементі.
     * @param  min
     *       Мінімальне значення.
     */
    void setMin(const int32_t min);

    /*!
     * @brief
     *       Задати максимальне значення, яке може бути встановлено в елементі.
     * @param  max
     *       Максимальне значення.
     */
    void setMax(const int32_t max);

    /*!
     * @brief
     *       Задати поточне значення для елементу.
     * @param  value
     *       Поточне значення.
     */
    void setValue(const int32_t value);

    int32_t getMin() const { return _min; }
    int32_t getMax() const { return _max; }
    int32_t getValue() const { return _value; }

private:
    using Focusable::isTicker;
    using Focusable::setIsTicker;

    int32_t _min{0};
    int32_t _max{0};

    int32_t _value;
};
