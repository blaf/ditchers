#include "menu/menucreategame.hpp"
#include "global.hpp"
#include "game.hpp"
#include "network.hpp"
#include "settings.hpp"
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

    /* *** MenuCreateGame *** */

/**
Creates the game.
*/
class MenuCreateGame::NextActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {

        MenuCreateGame* menu = userface.mCreateGame;

        settings.gamename = menu->cleanname(menu->fieldName->getText());

        settings.mapunique = settings.maps[menu->dropMap->getSelected()]->unique;

        settings.maphash = settings.maps[menu->dropMap->getSelected()]->hash;

        bool gameblob = settings.maps[menu->dropMap->getSelected()]->blob;

        int gamelimit = settings.maps[menu->dropMap->getSelected()]->limit;

        if (!gameplay.local){
            network.buffer << "sc " << settings.gamename << " " << settings.mapunique
                << " " << settings.maphash << " " << (gameblob ? 1 : 0) << " " << gamelimit;
            network.send();
        }

        Menu* prec = userface.current->preceding;
        userface.switchTo((Menu*)userface.mGameLobby);
        userface.current->preceding = prec;

    }
};

/**
Contains all known maps.
*/
class MenuCreateGame::MapListModel : public ListModel{
    public:
	std::string getElementAt(int i){
        if ((i >= 0) && (i < getNumberOfElements()))
            return settings.maps[i]->name;
        else return "";
	}
	int getNumberOfElements(){
        return settings.maps.size();
    }
};

/**
Sets map preview upon map change.
*/
class MenuCreateGame::MapSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        userface.mCreateGame->setMapImage();
    }
};

/**
Changes the map icon according to the selected map.
*/
void MenuCreateGame::setMapImage(){
    Map* thismap = settings.maps[dropMap->getSelected()];
    string mapfile = thismap->dir;
    if (userface.imageMap){ delete(userface.imageMap); userface.imageMap = 0; }
    if (fs::exists( "data/maps/"+mapfile+"/preview.png" ))
        userface.imageMap = Image::load("data/maps/"+mapfile+"/preview.png");
    else
        userface.imageMap = Image::load("data/maps/nopreview.png");
    iconMap->setImage(userface.imageMap);
    if (thismap->size.x != 0){
        labelSize->setCaption(intToString(thismap->size.x)+" x "+intToString(thismap->size.y));
        labelSize->adjustSize();
        labelSize->setPosition(getWidth() / 2 - labelSize->getWidth() / 2, getHeight() / 12 * 5);
    }
    if (thismap->limit != 0)
        labelPlayers->setCaption("max "+intToString(thismap->limit)+" players");
    else if (thismap->blob != 0)
        labelPlayers->setCaption("unlimited players");
    else
        labelPlayers->setCaption("unlimited players");

    labelPlayers->adjustSize();
    labelPlayers->setPosition(getWidth() / 2 - labelPlayers->getWidth() / 2, getHeight() / 24 * 17);
}

void MenuCreateGame::onEnter(){
    setMapImage();
}

/**
Puts all widgets into the menu.
*/
MenuCreateGame::MenuCreateGame(){

    setDimension(Rectangle(MENUWIDTH / 4, 0, MENUWIDTH / 2, MENUHEIGHT));

    Label* labelGame = new Label("Game title:");
    labelGame->setPosition(getWidth() / 2 - labelGame->getWidth() / 2, getHeight() / 12);
    add(labelGame);

    fieldName = new TextField("game");
	fieldName->setWidth(getWidth() / 3);
	fieldName->setPosition(getWidth() / 2 - fieldName->getWidth() / 2,
		getHeight() / 6 * 1 - fieldName->getHeight() / 2);
    add(fieldName);

    Label* labelMap = new Label("Map:");
    labelMap->setPosition(getWidth() / 2 - labelMap->getWidth() / 2, getHeight() / 12 * 3);
    add(labelMap);

    dropMap = new DropDown(new MapListModel());
	dropMap->setSelected(0);
	dropMap->setPosition(getWidth() / 2 - dropMap->getWidth() / 2,
		getHeight() / 6 * 2 - dropMap->getHeight() / 2);
	dropMap->addSelectionListener(new MapSelectionListener);
	add(dropMap);

    labelSize = new Label("??? x ???");
    labelSize->setPosition(getWidth() / 2 - labelSize->getWidth() / 2, getHeight() / 12 * 5);
    add(labelSize);
    
    iconMap = new Icon();
    iconMap->setDimension(Rectangle(getWidth() / 2 - 48, getHeight() * 7 / 12 - 48, 96, 96));
    iconMap->setVisible(true);
    add(iconMap);

    labelPlayers = new Label("unlimited players");
    labelPlayers->setPosition(getWidth() / 2 - labelPlayers->getWidth() / 2, getHeight() / 24 * 17);
    add(labelPlayers);
    
    buttonBack = new Button("Back");
	buttonBack->setPosition(getWidth() / 4 - buttonBack->getWidth() / 2,
		getHeight() / 6 * 5 - buttonBack->getHeight() / 2);
	buttonBack->addActionListener(new BackActionListener());
	add(buttonBack);

	buttonNext = new Button("Create");
	buttonNext->setPosition(getWidth() / 4 * 3 - buttonNext->getWidth() / 2,
		getHeight() / 6 * 5 - buttonNext->getHeight() / 2);
	buttonNext->addActionListener(new NextActionListener());
	add(buttonNext);

    ((CustomContainer*)this)->initWidgets();
}
