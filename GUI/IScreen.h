#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

#include "./widget/layout/IWidgetContainer.h"
#include "input/Input.h"

class IScreen
{
public:
    // Ідентифікатори вікон.
    // Відредагуй так, як тобі потрібно.
    // -------------------------
    enum ScreenID : uint8_t  //
    {                         //
        ID_SCREEN_HOME = 0,  //
        ID_SCREEN_MENU, //
        ID_SCREEN_FLASH,     //
        ID_SCREEN_MUSIC,     //
        ID_SCREEN_FILES,     //
        ID_SCREEN_WATCH,     //
        ID_SCREEN_PREF,      //
        ID_SCREEN_IMPORT,    //
        ID_SCREEN_EXPORT,    //
        ID_SCREEN_UPDATE,    //
    };                        //
    // -------------------------

#pragma region "don't touch this"
    IScreen(TFT_eSPI &tft);
    virtual ~IScreen() = 0;

    // Відобразити поточний екран
    void show();
    void tick();

    IScreen(const IScreen &rhs) = delete;
    IScreen &operator=(const IScreen &rhs) = delete;

    ScreenID getNextScreenID() { return _next_screen_ID; }
    bool isReleased() { return _is_released; }
#pragma endregion "don't touch this"

protected:
    // Глобальні константи, що повинні бути доступні для декількох вікон.
    // ---------------------------------
    const uint8_t SCROLLBAR_WIDTH{5}; //
    const uint8_t NAVBAR_HEIGHT{18};  //
    // ---------------------------------

    // Методи, які будуть виконуватися на екрані при натисканні кнопок.
    // Можна повністю замінити на свої.
    // ---------------------------------------
    virtual void okClickedHandler() = 0;    //
    virtual void okPressedHandler() = 0;    //
    virtual void upClickedHandler() = 0;    //
    virtual void upPressedHandler() = 0;    //
    virtual void downClickedHandler() = 0;  //
    virtual void downPressedHandler() = 0;  //
    virtual void leftClickedHandler() = 0;  //
    virtual void leftPressedHandler() = 0;  //
    virtual void rightClickedHandler() = 0; //
    virtual void rightPressedHandler() = 0; //
    // ---------------------------------------

#pragma region "don't touch this"
    virtual void doGUIWork() = 0; // Фонова робота, повязана з відрисовкою GUI. Наприклад, оновлення значення годинника, заряду акумулятора, тощо.
    virtual void loop() = 0;      // Код, який повинен виконуватися без затримок. Наприклад, відтворення аудіофайлів.
    //
    ScreenID _next_screen_ID{(ScreenID)0};
    TFT_eSPI &_tft;
    //
    void setLayout(IWidgetContainer *layout);
    IWidgetContainer *getLayout() const { return _layout; }
    void openScreenByID(ScreenID screen_ID);

private:
    Input _input;
    const uint8_t DELAY_BTW_GUI_UPDATE{30}; // затримка між "фреймами"
    unsigned long _last_upd_time{0};
    IWidgetContainer *_layout;
    bool _is_released{false};
#pragma endregion "don't touch this"
};