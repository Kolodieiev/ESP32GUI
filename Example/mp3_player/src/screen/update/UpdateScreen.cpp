#include "UpdateScreen.h"
#include "SD.h"
#include "Update.h"
#include "screen/resources/string.h"

UpdateScreen::UpdateScreen(TFT_eSPI &tft): IScreen{tft}
{
    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);
    layout->addWidget(_creator.getNavbar(1, STR_UPDATE, "", STR_BACK));

    Label *title = new Label(2, _tft);
    title->setText(STR_UPDATE_SCREEN_TITLE);
    title->setWidth(_tft.width());
    title->setHeight(15);
    title->setGravity(IWidget::GRAVITY_CENTER);
    title->setAlign(IWidget::ALIGN_CENTER);
    title->setBackColor(COLOR_MAIN_BACK);
    layout->addWidget(title);

    Label *author = title->clone(3);
    author->setText(STR_AUTHOR);
    author->setFontID(4);
    author->setHeight(9);
    author->setPos(0, (_tft.height() - NAVBAR_HEIGHT - 15) / 2);
    layout->addWidget(author);
}

void UpdateScreen::showUpdating()
{
    EmptyLayout *layout = _creator.getEmptyLayout(1);
    setLayout(layout);

    Label *updating_msg = new Label(1, _tft);
    updating_msg->setText(STR_UPDATING);
    updating_msg->setWidth(_tft.width());
    updating_msg->setHeight(_tft.height() / 2);
    updating_msg->setGravity(IWidget::GRAVITY_CENTER);
    updating_msg->setAlign(IWidget::ALIGN_CENTER);
    updating_msg->setBackColor(COLOR_MAIN_BACK);
    layout->addWidget(updating_msg);
    layout->onDraw();
}

void UpdateScreen::leftClickedHandler()
{
    if (SD.begin(SD_CS))
    {
        File firmware = SD.open("/firmware.bin");

        if (firmware && !firmware.isDirectory())
        {
            showUpdating();

            Update.begin(firmware.size(), U_FLASH);
            Update.writeStream(firmware);
            Update.end();

            firmware.close();
            ESP.restart();
        }
    }
    else
        log_e("SD not mounted");
}

void UpdateScreen::rightClickedHandler()
{
    openScreenByID(ID_SCREEN_MENU);
}

#pragma region EMPTY

void UpdateScreen::doGUIWork()
{
}

void UpdateScreen::loop()
{
}

void UpdateScreen::upClickedHandler()
{
}

void UpdateScreen::downClickedHandler()
{
}

void UpdateScreen::okClickedHandler()
{
}

void UpdateScreen::okPressedHandler()
{
}

void UpdateScreen::upPressedHandler()
{
}

void UpdateScreen::downPressedHandler()
{
}

void UpdateScreen::leftPressedHandler()
{
}

void UpdateScreen::rightPressedHandler()
{
}

#pragma endregion EMPTY
