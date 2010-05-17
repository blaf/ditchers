#include "menu/menugamelobby.hpp"
#include "global.hpp"
#include "basic.hpp"
#include "game.hpp"
#include "network.hpp"
#include "settings.hpp"
#include "robottype.hpp"

    /* *** MenuGameLobby *** */

/**
Contains possible numbers of teams.
*/
class MenuGameLobby::TeamsListModel : public ListModel{
public:
        std::string getElementAt(int i){
                return ((i == 0) ? "No" : intToString(i+1)) + " teams";
        }
        int getNumberOfElements(){
                return 8;
        }
};

/**
Contains available teams.
*/
class MenuGameLobby::TeamListModel : public ListModel{
public:
        std::string getElementAt(int i){
            return intToString(i+1)+" - "+gameplay.getTeamColor(i+1);
        }
        int getNumberOfElements(){
                return gameplay.teamscount;
        }
};

/**
Contains local players.
*/
class MenuGameLobby::LocalPlayersListModel : public ListModel{
public:
        std::string getElementAt(int i){
            if ((i >= 0) && (i < getNumberOfElements()))
                return (std::string)(settings.locals[i]->artificial ? "} " : "{ ") +
                    (std::string)(settings.locals[i]->chosen ? "> " : "") +
                    (std::string)(settings.locals[i]->name);
            else return "";
        }
        int getNumberOfElements(){
            return settings.locals.size();
    }
};

/**
Contains game players.
*/
class MenuGameLobby::PlayersListModel : public ListModel{
public:
        std::string getElementAt(int i){
            PrePlayer* pl = gameplay.preplayers.vector<PrePlayer*>::at(i);
            if (pl) return
                (gameplay.teamscount ? (intToString(pl->getTeam()) + " ") : "") +
                (string)((!gameplay.local) && (pl->local >= 0) ? "> " : "") +
                (pl->name);
            else
            return "- empty -";
        }
        int getNumberOfElements(){
            return gameplay.preplayers.size();
    }
};

/**
If at least two players are in the game, game starts.
*/
class MenuGameLobby::StartGameActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        int count = 0;
        int team = -1;
        for (unsigned int i=0; i<gameplay.preplayers.size(); i++){
            PrePlayer* ppl = gameplay.preplayers.vector<PrePlayer*>::at(i);
            if (ppl){
                count++;
                if (team == -1) team = ppl->getTeam();
                else if (team != ppl->getTeam()) team = -2;
            }
        }
        if ((count < 2) || (gameplay.teamscount && (team != -2))) return;

        if (!gameplay.local){
            network.buffer << "ls";
            network.send();
        }else
            gameplay.approved = true;
    }
};

/**
Adds a local player to the game.
*/
class MenuGameLobby::AddPlayerActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){
        MenuGameLobby* menu = userface.mGameLobby;

        if (!inRange(menu->listLocalPlayers)){
            menu->buttonAddPlayer->setVisible(false);
            return;
        }

        int selloc = menu->listLocalPlayers->getSelected();
        int selpl  = menu->listPlayers->getSelected();

        if (!inRange(menu->listPlayers)) selpl = -1;

        if (gameplay.blob) selpl = -1;
        else if (selpl == -1){
            menu->listPlayers->setSelected(0);
            selpl = menu->listPlayers->getSelected();
        }

        if ((settings.locals[selloc]->chosen)
           || ((!gameplay.blob) &&  gameplay.preplayers.byIndex(selpl))){
            menu->buttonAddPlayer->setVisible(false);
            return;
        }

        if (gameplay.local){
            PrePlayer* pladd = gameplay.preplayers.add(selloc, selpl);
            pladd->name = settings.locals[selloc]->name;
            pladd->robottype = settings.locals[selloc]->robottype;
            pladd->local = selloc;
            settings.locals[selloc]->chosen = true;
            if (!settings.locals[selloc]->artificial) gameplay.localhumans++;
            gameplay.localplayers++;
        }else{
            network.buffer << "l+ " << settings.locals[selloc]->name
                << " " << settings.locals[selloc]->robottype->unique
                << " " << (settings.locals[selloc]->artificial ? 1 : 0)
                << " " << selpl;
            network.send();
        }

        menu->update();
    }
};

/**
Removes a local player from the game.
*/
class MenuGameLobby::RemovePlayerActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){
        MenuGameLobby* menu = userface.mGameLobby;

        if (!inRange(menu->listPlayers)){
            menu->buttonRemovePlayer->setVisible(false);
            menu->dropTeams->setVisible(false);
            return;
        }

        int selpl = menu->listPlayers->getSelected();

        PrePlayer* pl = gameplay.preplayers.vector<PrePlayer*>::at(selpl);

        if (!pl){
            menu->buttonRemovePlayer->setVisible(false);
            menu->dropTeams->setVisible(false);
            return;
        }

        if (gameplay.local){

            settings.locals[pl->local]->chosen = false;
            if (!settings.locals[pl->local]->artificial) gameplay.localhumans--;

            gameplay.preplayers.remove(selpl, gameplay.blob);
            gameplay.localplayers--;

        }else if (pl->local >= 0){

            network.buffer << "l- " << pl->id;
            network.send();

        }
        menu->update();
    }
};

class MenuGameLobby::PlayersSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuGameLobby* menu = userface.mGameLobby;

        if (!inRange(menu->listPlayers)) return;
        userface.mGameLobby->update();
    }
};

/**
Sets number of teams.
*/
class MenuGameLobby::TeamsSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuGameLobby* menu = userface.mGameLobby;

        if (!inRange(menu->dropTeams)) return;
        if (!menu->dropTeams->isVisible()) return;

        if (gameplay.local){
                if (menu) gameplay.teamscount = menu->dropTeams->getSelected() + 1;
                if (gameplay.teamscount == 1) gameplay.teamscount = 0;

                if (gameplay.teamscount){
                        int selpl   = menu->listPlayers->getSelected();
                        PrePlayer* pl = (inRange(menu->listPlayers)) ?
                                gameplay.preplayers.vector<PrePlayer*>::at(selpl) : 0;

                        if (pl && (gameplay.local || (pl->local >= 0))){
                                menu->buttonRemovePlayer->setVisible(true);
                                menu->dropTeam->setSelected(pl->getTeam()-1);
                                menu->dropTeam->setVisible(true);
                        }else{
                                menu->buttonRemovePlayer->setVisible(false);
                                menu->dropTeam->setVisible(false);
                        }
                }else{
                        menu->dropTeam->setVisible(false);
                }
        }else{
                int teamscount = menu->dropTeams->getSelected() + 1;
                if (teamscount == 1) teamscount = 0;
                network.buffer << "lt " << teamscount;
                network.send();
        }
    }
};

/**
Sets team of a player.
*/
class MenuGameLobby::TeamSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuGameLobby* menu = userface.mGameLobby;

        if (!inRange(menu->dropTeam)) return;
        if (!menu->dropTeam->isVisible()) return;

        int selpl   = menu->listPlayers->getSelected();
        PrePlayer* pl = (inRange(menu->listPlayers)) ?
                gameplay.preplayers.vector<PrePlayer*>::at(selpl) : 0;

        if (pl && (gameplay.local || (pl->local >= 0))){
            if (gameplay.local){
                pl->team = menu->dropTeam->getSelected() + 1;
            }else{
                network.buffer << "lc " << pl->id << " " << menu->dropTeam->getSelected() + 1;
                network.send();
            }
        }else{
            menu->buttonRemovePlayer->setVisible(false);
            menu->dropTeam->setVisible(false);
        }
    }
};

/**
Sets points limit slider.
*/
class MenuGameLobby::LimitSliderActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){ userface.mGameLobby->limitSliderAction(0); }
};

/**
Adds one to or substracts one from points limit.
*/
void MenuGameLobby::limitSliderAction(int inc){
    MenuGameLobby* menu = userface.mGameLobby;
    menu->sliderLimit->setValue(menu->sliderLimit->getValue()+inc);
    int limit = (int)menu->sliderLimit->getValue();
    menu->fieldLimit->setText(intToString(limit));
    if (gameplay.local)
        gameplay.limit = limit;
    else{
        network.buffer << "ll " << limit;
        network.send();
    }
}

/**
Substracts one from points limit.
*/
class MenuGameLobby::LimitLeftActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){ userface.mGameLobby->limitSliderAction(-1); }
};

/**
Adds one to points limit.
*/
class MenuGameLobby::LimitRightActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){ userface.mGameLobby->limitSliderAction(1); }
};

/**
Text field that acts upon Enter key press.
*/
class MenuGameLobby::ChatTextField : public TextField{
    void keyPressed(KeyEvent& keyEvent) {
        if (keyEvent.getKey() == Key::ENTER){
            MenuGameLobby* menu = userface.mGameLobby;
            menu->chatAction();
        }else TextField::keyPressed(keyEvent);
    };

    public:

    ChatTextField() : TextField("") {};
};

class MenuGameLobby::ChatActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){
        MenuGameLobby* menu = userface.mGameLobby;
        menu->chatAction();
    }
};

/**
Sends chat message to the server.
*/
void MenuGameLobby::chatAction(){
    if (!gameplay.local){
        std::string message = fieldChat->getText();
        network.buffer << "lm " << network.clientname << ": " << message;
        network.send();
        fieldChat->setText("");
    }
}

/**
Sets map and widget values according to game settings.
*/
void MenuGameLobby::onEnter(){

    dropTeams->setSelected(0);
    listLocalPlayers->setSelected(-1);
    listPlayers->setSelected(-1);

    gameplay.limit = 10;
    sliderLimit->setValue(gameplay.limit);

    gameplay.teamscount = 0;

    gameplay.localplayers   = 0;
    gameplay.localhumans    = 0;

    settings.map   = 0;
    for (unsigned int i = 0; i < settings.maps.size(); i++)
        if (settings.maps[i]->unique == settings.mapunique){
            settings.map  = settings.maps[i];
            gameplay.blob = settings.map->blob;
            break;
        }

    //    if (settings.map == 0) cout << "error: unknown map!" << endl;

    gameplay.preplayers.clear();
    if (!gameplay.blob) gameplay.preplayers.resize(settings.map->limit);

    for (unsigned int i = 0; i < settings.locals.size(); i++)
        settings.locals[i]->chosen = false;

    if (gameplay.local) setHeight(MENUHEIGHT / 3 * 2); /* hiding chatbox */
    else setHeight(MENUHEIGHT);

    iconMap->setImage(userface.imageMap);

    textChat->setText("");
}

void MenuGameLobby::onLeave(){
    if (!gameplay.local){
        network.buffer << "sl";
        network.send();
    }
    gameplay.preplayers.clear();
}

/**
Sets buttons visibility and widget values
according to game settings and list indices.
*/
void MenuGameLobby::update(){
        int selpl   = listPlayers->getSelected();
        int selloc  = listLocalPlayers->getSelected();

        PrePlayer* pl = (inRange(listPlayers)) ?
            gameplay.preplayers.vector<PrePlayer*>::at(selpl) : 0;

        Local* loc = (inRange(listLocalPlayers)) ?
            settings.locals.vector<Local*>::at(selloc) : 0;

        if ((!pl || gameplay.blob) && loc && !loc->chosen && ((gameplay.localhumans < 2) || loc->artificial))
            buttonAddPlayer->setVisible(true);
        else buttonAddPlayer->setVisible(false);

        dropTeams->setSelected(gameplay.teamscount == 0 ? 0 : gameplay.teamscount-1);

        if (pl && (gameplay.local || (pl->local >= 0))){
            buttonRemovePlayer->setVisible(true);
            dropTeam->setSelected(pl->getTeam()-1);
            if (gameplay.teamscount) dropTeam->setVisible(true);
            else dropTeam->setVisible(false);
        }else{
            buttonRemovePlayer->setVisible(false);
            if (pl) dropTeam->setSelected(pl->getTeam()-1);
            dropTeam->setVisible(false);
        }

        fieldLimit->setText(intToString(gameplay.limit));
        sliderLimit->setValue(gameplay.limit);
}

/**
Puts all widgets into the menu.
*/
MenuGameLobby::MenuGameLobby(){

        setDimension(Rectangle(0, 0, MENUWIDTH, MENUHEIGHT));

        CustomContainer* contControl = new CustomContainer();
        contControl->setDimension
        (Rectangle(getWidth()/32, getHeight()/32, getWidth(), getHeight() / 16));
        add(contControl);

        buttonBack = new Button("Leave");
        buttonBack->setPosition(0, 0);
        buttonBack->addActionListener(new BackActionListener);
        contControl->add(buttonBack);

        buttonStartGame = new Button("Start");
        buttonStartGame->setPosition(buttonBack->getWidth() * 3 / 2, 0);
        buttonStartGame->addActionListener(new StartGameActionListener);
        contControl->add(buttonStartGame);

        PlayersSelectionListener* playersselectionlistener = new PlayersSelectionListener;

        listLocalPlayers = new ListBox(new LocalPlayersListModel);
        listLocalPlayers->addSelectionListener(playersselectionlistener);
        ScrollArea* scrollLocalPlayers =
        new ScrollArea(listLocalPlayers, gcn::ScrollArea::SHOW_NEVER, gcn::ScrollArea::SHOW_ALWAYS);
        scrollLocalPlayers->setSize(getWidth() / 4, getHeight() / 2);
        scrollLocalPlayers->setPosition(getWidth() / 20, getHeight() / 8);
        add(scrollLocalPlayers);
        listLocalPlayers->setWidth(scrollLocalPlayers->getWidth());
    CustomContainer::setColor(listLocalPlayers);

        listPlayers = new ListBox(new PlayersListModel);
        listPlayers->addSelectionListener(playersselectionlistener);
        ScrollArea* scrollPlayers =
        new ScrollArea(listPlayers, gcn::ScrollArea::SHOW_NEVER, gcn::ScrollArea::SHOW_ALWAYS);
        scrollPlayers->setSize(getWidth() / 5 * 2, getHeight() / 2);
        scrollPlayers->setPosition(getWidth() / 20 * 11, getHeight() / 8);
        add(scrollPlayers);
        listPlayers->setWidth(scrollPlayers->getWidth());
    CustomContainer::setColor(listPlayers);

        buttonAddPlayer = new Button("Add >");
        buttonAddPlayer->addActionListener(new AddPlayerActionListener);
        buttonAddPlayer->setVisible(false);
        add(buttonAddPlayer);

        int playerbuttonwidth = buttonAddPlayer->getWidth();
        int playerbuttonheight = buttonAddPlayer->getHeight();

        buttonRemovePlayer = new Button("< Remove");
        buttonRemovePlayer->addActionListener(new RemovePlayerActionListener);
        buttonRemovePlayer->setVisible(false);
        add(buttonRemovePlayer);

        playerbuttonwidth =
        (buttonRemovePlayer->getWidth() > playerbuttonwidth) ?
        buttonRemovePlayer->getWidth() : playerbuttonwidth;
        playerbuttonwidth = playerbuttonwidth * 3 / 2;

        buttonAddPlayer->setWidth(playerbuttonwidth);
        buttonRemovePlayer->setWidth(playerbuttonwidth);

        buttonAddPlayer->setPosition(
                (scrollLocalPlayers->getX()+scrollLocalPlayers->getWidth()+scrollPlayers->getX()-buttonAddPlayer->getWidth()) / 2,
                getHeight() / 8);
        buttonRemovePlayer->setPosition(
                (scrollLocalPlayers->getX()+scrollLocalPlayers->getWidth()+scrollPlayers->getX()-buttonRemovePlayer->getWidth()) / 2,
                getHeight() / 4);

        dropTeams = new DropDown(new TeamsListModel);
        dropTeams->addSelectionListener(new TeamsSelectionListener);
        dropTeams->setWidth(playerbuttonwidth);
        dropTeams->setPosition(
                (scrollLocalPlayers->getX()+scrollLocalPlayers->getWidth()+scrollPlayers->getX()-dropTeams->getWidth()) / 2,
                getHeight() / 8 * 3);
        dropTeams->setVisible(true);
        add(dropTeams);

        dropTeam = new DropDown(new TeamListModel);
        dropTeam->addSelectionListener(new TeamSelectionListener);
        dropTeam->setWidth(playerbuttonwidth);
        dropTeam->setPosition(
        (scrollLocalPlayers->getX()+scrollLocalPlayers->getWidth()+scrollPlayers->getX()-dropTeam->getWidth()) / 2,
                getHeight() / 16 * 7);
        add(dropTeam);

    Label* labelPoints = new Label("Points to win:");
    labelPoints->setPosition(
                           (scrollLocalPlayers->getX()+scrollLocalPlayers->getWidth()+scrollPlayers->getX()-dropTeam->getWidth()) / 2 - 16,
                           getHeight() / 2);
    add(labelPoints);

        sliderLimit = new Slider(1, 100);
        sliderLimit->addActionListener(new LimitSliderActionListener());
        sliderLimit->setSize(playerbuttonwidth, playerbuttonheight / 2);
        sliderLimit->setPosition(
            (scrollLocalPlayers->getX()+scrollLocalPlayers->getWidth()+scrollPlayers->getX()-sliderLimit->getWidth()) / 2,
            scrollLocalPlayers->getY()+scrollLocalPlayers->getHeight()-sliderLimit->getHeight());
        sliderLimit->setVisible(true);
        sliderLimit->setValue(10);
        add(sliderLimit);

        buttonLimitLeft = new Button("-");
        buttonLimitRight = new Button("+");
        buttonLimitLeft->addActionListener(new LimitLeftActionListener());
        buttonLimitRight->addActionListener(new LimitRightActionListener());
        buttonLimitLeft->setPosition(sliderLimit->getX(), sliderLimit->getY()-buttonLimitLeft->getHeight()-2);
        buttonLimitRight->setPosition(
            sliderLimit->getX()+sliderLimit->getWidth()-buttonLimitRight->getWidth(),
            sliderLimit->getY()-buttonLimitRight->getHeight()-2);
        buttonLimitLeft->setVisible(true);
        buttonLimitRight->setVisible(true);
        add(buttonLimitLeft);
        add(buttonLimitRight);

        fieldLimit = new TextField(intToString(gameplay.limit));
        fieldLimit->setPosition(
            buttonLimitLeft->getX()+buttonLimitLeft->getWidth(),
            buttonLimitLeft->getY());
        fieldLimit->setSize(
            buttonLimitRight->getX()-buttonLimitLeft->getX()-buttonLimitLeft->getWidth(),
            playerbuttonheight);
        fieldLimit->setEnabled(false);
        fieldLimit->setFocusable(false);
        fieldLimit->setVisible(true);
        add(fieldLimit);

        CustomContainer* contChat = new CustomContainer();
        contChat->setDimension
        (Rectangle(getWidth() / 36, getHeight() / 3 * 2, getWidth() / 3 * 2, getHeight() / 10 * 3));

    buttonChat = new Button("Send");
        buttonChat->addActionListener(new ChatActionListener);

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
    CustomContainer::setColor(textChat);
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

    iconMap = new Icon();
    iconMap->setDimension(Rectangle(getWidth() * 3 / 4, getHeight() * 5 / 7, 96, 96));
    iconMap->setVisible(true);
    add(iconMap);

    ((CustomContainer*)this)->initWidgets();
}
