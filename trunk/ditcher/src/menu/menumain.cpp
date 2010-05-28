#include "menu/menumain.hpp"
#include "global.hpp"
#include "network.hpp"

    /* *** MenuMain *** */

/**
Exit program.
*/
class MenuMain::QuitActionListener : public ActionListener
{
    void action(const ActionEvent& actionEvent) { userface.quit = true; }
};

/**
Creating local game.
*/
class MenuMain::MenuCreateGameActionListener : public ActionListener{
	void action(const ActionEvent& actionEvent) {
		userface.switchTo((Menu*)userface.mCreateGame);
	}
};

/**
Connecting to server.
*/
class MenuMain::MenuConnectActionListener : public ActionListener{
	void action(const ActionEvent& actionEvent) {
	    if (!network.inited){
            if (SDLNet_Init() < 0){
                userface.showMessage("Network initialization failed");
                return;
            }else network.inited = true;
	    }
        userface.switchTo((Menu*)userface.mConnect);
	}
};

/**
Graphics settings section.
*/
class MenuMain::MenuGfxActionListener : public ActionListener{
	void action(const ActionEvent& actionEvent) {
		userface.switchTo((Menu*)userface.mGfx);
	}
};

/**
Local players setting section.
*/
class MenuMain::MenuPlayersActionListener : public ActionListener{
	void action(const ActionEvent& actionEvent) {
		userface.switchTo((Menu*)userface.mPlayers);
	}
};

/**
Local players setting section.
*/
class MenuMain::MenuCreditsActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        userface.switchTo((Menu*)userface.mCredits);
    }
};

/**
Puts all widgets into the menu.
*/
MenuMain::MenuMain(){

    setDimension(Rectangle(MENUWIDTH / 3, MENUHEIGHT / 8, MENUWIDTH / 3, MENUHEIGHT * 3 / 4));

	button[0] = new Button("Local Game");
	button[1] = new Button("Network Game");
	button[2] = new Button("Preferences");
	button[3] = new Button("Players");
    button[4] = new Button("Credits");
    button[5] = new Button("Quit");

	int maxWidth = 0;
	for(int i=0; i<buttonCount; i++){
		add(button[i]);
		if (button[i]->getWidth() > maxWidth) maxWidth = button[i]->getWidth();
	}
    for(int i=0; i<buttonCount; i++){
        button[i]->setWidth(maxWidth);
        button[i]->setPosition(getWidth() / 2 - button[i]->getWidth() / 2,
            getHeight() / 12 + getHeight() / 6 * i - button[i]->getHeight() / 2);
    }

    button[0]->addActionListener(new MenuCreateGameActionListener());
    button[1]->addActionListener(new MenuConnectActionListener());
    button[2]->addActionListener(new MenuGfxActionListener());
    button[3]->addActionListener(new MenuPlayersActionListener());
    button[4]->addActionListener(new MenuCreditsActionListener());
    button[5]->addActionListener(new QuitActionListener());

    ((CustomContainer*)this)->initWidgets();
}
