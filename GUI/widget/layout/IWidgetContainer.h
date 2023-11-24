#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include <vector>

class IWidgetContainer : public IWidget
{

public:
    IWidgetContainer(uint16_t widget_ID, TFT_eSPI &tft);
    virtual ~IWidgetContainer();

    /*!
     * @brief
     *       Додати новий елемент до контейнера.
     *       Ідентифікатор нового елемента повинен бути унікальним для цього контейнера.
     * @param widget_ptr
     *       Вказівник на новий елемент.
     * @return
     *        true в разі успіху операції. Інакше false.
     */
    virtual bool addWidget(IWidget *widget_ptr);

    /*!
     * @brief
     *       Видалити елемент по його ідентифікатору з контейнера.
     * @param widget_ID
     *       Ідентифікатор елемента.
     * @return
     *        true в разі успіху операції. Інакше false.
     */
    virtual bool deleteWidgetByID(uint16_t widget_ID);

    /*!
     * @brief
     *       Знайти елемент по його ідентифікатору в контейнері.
     * @param widget_ID
     *       Ідентифікатор елемента.
     * @return
     *        Вказівник на елемент в разі успіху. Інакше nullptr.
     */
    virtual IWidget *findWidgetByID(uint16_t widget_ID) const;

    /*!
     * @brief
     *       Знайти елемент по його порядковому номері в контейнері.
     * @param widget_pos
     *       Порядковий номер елемента.
     * @return
     *       Вказівник на елемент в разі успіху. Інакше nullptr.
     */
    virtual IWidget *getWidget(uint16_t widget_pos) const;

    /*!
     * @brief
     *       Видалити усі елементи з контейнера та очистити пам'ять, яку вони займали.
     */
    virtual void deleteWidgets();

    /*!
     * @return
     *        Кількість елементів в контейнері.
     */
    uint16_t getSize() const;

protected:
    std::vector<IWidget *> _widgets;
};