#include "ExportScreen.h"
#include "screen/resources/string.h"

ExportScreen::ExportScreen(TFT_eSPI &tft): IScreen{tft}
{
    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    layout->addWidget(_creator.getNavbar(1, "", "", STR_BACK));

    Label *title = new Label(2, _tft);
    title->setText(STR_EXPORT_SCREEN_TITLE);
    title->setWidth(_tft.width());
    title->setHeight(15);
    title->setGravity(IWidget::GRAVITY_CENTER);
    title->setAlign(IWidget::ALIGN_CENTER);
    title->setBackColor(COLOR_MAIN_BACK);
    layout->addWidget(title);

    _server_ip_txt = new Label(3, _tft);
    _server_ip_txt->setPos(0, 15);
    _server_ip_txt->setWidth(_tft.width());
    _server_ip_txt->setHeight(_tft.height() - NAVBAR_HEIGHT - 15);
    _server_ip_txt->setGravity(IWidget::GRAVITY_CENTER);
    _server_ip_txt->setAlign(IWidget::ALIGN_CENTER);
    _server_ip_txt->setBackColor(COLOR_MAIN_BACK);
    layout->addWidget(_server_ip_txt);

    if (!_server.start(true))
        _server_ip_txt->setText(STR_OFFLINE);
    else
    {
        _server_ip_txt->setText(STR_PREPARE);
        _is_started = true;
    }
}

void ExportScreen::rightClickedHandler()
{
    _server.stop();
    openScreenByID(ID_SCREEN_MENU);
}

void ExportScreen::doGUIWork()
{
    if (_is_started)
    {
        if (_server.isRunning())
        {
            _server_ip_txt->setText(_server.getIP().toString());
            _is_started = false;
        }
    }
}

#pragma region EMPTY

void ExportScreen::loop()
{
}

void ExportScreen::upClickedHandler()
{
}

void ExportScreen::downClickedHandler()
{
}

void ExportScreen::leftClickedHandler()
{
}

void ExportScreen::okClickedHandler()
{
}

void ExportScreen::okPressedHandler()
{
}

void ExportScreen::upPressedHandler()
{
}

void ExportScreen::downPressedHandler()
{
}

void ExportScreen::leftPressedHandler()
{
}

void ExportScreen::rightPressedHandler()
{
}

#pragma endregion EMPTY
