#include "menu/menuconnect.hpp"
#include "global.hpp"
#include "network.hpp"
#include "game.hpp"
#include "gfx.hpp"
#include "settings.hpp"

#include <sstream>

    /* *** MenuConnect *** */

/**
Attempts a connection to server.
*/
class MenuConnect::ConnectActionListener : public ActionListener{
	void action(const ActionEvent& actionEvent) {

	    MenuConnect* menu = userface.mConnect;

        menu->acquireData();

        if (network.connect()){
            network.buffer << "sn " << " " << network.clientname << " " << gfx.res.x << " " << gfx.res.y;
            network.send();
            userface.switchTo((Menu*)userface.mNetGames);
        }else userface.showMessage("Cannot connect to remote host");

	}
};

/**
Reads data from fields and stores into settings.
*/
void MenuConnect::acquireData(){
    network.hostname = cleanname(fieldAddress->getText());
    network.clientname = cleanname(fieldName->getText());
    network.port = atoi(fieldPort->getText().c_str());

    settings.hostname = network.hostname;
    settings.clientname = network.clientname;
    settings.port = network.port;

    settings.writeSettings();
}

void MenuConnect::onEnter(){
    if (!gameplay.local) network.disconnect();
};

void MenuConnect::onLeave(){
    acquireData();
}

/**
Puts all widgets into the menu.
*/
MenuConnect::MenuConnect(){

    setDimension(Rectangle(MENUWIDTH / 4, MENUHEIGHT / 4, MENUWIDTH / 2, MENUHEIGHT / 2));

    Label* labelClient = new Label("Your network name:");
    labelClient->setPosition(getWidth() / 2 - labelClient->getWidth() / 2, getHeight() * 1 / 9);
    add(labelClient);

    fieldName = new TextField(settings.clientname);
    fieldName->setWidth(getWidth() / 2);
	fieldName->setPosition(getWidth() / 2 - fieldName->getWidth() / 2,
        getHeight() * 2 / 9 - fieldName->getHeight() / 2);
	add(fieldName);

    Label* labelAddress = new Label("Server address:");
    labelAddress->setPosition(getWidth() / 2 - labelAddress->getWidth() / 2, getHeight() * 3 / 9);
    add(labelAddress);

    fieldAddress = new TextField(settings.hostname);
    fieldAddress->setWidth(getWidth() / 6 * 5);
	fieldAddress->setPosition(getWidth() / 2 - fieldAddress->getWidth() / 2,
        getHeight() * 4 / 9 - fieldAddress->getHeight() / 2);
	add(fieldAddress);

    Label* labelPort = new Label("Port:");
    labelPort->setPosition(getWidth() / 2 - labelPort->getWidth() / 2, getHeight() * 5 / 9);
    add(labelPort);

    stringstream ss; ss << settings.port;
	fieldPort = new TextField(ss.str());
	fieldPort->setPosition(getWidth() / 2 - fieldPort->getWidth() / 2,
        getHeight() * 6 / 9 - fieldPort->getHeight() / 2);
	add(fieldPort);

	buttonBack = new Button("Back");
	buttonBack->setPosition(getWidth() / 3 - buttonBack->getWidth() / 2,
        getHeight() * 8 / 9 - buttonBack->getHeight() / 2);
	buttonBack->addActionListener(new BackActionListener());
	add(buttonBack);

	buttonConnect = new Button("Connect");
	Menu::setColor(buttonConnect);
    buttonConnect->setPosition(getWidth() * 2 / 3 - buttonConnect->getWidth() / 2,
        getHeight() * 8 / 9 - buttonConnect->getHeight() / 2);
	buttonConnect->addActionListener(new ConnectActionListener());
	add(buttonConnect);

    initWidgets();
}
