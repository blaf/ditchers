#include "menu/menunetgames.hpp"
#include "global.hpp"
#include "network.hpp"
#include "settings.hpp"
#include "game.hpp"
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

/**
List of games on server.
*/
class MenuNetGames::GamesListModel : public ListModel{
    public:
        std::string getElementAt(int i){
            Game* gm = network.games.atIndex(i);
            if (gm != 0) return gm->name;
            else return "";
        }
        int getNumberOfElements(){
            return network.games.size();
        }
};

/**
List of connected clients.
*/
class MenuNetGames::ClientsListModel : public ListModel{
    public:
        std::string getElementAt(int i){
            Client* cl = network.clients.atIndex(i);
            if (cl != 0) return cl->name;
            else return "";
        }
        int getNumberOfElements(){
            return network.clients.size();
    }
};

/**
Shows map preview and "Join" button for the selected game.
*/
class MenuNetGames::GamesSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuNetGames* menu = userface.mNetGames;
        int selgame = userface.mNetGames->listGames->getSelected();
        if (inRange(userface.mNetGames->listGames)){

            Game* gm = network.games.atIndex(selgame);
            string gameunique = gm->mapname;
            string gamehash = gm->maphash;

            menu->contGame->setVisible(true);
            int tmap   = -1;
            bool imap  = false;
            for (unsigned int i = 0; i < settings.maps.size(); i++)
                if (settings.maps[i]->unique == gameunique){
                    tmap  = i;
                    break;
                }
            string mappath;
            if (userface.imageMap){ delete(userface.imageMap); userface.imageMap = 0; }

            if (tmap >= 0){
                mappath = settings.maps[tmap]->wholePath();
                if (fs::exists( mappath+"/preview.png" )){
                    imap = true;
                    userface.imageMap = Image::load(mappath+"/preview.png");
                }
            }
            if (!imap) userface.imageMap = Image::load(settings.loc_nopreviewimg);

            menu->iconMap->setImage(userface.imageMap);
            if ((tmap >= 0) && (settings.maps[tmap]->hash == gamehash)){
                menu->buttonJoin->setVisible(true);
                if (settings.maps[tmap]->limit > 0)
                    menu->labelMap->setCaption(intToString(settings.maps[tmap]->limit)+" players");
                else menu->labelMap->setCaption("unlimited players");
                if (settings.maps[tmap]->size.x > 0){
                    menu->labelSize->setCaption(intToString(settings.maps[tmap]->size.x)
                        +" x "+intToString(settings.maps[tmap]->size.y));
                }else menu->labelSize->setCaption("??? x ???");
            }else{
                menu->buttonJoin->setVisible(false);
                menu->labelSize->setCaption("");
                if (tmap == -1){                        
                    menu->labelMap->setCaption("map not found");
                }else if (settings.maps[tmap]->hash != gamehash){
                    menu->labelMap->setCaption("map hashes differ");
                }
            }
            menu->labelMap->adjustSize();
            menu->labelSize->adjustSize();
        }else
            menu->contGame->setVisible(false);
    }
};

/**
Joins the selected game.
*/
class MenuNetGames::JoinGameActionListener : public ActionListener{
        void action(const ActionEvent& actionEvent) {
            int selgame = userface.mNetGames->listGames->getSelected();
            if (inRange(userface.mNetGames->listGames)){
                Game* gm = network.games.atIndex(selgame);
                network.buffer << "sj " << gm->id;
                network.send();
            }else
                userface.mNetGames->contGame->setVisible(false);
        }
};

/**
Switch to creating a game.
*/
class MenuNetGames::CreateGameActionListener : public ActionListener{
        void action(const ActionEvent& actionEvent) {
                userface.switchTo((Menu*)userface.mCreateGame);
        }
};

/**
Text field that performs an action upon Enter press.
*/
class MenuNetGames::ChatTextField : public TextField{
    void keyPressed(KeyEvent& keyEvent) {
        if (keyEvent.getKey() == Key::ENTER){
            MenuNetGames* menu = userface.mNetGames;
            menu->chatAction();
        }else TextField::keyPressed(keyEvent);
    };

    public:

    ChatTextField() : TextField("") {};
};


class MenuNetGames::ChatActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        MenuNetGames* menu = userface.mNetGames;
        menu->chatAction();
    }
};

/**
Sends the chat message to the server.
*/
void MenuNetGames::chatAction(){
    std::string message = fieldChat->getText();
    network.buffer << "sm " << network.clientname << ": " << message;
    network.send();
    fieldChat->setText("");
}

void MenuNetGames::onEnter(){
    gameplay.localplayers = 0;
    gameplay.localhumans  = 0;
    gameplay.preplayers.clear();
    userface.mNetGames->contGame->setVisible(false);
}

/**
Puts all widgets into the menu.
*/
MenuNetGames::MenuNetGames(){

        setDimension(Rectangle(0, 0, MENUWIDTH, MENUHEIGHT));
    int w = getWidth(); int h = getHeight();
    
    CustomContainer* contControl = new CustomContainer();
        contControl->setDimension
        (Rectangle(w / 24, h / 24, w * 22 / 24, h * 2 / 24));
        add(contControl);

        buttonBack = new Button("Leave");
        buttonBack->setPosition(0, 0);
        buttonBack->addActionListener(new BackActionListener());
        contControl->add(buttonBack);

        buttonCreate = new Button("Create");
        buttonCreate->setPosition(w * 3 / 24, 0);
        buttonCreate->addActionListener(new CreateGameActionListener());
        contControl->add(buttonCreate);

    CustomContainer* contGames = new CustomContainer();
        contGames->setDimension
        (Rectangle(w * 1 / 24, h * 4 / 24, w * 16 / 24, h * 11 / 24));
        add(contGames);

    Label* labelGames = new Label("Games:");
    labelGames->setPosition(w * 4 / 24 - labelGames->getWidth() / 2, h * 4 / 24 - labelGames->getHeight());
    add(labelGames);

    listGames = new ListBox(new GamesListModel);
        listGames->addSelectionListener(new GamesSelectionListener());
        ScrollArea* scrollGames =
        new ScrollArea(listGames, gcn::ScrollArea::SHOW_AUTO, gcn::ScrollArea::SHOW_ALWAYS);
    scrollGames->setSize(w * 6 / 24, h * 11 / 24);
        scrollGames->setPosition(0, 0);
    contGames->add(scrollGames);
    CustomContainer::setColor(listGames);

    contGame = new CustomContainer();
    contGame->setDimension(Rectangle(w * 7 / 24, 0, w * 9 / 24, h * 11 / 24));
    contGames->add(contGame);
    contGame->setVisible(false);

        buttonJoin = new Button("Join");
    buttonJoin->setPosition(w * 0 / 24, h * 0 / 24);
    buttonJoin->addActionListener(new JoinGameActionListener());
    contGame->add(buttonJoin);

    iconMap = new Icon();
    iconMap->setDimension(Rectangle(w * 0 / 24, h * 2 / 24, 96, 96));
    iconMap->setVisible(true);
    contGame->add(iconMap);

    labelMap = new Label("");
    labelMap->setPosition(w * 0 / 24, h * 8 / 24 - labelMap->getHeight() / 2);
    contGame->add(labelMap);

    labelSize = new Label("");
    labelSize->setPosition(w * 0 / 24, h * 9 / 24 - labelSize->getHeight() / 2);
    contGame->add(labelSize);

    CustomContainer* contChat = new CustomContainer();
        contChat->setDimension
        (Rectangle(w * 1 / 24, h * 16 / 24, w * 15 / 24, h * 7 / 24));

    buttonChat = new Button("Send");
    buttonChat->addActionListener(new ChatActionListener());

    fieldChat = new ChatTextField();

    textChat = new TextBox("");

    CustomContainer* contChatBox = new CustomContainer();
    contChatBox->setDimension
        (Rectangle(0, 0, contChat->getWidth(), contChat->getHeight() - buttonChat->getHeight()));

    scrollChat =
        new ScrollArea(textChat, gcn::ScrollArea::SHOW_AUTO, gcn::ScrollArea::SHOW_ALWAYS);
    scrollChat->setPosition(0, 0);
    scrollChat->setSize(contChatBox->getWidth(), contChatBox->getHeight());

        textChat->setPosition(0, 0);
    textChat->setSize
        (scrollChat->getWidth()-scrollChat->getScrollbarWidth(), scrollChat->getHeight());
    textChat->setEditable(false);
    CustomContainer::setColor(textChat);

    fieldChat->setWidth(contChat->getWidth()-buttonChat->getWidth());

    buttonChat->setPosition(fieldChat->getWidth(),textChat->getHeight());

    fieldChat->setPosition(0,buttonChat->getY()+(buttonChat->getHeight()-fieldChat->getHeight()) / 2);

        add(contChat);
    contChat->add(contChatBox);
    contChatBox->add(scrollChat);
    contChat->add(fieldChat);
    contChat->add(buttonChat);

    Label* labelClients = new Label("Clients:");
    labelClients->setPosition(w * 20 / 24 - labelClients->getWidth() / 2, h * 4 / 24 - labelClients->getHeight());
    add(labelClients);

    CustomContainer* contClients = new CustomContainer();
        contClients->setDimension
        (Rectangle(w * 17 / 24, h * 4 / 24, w * 6 / 24, h * 19 / 24));
        listClients = new ListBox(new ClientsListModel);
        ScrollArea* scrollClients =
            new ScrollArea(listClients, gcn::ScrollArea::SHOW_AUTO, gcn::ScrollArea::SHOW_ALWAYS);
        scrollClients->setSize(contClients->getWidth(), contClients->getHeight());
        scrollClients->setPosition(0, 0);
    CustomContainer::setColor(listClients);

    contClients->add(scrollClients);
    add(contClients);

    initWidgets();
}
