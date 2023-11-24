#include <pgmspace.h>
#include "screen/resources/color.h"
#include "screen/resources/string.h"
#include "FilesScreen.h"
#include "lib/GUI/widget/text/Label.h"
#include "lib/GUI/widget/keyboard/KeyboardRow.h"

FilesScreen::FilesScreen(TFT_eSPI &tft) : IScreen{tft}
{
    showFirst();
}

void FilesScreen::showBadSDConnect()
{
    _mode = MODE_BAD_CONNECT;
    getLayout()->deleteWidgets();
    getLayout()->addWidget(_creator.getNavbar(ID_NAVBAR, "", "", STR_BACK));
}

void FilesScreen::showFirst()
{
    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    // navbar
    layout->addWidget(_creator.getNavbar(ID_NAVBAR, STR_SELECT, "", STR_BACK));

    // menu
    _files_menu = _creator.getDynamicMenu(ID_DYNAMIC_MENU, this);
    layout->addWidget(_files_menu);

    // menu item
    Focusable *template_item = _creator.getFocusableItem(1, "");

    String root_path{"/"};
    // fill menu
    std::vector<IWidget *> items;
    if (loadMenuFileItems(items, root_path, 0, _files_menu->getItemsNumberOnScreen(), template_item))
    {
        for (uint16_t i{0}; i < items.size(); ++i)
            _files_menu->addWidget(items[i]);
    }
    else if (updateDir(root_path))
        showDirUpdating();
    else
        showBadSDConnect();
}

void FilesScreen::showDirUpdating()
{
    _mode = MODE_UPDATE_DIR;

    getLayout()->deleteWidgets();
    getLayout()->addWidget(_creator.getNavbar(ID_NAVBAR, "", "", STR_CANCEL));

    Label *cancel_txt = new Label(2, _tft);
    cancel_txt->setText(STR_UPDATING);
    cancel_txt->setAlign(IWidget::ALIGN_CENTER);
    cancel_txt->setGravity(IWidget::GRAVITY_CENTER);
    cancel_txt->setWidth(_tft.width());
    cancel_txt->setHeight(_tft.height() - NAVBAR_HEIGHT);
    getLayout()->addWidget(cancel_txt);
}

void FilesScreen::doGUIWork()
{
    if (_mode == MODE_UPDATE_DIR)
    {
        if (isUpdateDirTaskComplete())
        {
            getLayout()->deleteWidgets();
            getLayout()->addWidget(_creator.getNavbar(ID_NAVBAR, STR_SELECT, "", STR_BACK));

            _files_menu = _creator.getDynamicMenu(ID_DYNAMIC_MENU, this);
            getLayout()->addWidget(_files_menu);

            String path = getPathFromBreadcrumbs();

            Focusable *update_item = _creator.getFocusableItem(1, "");

            std::vector<IWidget *> items;
            if (loadMenuFileItems(items, path, 0, _files_menu->getItemsNumberOnScreen(), update_item))
            {
                _mode = MODE_DIR_NAVIGATION;

                for (uint16_t i{0}; i < items.size(); ++i)
                    _files_menu->addWidget(items[i]);
            }
            else
                showBadSDConnect();
        }
    }
}

bool FilesScreen::loadNext(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID)
{
    if (!current_ID)
        return false;

    String cur_path = getPathFromBreadcrumbs();

    Focusable *template_item = _creator.getFocusableItem(1, "");

    return loadMenuFileItems(widgets, cur_path, current_ID, size, template_item);
}

bool FilesScreen::loadPrev(std::vector<IWidget *> &widgets, uint8_t size, uint16_t current_ID)
{
    // Не отримали ID
    if (!current_ID)
        return false;

    uint16_t item_pos = current_ID - 1;

    // Вже перший елемент
    if (!item_pos)
        return false;

    // Вирахувати першу позицію звідки потрібно читати файл.
    if (current_ID > size)
        item_pos = current_ID - size - 1;
    else
        item_pos = 0;

    String cur_path = getPathFromBreadcrumbs();

    Focusable *template_item = _creator.getFocusableItem(1, "");

    return loadMenuFileItems(widgets, cur_path, item_pos, size, template_item);
}

void FilesScreen::showNewDirDialog()
{
    _mode = MODE_NEW_DIR_DIALOG;
    getLayout()->deleteWidgets();

    _dir_name_edit = new TextBox(ID_DIR_EDIT, _tft);
    _dir_name_edit->setWidth(_tft.width());
    _dir_name_edit->setHeight(20);
    _dir_name_edit->setAlign(IWidget::ALIGN_CENTER);
    _dir_name_edit->setBackColor(COLOR_WHITE);
    _dir_name_edit->setTextColor(COLOR_BLACK);

    Focusable *q = _creator.getFocusableItem(ID_BTN_Q, "q", 4);
    q->setAlign(IWidget::ALIGN_CENTER);
    q->setTextOffset(0);
    q->setCornerRadius(1);

    Focusable *w = q->clone(ID_BTN_W);
    w->setText("w");

    Focusable *e = q->clone(ID_BTN_E);
    e->setText("e");

    Focusable *r = q->clone(ID_BTN_R);
    r->setText("r");

    Focusable *t = q->clone(ID_BTN_T);
    t->setText("t");

    Focusable *y = q->clone(ID_BTN_Y);
    y->setText("y");

    Focusable *u = q->clone(ID_BTN_U);
    u->setText("u");

    Focusable *i = q->clone(ID_BTN_I);
    i->setText("i");

    Focusable *o = q->clone(ID_BTN_O);
    o->setText("o");

    Focusable *p = q->clone(ID_BTN_P);
    p->setText("p");

    Focusable *a = q->clone(ID_BTN_A);
    a->setText("a");

    Focusable *s = q->clone(ID_BTN_S);
    s->setText("s");

    Focusable *d = q->clone(ID_BTN_D);
    d->setText("d");

    Focusable *f = q->clone(ID_BTN_F);
    f->setText("f");

    Focusable *g = q->clone(ID_BTN_G);
    g->setText("g");

    Focusable *h = q->clone(ID_BTN_H);
    h->setText("h");

    Focusable *j = q->clone(ID_BTN_J);
    j->setText("j");

    Focusable *k = q->clone(ID_BTN_K);
    k->setText("k");

    Focusable *l = q->clone(ID_BTN_L);
    l->setText("l");

    Focusable *z = q->clone(ID_BTN_Z);
    z->setText("z");

    Focusable *x = q->clone(ID_BTN_X);
    x->setText("x");

    Focusable *c = q->clone(ID_BTN_C);
    c->setText("c");

    Focusable *v = q->clone(ID_BTN_V);
    v->setText("v");

    Focusable *b = q->clone(ID_BTN_B);
    b->setText("b");

    Focusable *n = q->clone(ID_BTN_N);
    n->setText("n");

    Focusable *m = q->clone(ID_BTN_M);
    m->setText("m");

    KeyboardRow *row1 = new KeyboardRow(1, _tft);
    row1->setHeight(20);
    row1->setBtnsHeight(15);
    row1->setBtnsWidth(10);
    KeyboardRow *row2 = new KeyboardRow(2, _tft);
    row2->setHeight(20);
    row2->setBtnsHeight(15);
    row2->setBtnsWidth(10);
    KeyboardRow *row3 = new KeyboardRow(3, _tft);
    row3->setHeight(20);
    row3->setBtnsHeight(15);
    row3->setBtnsWidth(10);
    //
    row1->addWidget(q);
    row1->addWidget(w);
    row1->addWidget(e);
    row1->addWidget(r);
    row1->addWidget(t);
    row1->addWidget(y);
    row1->addWidget(u);
    row1->addWidget(i);
    row1->addWidget(o);
    row1->addWidget(p);
    //
    row2->addWidget(a);
    row2->addWidget(s);
    row2->addWidget(d);
    row2->addWidget(f);
    row2->addWidget(g);
    row2->addWidget(h);
    row2->addWidget(j);
    row2->addWidget(k);
    row2->addWidget(l);
    //
    row3->addWidget(z);
    row3->addWidget(x);
    row3->addWidget(c);
    row3->addWidget(v);
    row3->addWidget(b);
    row3->addWidget(n);
    row3->addWidget(m);
    //

    _keyboard = new Keyboard(ID_KEYBOARD, _tft);
    _keyboard->setWidth(_tft.width());
    _keyboard->setHeight(_tft.height() - 22);
    _keyboard->setPos(0, 22);

    _keyboard->addWidget(row1);
    _keyboard->addWidget(row2);
    _keyboard->addWidget(row3);

    getLayout()->addWidget(_dir_name_edit);
    getLayout()->addWidget(_keyboard);
}

void FilesScreen::closeNewDirDialog()
{
    _mode = MODE_DIR_NAVIGATION;

    getLayout()->deleteWidgets();

    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    // navbar
    layout->addWidget(_creator.getNavbar(ID_NAVBAR, STR_SELECT, "", STR_BACK));

    // menu
    _files_menu = _creator.getDynamicMenu(ID_DYNAMIC_MENU, this);
    layout->addWidget(_files_menu);

    String cur_dir = getPathFromBreadcrumbs();
    //
    fillMenuForPath(cur_dir);
}

void FilesScreen::keyboardEventHandler()
{
    // uint16_t id = _keyboard->getCurrentBtnID();
    // if (id ==)
    // {
    // }

    // Можна оброблювати по ID кнопки, але в даному випадку зручніше оброблювати текст,
    // Тому що ніякі керуючі кнопки не використовуються.

    String dir_name_str = _dir_name_edit->getText();
    dir_name_str += _keyboard->getCurrentBtnTxt();
    _dir_name_edit->setText(dir_name_str);
}

void FilesScreen::createNewDir()
{
    String cur_dir = getPathFromBreadcrumbs();
    String dir_name = _dir_name_edit->getText();

    if (dir_name != "")
    {
        dir_name = cur_dir + "/" + dir_name;

        if (createDir(dir_name.c_str()))
        {
            // Видалити список файлів для оновлення і закрити діалог
            String rem_file_list = cur_dir + "/" + FILELIST_NAME;
            remove(rem_file_list.c_str());
        }
    }

    closeNewDirDialog();
}

void FilesScreen::okClickedHandler()
{
    if (_mode == MODE_DIR_NAVIGATION)
        loadNextDir();
    else if (_mode == MODE_CONTEXT_MENU)
    {
        uint16_t id = _context_menu->getCurrentItemID();

        if (id == ID_ITEM_UPDATE)
        {
            String cur_dir = getPathFromBreadcrumbs();

            if (updateDir(cur_dir))
            {
                closeContext();
                showDirUpdating();
            }
        }
        else if (id == ID_ITEM_REMOVE)
        {
            String path = getPathFromBreadcrumbs();
            String file_name = path + "/" + _files_menu->getCurrentItemText().substring(4);
            if (remove(file_name.c_str()))
                if (updateDir(path))
                {
                    closeContext();
                    showDirUpdating();
                }
        }
        else if (id == ID_ITEM_MOVE)
            prepareFileMoving();
        else if (id == ID_ITEM_PASTE)
            pasteFile();
        else if (id == ID_ITEM_NEW_DIR)
            showNewDirDialog();
    }
    else if (_mode == MODE_NEW_DIR_DIALOG)
        keyboardEventHandler();
}

void FilesScreen::rightClickedHandler()
{
    if (_mode == MODE_DIR_NAVIGATION)
        loadPrevDir();
    else if (_mode == MODE_CONTEXT_MENU)
        closeContext();
    else if (_mode == MODE_UPDATE_DIR)
        cancelDirUpdating();
    else if (_mode == MODE_BAD_CONNECT)
        rightPressedHandler();
    else if (_mode == MODE_NEW_DIR_DIALOG)
        _keyboard->focusRight();
}

void FilesScreen::downClickedHandler()
{
    if (_mode == MODE_DIR_NAVIGATION)
        _files_menu->focusDown();
    else if (_mode == MODE_CONTEXT_MENU)
        _context_menu->focusDown();
    else if (_mode == MODE_NEW_DIR_DIALOG)
        _keyboard->focusDown();
}

void FilesScreen::upClickedHandler()
{
    if (_mode == MODE_DIR_NAVIGATION)
        _files_menu->focusUp();
    else if (_mode == MODE_CONTEXT_MENU)
        _context_menu->focusUp();
    else if (_mode == MODE_NEW_DIR_DIALOG)
        _keyboard->focusUp();
}

void FilesScreen::leftClickedHandler()
{
    if (_mode == MODE_NEW_DIR_DIALOG)
        _keyboard->focusLeft();
    else
        okClickedHandler();
}

void FilesScreen::showContext()
{
    _mode = MODE_CONTEXT_MENU;

    Focusable *update_item = _creator.getFocusableItem(ID_ITEM_UPDATE, STR_UPDATE, 4);
    update_item->setTextOffset(1);
    Focusable *new_dir_item = _creator.getFocusableItem(ID_ITEM_NEW_DIR, STR_NEW_DIR, 4);
    new_dir_item->setTextOffset(1);

    _context_menu = new FixedMenu(ID_FIXED_MENU, _tft);
    _context_menu->setItemsHeight(9);
    _context_menu->setWidth((float)_tft.width() / 2.3);

    _context_menu->setBackColor(COLOR_BLACK);
    _context_menu->setBorder(true);
    _context_menu->setBorderColor(COLOR_ORANGE);

    if (_has_moving_file)
    {
        // Якщо є файл для переміщення додати відповідний пункт меню
        Focusable *paste_item = _creator.getFocusableItem(ID_ITEM_PASTE, STR_PASTE, 4);
        paste_item->setTextOffset(1);
        _context_menu->addWidget(paste_item);
    }

    // оновити
    _context_menu->addWidget(update_item);

    // контекст для вибраного файлу
    String cur_item_str = _files_menu->getCurrentItemText();
    if (cur_item_str != "")
    {
        // перемістити
        Focusable *file_move_item = _creator.getFocusableItem(ID_ITEM_MOVE, STR_MOVE, 4);
        file_move_item->setTextOffset(1);
        _context_menu->addWidget(file_move_item);

        // видалити
        Focusable *file_remove_item = _creator.getFocusableItem(ID_ITEM_REMOVE, STR_ITEM_REMOVE, 4);
        file_remove_item->setTextOffset(1);
        _context_menu->addWidget(file_remove_item);
    }

    // новий каталог
    _context_menu->addWidget(new_dir_item);
    _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemsHeight() + 2);
    _context_menu->setPos(_tft.width() - _context_menu->getWidth() - 1,
                          _tft.height() - NAVBAR_HEIGHT - _context_menu->getHeight() - 2);

    getLayout()->addWidget(_context_menu);
}

void FilesScreen::closeContext()
{
    _mode = MODE_DIR_NAVIGATION;
    getLayout()->deleteWidgetByID(ID_FIXED_MENU);
}

void FilesScreen::prepareFileMoving()
{
    _path_move_from = getPathFromBreadcrumbs();
    _file_name_to_move = "/" + _files_menu->getCurrentItemText().substring(4);
    _has_moving_file = true;

    closeContext();
}

void FilesScreen::pasteFile()
{
    String old_file_name = _path_move_from + _file_name_to_move;

    if (fileExist(old_file_name.c_str()) || dirExist(old_file_name.c_str()))
    {
        String rem_file_list = _path_move_from + "/" + FILELIST_NAME;
        remove(rem_file_list.c_str());
        //
        String cur_dir = getPathFromBreadcrumbs();
        String new_file_name = cur_dir + _file_name_to_move;

        if (rename(old_file_name.c_str(), new_file_name.c_str()))
            if (updateDir(cur_dir))
            {
                closeContext();
                showDirUpdating();
            }
    }

    _has_moving_file = false;
    _path_move_from = "";
    _file_name_to_move = "";
}

void FilesScreen::loadNextDir()
{
    if (_files_menu->getCurrentItemID() == 0)
        return;

    String clear_str = _files_menu->getCurrentItemText().substring(4);

    String next_dir = getPathFromBreadcrumbs() + "/" + clear_str;

    if (dirExist(next_dir.c_str()))
    {
        _breadcrumbs.push_back(clear_str);
        fillMenuForPath(next_dir);
    }
}

void FilesScreen::loadPrevDir()
{
    if (_breadcrumbs.size() > 0)
    {
        _breadcrumbs.pop_back();
        String path = getPathFromBreadcrumbs();
        fillMenuForPath(path);
    }
    else
        rightPressedHandler();
}

void FilesScreen::fillMenuForPath(String &path)
{
    std::vector<IWidget *> items;
    Focusable *template_item = _creator.getFocusableItem(1, "");

    if (loadMenuFileItems(items, path, 0, _files_menu->getItemsNumberOnScreen(), template_item))
    {
        _files_menu->deleteWidgets();

        for (uint16_t i{0}; i < items.size(); ++i)
            _files_menu->addWidget(items[i]);
    }
    else if (updateDir(path))
        showDirUpdating();
    else
        showBadSDConnect();
}

void FilesScreen::okPressedHandler()
{
    if (_mode == MODE_DIR_NAVIGATION)
        showContext();
    else if (_mode == MODE_NEW_DIR_DIALOG)
        createNewDir();
}

void FilesScreen::rightPressedHandler()
{
    if (_mode == MODE_NEW_DIR_DIALOG)
        closeNewDirDialog();
    else
        openScreenByID(ID_SCREEN_MENU);
}

void FilesScreen::leftPressedHandler()
{
    if (_mode == MODE_NEW_DIR_DIALOG)
    {
        _dir_name_edit->removeLastChar();
    }
}

#pragma region EMPTY

void FilesScreen::loop()
{
}

void FilesScreen::upPressedHandler()
{
}

void FilesScreen::downPressedHandler()
{
}

#pragma endregion EMPTY
