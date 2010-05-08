#include "userface.hpp"
#include "global.hpp"
#include "network.hpp"
#include "game.hpp"

#include "guichan/sdl.hpp"

#include <cstring>

/**
Sets widgets color in the container.
*/
void CustomContainer::initWidgets(){
    for (WidgetListIterator it = mWidgets.begin(); it != mWidgets.end(); it++){
        setColor(*it);
        CustomContainer* widg = dynamic_cast<CustomContainer*>(*it);
        if (widg) widg->initWidgets();
    }
}

/**
Set color of the given widget.
*/
void CustomContainer::setColor(gcn::Widget* widget){
    DropDown* widg = dynamic_cast<DropDown*>(widget);
    if (widg!=NULL){
        widg->setBaseColor(Color(0x886644));
        widg->setForegroundColor(Color(0xffff00));
        widg->setBackgroundColor(Color(0x442200));
    }else{
        widget->setBaseColor(Color(0x775533));
        widget->setForegroundColor(Color(0xffff00));
        widget->setBackgroundColor(Color(0x442200));
    }
}

    /* *** Menu *** */

/**
Removes whitespaces from the string.
*/
string Menu::cleanname(string dirtyname){
    string str = dirtyname;
    for (int i = 0; i < (int)str.length(); i++)
        if ((str[i] == ' ') || (str[i] == '\t') || (str[i] == '\n')) {
            str.erase(i, 1);
            i--;
        }
    return str;
}

/**
Returns to the preceding menu of GUI.
*/
void Menu::BackActionListener::action(const ActionEvent& actionEvent) {
    userface.switchTo(userface.current->preceding);
}

/**
Checks whether a listbox has its selected item within items range.
*/
bool Menu::inRange(ListBox* listbox){
    return ((listbox->getSelected() >= 0)
        && (listbox->getSelected() < listbox->getListModel()->getNumberOfElements()));
}

/**
Checks whether a dropdown has its selected item within items range.
*/
bool Menu::inRange(DropDown* dropdown){
    return ((dropdown->getSelected() >= 0)
        && (dropdown->getSelected() < dropdown->getListModel()->getNumberOfElements()));
}

/**
Initializes the menu.
*/
void Menu::init(Container* par){
    parent = par;
    parent->add(this);

    setVisible(false);

    setColor(this);

    preceding = NULL;
}

void Menu::onEnter(){};

void Menu::onLeave(){};

void Menu::update(){};

Menu::Menu(){
    init(userface.container);
}

Menu::Menu(Container* par){
    init(par);
}

    /* *** MsgBox *** */

/**
Sets the displayed text and resizes the menu.
*/
void MsgBox::setText(const string msg){
    userface.current->setEnabled(false);
    labelMessage->setCaption(msg);
    labelMessage->adjustSize();
    setSize(labelMessage->getWidth() * 6 / 5, MENUHEIGHT / 3);

    setPosition(userface.container->getWidth() / 2 - getWidth() / 2,
        userface.container->getHeight() / 2 - getHeight() / 2);

    labelMessage->setPosition
        (getWidth() / 2 - labelMessage->getWidth() / 2,
        getHeight() / 3 - labelMessage->getHeight() / 2);

    buttonOK->setPosition
        (getWidth() / 2 - buttonOK->getWidth() / 2,
        getHeight() / 3 * 2 - buttonOK->getHeight() / 2);
}

/**
Initializes all widgets in the message box.
*/
MsgBox::MsgBox(){

    backActionListener = new BackActionListener();

    labelMessage = new Label("");
    add(labelMessage);

    setSize(labelMessage->getWidth() * 6 / 5, MENUHEIGHT / 3);

    setPosition(userface.container->getWidth() / 2 - getWidth() / 2,
        userface.container->getHeight() / 2 - getHeight() / 2);

    labelMessage->setPosition
        (getWidth() / 2 - labelMessage->getWidth() / 2,
        getHeight() / 3 - labelMessage->getHeight() / 2);

    buttonOK = new Button("OK");
    buttonOK->setWidth(buttonOK->getWidth()*2);
    buttonOK->setPosition
        (getWidth() / 2 - buttonOK->getWidth() / 2,
        getHeight() / 3 * 2 - buttonOK->getHeight() / 2);
    buttonOK->addActionListener(backActionListener);
    add(buttonOK);

    initWidgets();
}
