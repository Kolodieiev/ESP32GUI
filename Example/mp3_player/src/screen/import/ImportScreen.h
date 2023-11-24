#pragma once
#include <Arduino.h>

#include "lib/server/FileServer.h"

#include "screen/WidgetCreator.h"
#include "lib/GUI/IScreen.h"
#include "lib/GUI/widget/text/Label.h"

class ImportScreen : public IScreen
{

public:
    ImportScreen(TFT_eSPI &tft);
    virtual ~ImportScreen() {}

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
    WidgetCreator _creator{_tft};
    FileServer _server;

    Label *_server_ip_txt;

    bool _is_started{true};
};
