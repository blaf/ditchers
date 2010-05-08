#include "menu/menucredits.hpp"

MenuCredits::MenuCredits(){
    setDimension(Rectangle(MENUWIDTH / 4, MENUHEIGHT / 6, MENUWIDTH / 2, MENUHEIGHT * 2 / 3));

    int labels = 6;
    int steps = 2*(labels+1);
    Label* label[labels];
    label[0] = new Label("Ditchers");
    label[1] = new Label("version 1.0");
    label[2] = new Label("Programming & Design:");
    label[3] = new Label("David Slaby");
    label[4] = new Label("Graphics:");
    label[5] = new Label("Alzbeta Slaba");

    for (int i = 0; i < labels; i++){
        label[i]->setPosition(getWidth() / 2 - label[i]->getWidth() / 2,
                              getHeight() / steps + i*(getHeight() / (steps / 2)) - (i % 2) * getHeight() / (steps * 3 / 2) - label[i]->getHeight() / 2);
        add(label[i]);
    }

    buttonBack = new Button("Back");
    buttonBack->setPosition(getWidth() / 2 - buttonBack->getWidth() / 2,
                            getHeight() * (steps-1) / steps - buttonBack->getHeight() / 2);
    buttonBack->addActionListener(new BackActionListener());
    add(buttonBack);

    initWidgets();
}