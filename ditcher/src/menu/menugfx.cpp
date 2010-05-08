#include "menu/menugfx.hpp"
#include "global.hpp"
#include "settings.hpp"
#include "gfx.hpp"
#include "game.hpp"

    /* *** MenuGfx *** */

/**
Switches fullscreen / windowed video mode.
*/
class MenuGfx::FScreenActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        gfx.fullscreen=!gfx.fullscreen;
        gfx.setVideoMode();
        userface.refreshBkg();
    }
};

/**
Changes graphics resolution.
*/
class MenuGfx::DropResActionListener: public ActionListener{
    void action(const ActionEvent& actionEvent) {
        Point newres = gfx.res;
        switch (((DropDown*)(actionEvent.getSource()))->getSelected()){
            case 0: newres.set(640, 480); break;
            case 1: newres.set(800, 600); break;
            case 2: newres.set(1024, 768); break;
            case 3: newres.set(1280, 800); break;
        }
        if (newres != gfx.res){
            Point oldres = gfx.res;
            gfx.res = newres;
            if (!gfx.setVideoMode()){
                gfx.res = oldres;
                userface.mGfx->checkFullScreen->setSelected(gfx.fullscreen);
            }
            userface.refreshBkg();
            userface.setContainer();
        }
    }
};

/**
Contains resolution options.
*/
class MenuGfx::ResolutionListModel: public gcn::ListModel{
    public:
    std::string getElementAt(int i){
        switch (i){
            case 0: return "640x480"; break;
            case 1: return "800x600"; break;
            case 2: return "1024x768"; break;
            case 3: return "1280x800"; break;
            default: return ""; break;
        }
    }
    int getNumberOfElements(){ return 4; }
};

void MenuGfx::onEnter(){
    int ressel = 1;
    switch(settings.gfxres.x){
        case 640:  ressel = 0; break;
        case 800:  ressel = 1; break;
        case 1024: ressel = 2; break;
        case 1280: ressel = 3; break;
    }
    dropRes->setSelected(ressel);
    checkFullScreen->setSelected(gfx.fullscreen);
}

void MenuGfx::onLeave(){
    settings.gfxres = gfx.res;
    settings.fullscreen = gfx.fullscreen;
    settings.writeSettings();
}

/**
Puts all widgets into the menu.
*/
MenuGfx::MenuGfx(){

    setDimension(Rectangle(MENUWIDTH / 3, MENUHEIGHT / 4, MENUWIDTH / 3, MENUHEIGHT / 2));

    Label* labelRes = new Label("Resolution:");
    labelRes->setPosition(getWidth() / 2 - labelRes->getWidth() / 2, getHeight() * 2 / 9);
    add(labelRes);

    dropRes = new DropDown(new ResolutionListModel());
    dropRes->addActionListener(new DropResActionListener());
    dropRes->setPosition(getWidth() / 2 - dropRes->getWidth() / 2,
        getHeight() * 3 / 9 - dropRes->getHeight() / 2);
    add(dropRes);

    checkFullScreen = new CheckBox("Fullscreen");
    checkFullScreen->setPosition(getWidth() / 2 - checkFullScreen->getWidth() / 2,
        getHeight() * 5 / 9 - checkFullScreen->getHeight() / 2);
    checkFullScreen->addActionListener(new FScreenActionListener());
    add(checkFullScreen);

    Button* buttonBack = new Button("Back");
    buttonBack->setPosition(getWidth() / 2 - buttonBack->getWidth() / 2,
        getHeight() * 7 / 9 - buttonBack->getHeight() / 2);
    buttonBack->addActionListener(new BackActionListener());
    add(buttonBack);

    ((CustomContainer*)this)->initWidgets();
}

