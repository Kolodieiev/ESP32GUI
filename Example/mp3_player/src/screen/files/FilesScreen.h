#pragma once
#include <Arduino.h>
#include "FileManager.h"

#include "screen/WidgetCreator.h"
#include "lib/GUI/IScreen.h"
#include "lib/GUI/widget/scrollbar/ScrollBar.h"
#include "lib/GUI/widget/menu/FixedMenu.h"
#include "lib/GUI/widget/keyboard/Keyboard.h"
#include "lib/GUI/widget/text/TextBox.h"

class FilesScreen : public IScreen, public IItemsLoader, public FileManager
{
public:
    FilesScreen(TFT_eSPI &tft);
    virtual ~FilesScreen(){};
    virtual bool loadNext(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID);
    virtual bool loadPrev(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID);

protected:
    virtual void loop() override;
    virtual void doGUIWork() override;
    //
    virtual void upClickedHandler() override;
    virtual void downClickedHandler() override;
    virtual void leftClickedHandler() override;
    virtual void rightClickedHandler() override;
    virtual void okClickedHandler() override;
    virtual void okPressedHandler() override;
    virtual void upPressedHandler() override;
    virtual void downPressedHandler() override;
    virtual void leftPressedHandler() override;
    virtual void rightPressedHandler() override;

private:
    const uint16_t ID_NAVBAR{1};
    const uint16_t ID_DYNAMIC_MENU{2};
    const uint16_t ID_FIXED_MENU{3};
    const uint16_t ID_FIXED_MENU_SCROLL{4};
    const uint16_t ID_KEYBOARD{5};
    const uint16_t ID_DIR_EDIT{6};

    const uint16_t ID_ITEM_UPDATE{1};
    const uint16_t ID_ITEM_PASTE{2};
    const uint16_t ID_ITEM_MOVE{3};
    const uint16_t ID_ITEM_REMOVE{4};
    const uint16_t ID_ITEM_NEW_DIR{5};
    // btns
    const uint16_t ID_BTN_Q{1};
    const uint16_t ID_BTN_W{2};
    const uint16_t ID_BTN_E{3};
    const uint16_t ID_BTN_R{4};
    const uint16_t ID_BTN_T{5};
    const uint16_t ID_BTN_Y{6};
    const uint16_t ID_BTN_U{7};
    const uint16_t ID_BTN_I{8};
    const uint16_t ID_BTN_O{9};
    const uint16_t ID_BTN_P{10};
    const uint16_t ID_BTN_A{11};
    const uint16_t ID_BTN_S{12};
    const uint16_t ID_BTN_D{13};
    const uint16_t ID_BTN_F{14};
    const uint16_t ID_BTN_G{15};
    const uint16_t ID_BTN_H{16};
    const uint16_t ID_BTN_J{17};
    const uint16_t ID_BTN_K{18};
    const uint16_t ID_BTN_L{19};
    const uint16_t ID_BTN_Z{20};
    const uint16_t ID_BTN_X{21};
    const uint16_t ID_BTN_C{22};
    const uint16_t ID_BTN_V{23};
    const uint16_t ID_BTN_B{24};
    const uint16_t ID_BTN_N{25};
    const uint16_t ID_BTN_M{26};
    //
    enum Mode
    {
        MODE_DIR_NAVIGATION,
        MODE_CONTEXT_MENU,
        MODE_COPY_FILE,
        MODE_NEW_DIR_DIALOG,
        MODE_UPDATE_DIR,
        MODE_BAD_CONNECT
    };

    WidgetCreator _creator{_tft};

    Mode _mode{MODE_DIR_NAVIGATION};

    FixedMenu *_context_menu;
    ScrollBar *_context_scrollbar;
    DynamicMenu *_files_menu;
    //
    Keyboard *_keyboard;
    TextBox *_dir_name_edit;
    // Переміщення
    bool _has_moving_file{false};
    String _path_move_from;
    String _file_name_to_move;
    //
    void showFirst();
    void showDirUpdating();
    void showBadSDConnect();
    void showContext();

    void closeContext();
    //
    void keyboardEventHandler();
    void createNewDir();
    void showNewDirDialog();
    void closeNewDirDialog();
    //
    void prepareFileMoving();
    void pasteFile();

    void loadNextDir();
    void loadPrevDir();

    void fillMenuForPath(String &path);
};