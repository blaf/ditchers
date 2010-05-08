#include "menu/menuplayers.hpp"
#include "global.hpp"
#include "settings.hpp"
#include "robottype.hpp"

/**
List of local players.
*/
class MenuPlayers::PlayersListModel : public ListModel{
public:
	std::string getElementAt(int i){
        if ((i >= 0) && (i < getNumberOfElements()))
            return (settings.locals[i]->artificial ? "} " : "{ ") + settings.locals[i]->name;
        else return "";
	}
	int getNumberOfElements(){
	    return settings.locals.size();
    }
};

/**
List of robot types.
*/
class MenuPlayers::RobotListModel : public ListModel{
public:
	std::string getElementAt(int i){
        if ((i >= 0) && (i < getNumberOfElements()))
	       return settings.robottypes[i]->unique;
        else return "";
	}
	int getNumberOfElements(){
	    return settings.robottypes.size();
    }
};

/**
AI / Human list.
*/
class MenuPlayers::AIListModel : public ListModel{
public:
	std::string getElementAt(int i){
	    if (i == 1) return "AI";
	    else return "Human";
	}
	int getNumberOfElements(){
	    return 2;
    }
};

/**
List of AI scripts.
*/
class MenuPlayers::ScriptListModel : public ListModel{
public:
	std::string getElementAt(int i){
        if ((i >= 0) && (i < getNumberOfElements()))
	       return settings.ais[i];
        else
            return "";
	}
	int getNumberOfElements(){
	    return settings.ais.size();
    }
};

/**
Adds a player to the list.
*/
class MenuPlayers::NewActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){
        MenuPlayers* menu = userface.mPlayers;
        settings.locals.push_back(new Local());
        menu->listPlayers->setSelected(menu->listPlayers->getListModel()->getNumberOfElements() - 1);
    }
};

/**
Deletes the player from the list.
*/
class MenuPlayers::RemoveActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent){
        MenuPlayers* menu = userface.mPlayers;
        settings.locals.erase(settings.locals.begin() + menu->listPlayers->getSelected());
        int sel = menu->listPlayers->getSelected();
        if (sel >= menu->listPlayers->getListModel()->getNumberOfElements()){
            menu->buttonRemove->setVisible(false);
            menu->contPlayer->setVisible(false);
        }
        menu->fieldName->setText(settings.locals[sel]->name);
    }
};

/**
Selects a player to modify.
*/
class MenuPlayers::PlayersSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuPlayers* menu = userface.mPlayers;
        int sel = menu->listPlayers->getSelected();
        if (sel >= 0){
            menu->buttonRemove->setVisible(true);
            menu->contPlayer->setVisible(true);
            menu->fieldName->setText(settings.locals[sel]->name);
            menu->dropAI->setSelected(settings.locals[sel]->artificial ? 1 : 0);
            menu->dropRobot->setSelected(settings.locals[sel]->robottype->id);
            if (settings.locals[sel]->artificial){
                menu->dropScript->setSelected(settings.locals[sel]->scriptid);
                menu->dropScript->setVisible(true);
                menu->labelScript->setVisible(true);
            }else{
                menu->dropScript->setVisible(false);
                menu->labelScript->setVisible(false);
            }
        }
    }
};

/**
Updates name of the selected player.
*/
class MenuPlayers::NameKeyListener : public KeyListener{
    void keyReleased (KeyEvent &keyEvent){
        MenuPlayers* menu = userface.mPlayers;
        int sel = menu->listPlayers->getSelected();
        settings.locals[sel]->name = menu->cleanname(menu->fieldName->getText());
    }
};

/**
Sets robot type the selected player.
*/
class MenuPlayers::RobotSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuPlayers* menu = userface.mPlayers;
        int sel = menu->listPlayers->getSelected();
        int robtype = menu->dropRobot->getSelected();
        settings.locals[sel]->robottype = settings.robottypes[robtype];
	}
};

/**
Sets humanity of the selected player.
*/
class MenuPlayers::AISelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuPlayers* menu = userface.mPlayers;
        int sel = menu->listPlayers->getSelected();
        int ai = menu->dropAI->getSelected();
        settings.locals[sel]->artificial = (ai == 1 ? true : false);
        if (settings.locals[sel]->artificial){
            if (settings.locals[sel]->script == ""){
                settings.locals[sel]->scriptid = settings.defaultai;
                settings.locals[sel]->script = settings.ais[settings.defaultai];
                menu->dropScript->setSelected(settings.defaultai);
            }
            menu->dropScript->setVisible(true);
            menu->labelScript->setVisible(true);
        }else{
            menu->dropScript->setVisible(false);
            menu->labelScript->setVisible(false);
        }
	}
};

/**
Sets an AI script for the selected player.
*/
class MenuPlayers::ScriptSelectionListener : public SelectionListener{
    void valueChanged(const SelectionEvent& selectionEvent){
        MenuPlayers* menu = userface.mPlayers;
        int sel  = menu->listPlayers->getSelected();
        int ssel = menu->dropScript->getSelected();
        settings.locals[sel]->scriptid = menu->dropScript->getSelected();
        if (ssel >= 0) settings.locals[sel]->script = settings.ais[ssel];
	}
};

void MenuPlayers::onLeave(){
    settings.writeLocals();
}

/**
Puts all widgets into the menu.
*/
MenuPlayers::MenuPlayers(){

    setDimension(Rectangle(MENUWIDTH / 8, MENUHEIGHT / 8, MENUWIDTH / 4 * 3, MENUHEIGHT / 4 *3));

    CustomContainer* contControl = new CustomContainer();
	contControl->setDimension
        (Rectangle(getWidth() / 24, getHeight() / 24, getWidth(), getHeight() / 8));
	add(contControl);

	buttonBack = new Button("Back");
	buttonBack->setPosition(0, 0);
	buttonBack->addActionListener(new BackActionListener());
	contControl->add(buttonBack);

	listPlayers = new ListBox(new PlayersListModel);
    listPlayers->addSelectionListener(new PlayersSelectionListener());
	ScrollArea* scrollPlayers =
        new ScrollArea(listPlayers, gcn::ScrollArea::SHOW_NEVER, gcn::ScrollArea::SHOW_ALWAYS);
	scrollPlayers->setSize(getWidth() / 3, getHeight() * 3 / 4);
	scrollPlayers->setPosition(getWidth() / 20, getHeight() / 5);
    add(scrollPlayers);
    listPlayers->setWidth(scrollPlayers->getWidth());
    CustomContainer::setColor(listPlayers);

	buttonNew = new Button(" + ");
	buttonNew->setPosition(getWidth() * 2 / 5, getHeight() / 5);
	buttonNew->addActionListener(new NewActionListener());
	add(buttonNew);

	buttonRemove = new Button(" - ");
    buttonRemove->setPosition(getWidth() * 2 / 5, getHeight() / 5 + buttonNew->getHeight() * 3 / 2);
	buttonRemove->addActionListener(new RemoveActionListener());
	buttonRemove->setVisible(false);
	add(buttonRemove);

    contPlayer = new CustomContainer();
    contPlayer->setDimension
        (Rectangle(getWidth() / 2, getHeight() / 5, getWidth() * 2 / 5, getHeight() * 4 / 5));
    contPlayer->setVisible(false);
    add(contPlayer);

    Label* labelName = new Label("Name:");
    labelName->setPosition(contPlayer->getWidth() / 2 - labelName->getWidth() / 2, 0);
    contPlayer->add(labelName);

    fieldName = new TextField("");
	fieldName->setWidth(getWidth() / 3);
    fieldName->setPosition((contPlayer->getWidth() - fieldName->getWidth()) / 2, contPlayer->getHeight() * 1 / 9 - fieldName->getHeight() / 2);
	fieldName->addKeyListener(new NameKeyListener());
	contPlayer->add(fieldName);

    Label* labelRobot = new Label("Robot:");
    labelRobot->setPosition(contPlayer->getWidth() / 2 - labelRobot->getWidth() / 2, contPlayer->getHeight() * 2 / 9);
    contPlayer->add(labelRobot);

    dropRobot = new DropDown(new RobotListModel);
    dropRobot->setWidth(getWidth() / 3);
    dropRobot->setPosition((contPlayer->getWidth() - dropRobot->getWidth()) / 2,
                           contPlayer->getHeight() * 3 / 9 - dropRobot->getHeight() / 2);
    dropRobot->addSelectionListener(new RobotSelectionListener());
    contPlayer->add(dropRobot);

    Label* labelAI = new Label("Human / AI:");
    labelAI->setPosition(contPlayer->getWidth() / 2 - labelAI->getWidth() / 2, contPlayer->getHeight() * 4 / 9);
    contPlayer->add(labelAI);

    dropAI = new DropDown(new AIListModel);
    dropAI->setWidth(getWidth() / 3);
    dropAI->setPosition((contPlayer->getWidth() - dropAI->getWidth()) / 2,
                        contPlayer->getHeight() * 5 / 9 - dropAI->getHeight() / 2);
    dropAI->addSelectionListener(new AISelectionListener());
    contPlayer->add(dropAI);

    labelScript = new Label("AI script:");
    labelScript->setPosition(contPlayer->getWidth() / 2 - labelScript->getWidth() / 2, contPlayer->getHeight() * 6 / 9);
    contPlayer->add(labelScript);

    dropScript = new DropDown(new ScriptListModel);
    dropScript->setWidth(getWidth() / 3);
    dropScript->setPosition((contPlayer->getWidth() - dropScript->getWidth()) / 2,
                            contPlayer->getHeight() * 7 / 9 - dropScript->getHeight() / 2);

    dropScript->addSelectionListener(new ScriptSelectionListener());
    dropScript->setVisible(false);
    contPlayer->add(dropScript);

    initWidgets();
}
