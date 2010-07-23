#include "menu/menusettings.hpp"
#include "global.hpp"
#include "settings.hpp"
#include "gfx.hpp"
#include "game.hpp"
#include "keynames.hpp"

    /* *** MenuSettings *** */

/**
Switches to graphics settings.
*/
class MenuSettings::SwitchGfxActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        userface.mSettings->cControls->setVisible(false);
        userface.mSettings->cGfx->setVisible(true);
    }
};

/**
Switches to controls settings.
*/
class MenuSettings::SwitchControlsActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        userface.mSettings->cGfx->setVisible(false);
        userface.mSettings->cControls->setVisible(true);
    }
};

/**
Switches fullscreen / windowed video mode.
*/
class ContGfx::FScreenActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        gfx.fullscreen=!gfx.fullscreen;
        gfx.setVideoMode();
        userface.refreshBkg();
    }
};

/**
Switches sound on/off.
*/
class ContGfx::SoundActionListener : public ActionListener{
    void action(const ActionEvent& actionEvent) {
        settings.sound=!settings.sound;
    }
};

/**
Changes graphics resolution.
*/
class ContGfx::DropResActionListener: public ActionListener{
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
                userface.mSettings->cGfx->checkFullScreen->setSelected(gfx.fullscreen);
            }
            userface.refreshBkg();
            userface.setContainer();
        }
    }
};

/**
Contains resolution options.
*/
class ContGfx::ResolutionListModel: public gcn::ListModel{
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

class ContControls::SwitchControlActionListener: public ActionListener{
    void action(const ActionEvent& actionEvent) {
	
		int con = 0;
		
		Widget* source = actionEvent.getSource();		
		ContControls* cCont = userface.mSettings->cControls;
		
		if (source == cCont->buttonSingle) con = 0;
		else if (source == cCont->buttonPLeft) con = 1;
		else if (source == cCont->buttonPRight) con = 2;
		else if (source == cCont->buttonSpectator) con = 3;
		
		cCont->current = con;
		cCont->setCaptions();
		
		if (con == 3){
			cCont->cPlayer->setVisible(false);
			cCont->cSpectator->setVisible(true);
		}else{
			cCont->cPlayer->setVisible(true);
			cCont->cSpectator->setVisible(false);
		}
	}
};

class ContControls::SetKeyActionListener: public ActionListener{
    void action(const ActionEvent& actionEvent) {
	
		Button* source = (Button*)actionEvent.getSource();
		
		ContControls* cCont = userface.mSettings->cControls;
		
		cCont->setButton = source;
		source->setCaption("? ? ?");
		source->adjustSize();
		
		if (source == cCont->buttonChat)
			cCont->setKey = &(settings.controls.chat);
		else if (source == cCont->buttonQuit)
			cCont->setKey = &(settings.controls.quit);
		else if (source == cCont->buttonSound)
			cCont->setKey = &(settings.controls.sound);
		else if (source == cCont->buttonFScreen)
			cCont->setKey = &(settings.controls.fscreen);
		else if (cCont->current < 3){
			PlayerControl* pc;
			
			if (cCont->current == 0) pc = &settings.controls.single;
			else if (cCont->current == 1) pc = &settings.controls.left;
			else if (cCont->current == 2) pc = &settings.controls.right;
			
			if (source == cCont->buttonUp)
				cCont->setKey = &(pc->up);
			else if (source == cCont->buttonDown)
				cCont->setKey = &(pc->down);
			else if (source == cCont->buttonLeft)
				cCont->setKey = &(pc->left);
			else if (source == cCont->buttonRight)
				cCont->setKey = &(pc->right);
			else if (source == cCont->buttonFire)
				cCont->setKey = &(pc->fire);
			else if (source == cCont->buttonWeapon)
				cCont->setKey = &(pc->weapon);
			else if (source == cCont->buttonTeamChat)
				cCont->setKey = &(pc->teamchat);
			else for (int i = 0; i < 10; i++) if (source == cCont->buttonW[i])
				cCont->setKey = &(pc->w[i]);
		}else{
			SpectatorControl* sc = &settings.controls.spectator;
			
			if (source == cCont->buttonSMain)
				cCont->setKey = &(sc->main);
			else if (source == cCont->buttonSRight)
				cCont->setKey = &(sc->right);
			else if (source == cCont->buttonSLeft)
				cCont->setKey = &(sc->left);
			else if (source == cCont->buttonSScreen)
				cCont->setKey = &(sc->split);
		}
		
		userface.setkey = true;
    }
};

void ContControls::setCaptions(){

	buttonQuit->setCaption(keyname(settings.controls.quit));buttonQuit->adjustSize();
	buttonChat->setCaption(keyname(settings.controls.chat));buttonChat->adjustSize();
	buttonFScreen->setCaption(keyname(settings.controls.fscreen));buttonFScreen->adjustSize();
	buttonSound->setCaption(keyname(settings.controls.sound));buttonSound->adjustSize();
	
	if (current == 0)
		{ labelControl->setCaption("Single Player:"); labelControl->adjustSize(); }
	else if (current == 1)
		{ labelControl->setCaption("Left Player:"); labelControl->adjustSize(); }
	else if (current == 2)
		{ labelControl->setCaption("Right Player:"); labelControl->adjustSize(); }
	else if (current == 3)
		{ labelControl->setCaption("Spectator Mode:"); labelControl->adjustSize(); }
	
	if (current < 3){
		PlayerControl* pc;
		if (current == 0) pc = &settings.controls.single;
		else if (current == 1) pc = &settings.controls.left;
		else if (current == 2) pc = &settings.controls.right;
		
		buttonUp->setCaption(keyname(pc->up));buttonUp->adjustSize();
		buttonDown->setCaption(keyname(pc->down));buttonDown->adjustSize();
		buttonLeft->setCaption(keyname(pc->left));buttonLeft->adjustSize();
		buttonRight->setCaption(keyname(pc->right));buttonRight->adjustSize();
		
		buttonFire->setCaption(keyname(pc->fire));buttonFire->adjustSize();
		buttonWeapon->setCaption(keyname(pc->weapon));buttonWeapon->adjustSize();
		buttonTeamChat->setCaption(keyname(pc->teamchat));buttonTeamChat->adjustSize();

		for (int i = 0; i < 10; i++){
			buttonW[i]->setCaption(keyname(pc->w[i]));buttonW[i]->adjustSize();
		}
		
	}else{
		SpectatorControl* sc = &settings.controls.spectator;
		
		buttonSMain->setCaption(keyname(sc->main));buttonSMain->adjustSize();
		buttonSRight->setCaption(keyname(sc->right));buttonSRight->adjustSize();
		buttonSLeft->setCaption(keyname(sc->left));buttonSLeft->adjustSize();
		buttonSScreen->setCaption(keyname(sc->split));buttonSScreen->adjustSize();
	}
}

void MenuSettings::onEnter(){
    int ressel;
    switch(settings.gfxres.x){
        case 640:  ressel = 0; break;
        case 800:  ressel = 1; break;
        case 1024: ressel = 2; break;
        case 1280: ressel = 3; break;
		default: ressel = 1;
    }
    cGfx->dropRes->setSelected(ressel);
    cGfx->checkFullScreen->setSelected(gfx.fullscreen);
    cGfx->checkSound->setSelected(settings.sound);
	
	cControls->setCaptions();
}

void MenuSettings::onLeave(){
    settings.gfxres = gfx.res;
    settings.fullscreen = gfx.fullscreen;
    settings.writeSettings();
    settings.writeControls();
}

/**
Puts all widgets into the menu.
*/
ContGfx::ContGfx(){

    setDimension(Rectangle(MENUWIDTH / 6, MENUHEIGHT / 6, MENUWIDTH * 2 / 3, MENUHEIGHT * 2 / 3));

    Label* labelRes = new Label("Resolution:");
    labelRes->setPosition(getWidth() / 2 - labelRes->getWidth() / 2,
        getHeight() * 2 / 11 - labelRes->getHeight() / 2);
    add(labelRes);

    dropRes = new DropDown(new ResolutionListModel());
    dropRes->addActionListener(new DropResActionListener());
    dropRes->setPosition(getWidth() / 2 - dropRes->getWidth() / 2,
        getHeight() * 3 / 11 - dropRes->getHeight() / 2);
    add(dropRes);
    
    checkFullScreen = new CheckBox("Fullscreen");
    checkFullScreen->setPosition(getWidth() / 2 - checkFullScreen->getWidth() / 2,
        getHeight() * 5 / 11 - checkFullScreen->getHeight() / 2);
    checkFullScreen->addActionListener(new FScreenActionListener());
    add(checkFullScreen);
    
    checkSound = new CheckBox("Sound");
    checkSound->setPosition(getWidth() / 2 - checkSound->getWidth() / 2,
        getHeight() * 7 / 11 - checkSound->getHeight() / 2);
    checkSound->addActionListener(new SoundActionListener());
    add(checkSound);

}

/**
Puts all widgets into the menu.
*/
ContControls::ContControls(){

	current = 0;

    setDimension(Rectangle(MENUWIDTH, MENUHEIGHT / 12, MENUWIDTH, MENUHEIGHT * 11 / 12));
	
	labelChat = new Label("Chat: ");
    labelChat->setPosition(getWidth() * 1 / 4 - labelChat->getWidth(),
        getHeight() * 1 / 12);
    add(labelChat);
	
	buttonChat = new Button("Chat");
    buttonChat->setPosition(getWidth() * 1 / 4,
        labelChat->getY()+labelChat->getHeight()/2-buttonChat->getHeight()/2);
    buttonChat->addActionListener(new SetKeyActionListener());
    add(buttonChat);
	
	labelQuit = new Label("Quit: ");
    labelQuit->setPosition(getWidth() * 3 / 4 - labelQuit->getWidth(),
        getHeight() * 1 / 12);
    add(labelQuit);
	
	buttonQuit = new Button("Quit");
    buttonQuit->setPosition(getWidth() * 3 / 4,
        labelQuit->getY()+labelQuit->getHeight()/2-buttonQuit->getHeight()/2);
    buttonQuit->addActionListener(new SetKeyActionListener());
    add(buttonQuit);

	labelFScreen = new Label("Fullscreen: ");
    labelFScreen->setPosition(getWidth() * 1 / 4 - labelFScreen->getWidth(),
        getHeight() * 2 / 12);
    add(labelFScreen);
	
	buttonFScreen = new Button("Fullscreen");
    buttonFScreen->setPosition(getWidth() * 1 / 4,
        labelFScreen->getY()+labelFScreen->getHeight()/2-buttonFScreen->getHeight()/2);
    buttonFScreen->addActionListener(new SetKeyActionListener());
    add(buttonFScreen);
	
	labelSound = new Label("Sound: ");
    labelSound->setPosition(getWidth() * 3 / 4 - labelSound->getWidth(),
        getHeight() * 2 / 12);
    add(labelSound);
	
	buttonSound = new Button("Sound");
    buttonSound->setPosition(getWidth() * 3 / 4,
        labelSound->getY()+labelSound->getHeight()/2-buttonSound->getHeight()/2);
    buttonSound->addActionListener(new SetKeyActionListener());
    add(buttonSound);
	
	cPlayer = new CustomContainer();
	cPlayer->setDimension(Rectangle(0,getHeight()/3,getWidth(),getHeight()*2/3));
	add(cPlayer);
	
	labelUp = new Label("Up: ");
    labelUp->setPosition(getWidth() * 1 / 6 - labelUp->getWidth(),
        getHeight() * 1 / 12);
    cPlayer->add(labelUp);
	
	buttonUp = new Button("Up");
    buttonUp->setPosition(getWidth() * 1 / 6,
        labelUp->getY()+labelUp->getHeight()/2-buttonUp->getHeight()/2);
    buttonUp->addActionListener(new SetKeyActionListener());
    cPlayer->add(buttonUp);
	
	labelDown = new Label("Down: ");
    labelDown->setPosition(getWidth() * 1 / 6 - labelDown->getWidth(),
        getHeight() * 2 / 12);
    cPlayer->add(labelDown);
	
	buttonDown = new Button("Down");
    buttonDown->setPosition(getWidth() * 1 / 6,
        labelDown->getY()+labelDown->getHeight()/2-buttonDown->getHeight()/2);
    buttonDown->addActionListener(new SetKeyActionListener());
    cPlayer->add(buttonDown);
	
	labelLeft = new Label("Left: ");
    labelLeft->setPosition(getWidth() * 1 / 6 - labelLeft->getWidth(),
        getHeight() * 3 / 12);
    cPlayer->add(labelLeft);
	
	buttonLeft = new Button("Left");
    buttonLeft->setPosition(getWidth() * 1 / 6,
        labelLeft->getY()+labelLeft->getHeight()/2-buttonLeft->getHeight()/2);
    buttonLeft->addActionListener(new SetKeyActionListener());
    cPlayer->add(buttonLeft);
	
	labelRight = new Label("Right: ");
    labelRight->setPosition(getWidth() * 1 / 6 - labelRight->getWidth(),
        getHeight() * 4 / 12);
    cPlayer->add(labelRight);
	
	buttonRight = new Button("Right");
    buttonRight->setPosition(getWidth() * 1 / 6,
        labelRight->getY()+labelRight->getHeight()/2-buttonRight->getHeight()/2);
    buttonRight->addActionListener(new SetKeyActionListener());
    cPlayer->add(buttonRight);
	
	labelFire = new Label("Fire: ");
    labelFire->setPosition(getWidth() * 1 / 6 - labelFire->getWidth(),
        getHeight() * 5 / 12);
    cPlayer->add(labelFire);
	
	buttonFire = new Button("Fire");
    buttonFire->setPosition(getWidth() * 1 / 6,
        labelFire->getY()+labelFire->getHeight()/2-buttonFire->getHeight()/2);
    buttonFire->addActionListener(new SetKeyActionListener());
    cPlayer->add(buttonFire);
	
	labelWeapon = new Label("Weapon: ");
    labelWeapon->setPosition(getWidth() * 1 / 6 - labelWeapon->getWidth(),
        getHeight() * 6 / 12);
    cPlayer->add(labelWeapon);
	
	buttonWeapon = new Button("Weapon");
    buttonWeapon->setPosition(getWidth() * 1 / 6,
        labelWeapon->getY()+labelWeapon->getHeight()/2-buttonWeapon->getHeight()/2);
    buttonWeapon->addActionListener(new SetKeyActionListener());
    cPlayer->add(buttonWeapon);

	for (int i = 0; i < 10; i++){
		labelW[i] = new Label("Weapon "+intToString(i+1)+": ");
		labelW[i]->setPosition(getWidth() * (3+((i/6)*2)) / 6 - labelW[i]->getWidth(),
			getHeight() * ((i%6)+1) / 12);
		cPlayer->add(labelW[i]);
		
		buttonW[i] = new Button("Weapon "+intToString(i+1));
		buttonW[i]->setPosition(getWidth() * (3+((i/6)*2)) / 6,
			labelW[i]->getY()+labelW[i]->getHeight()/2-buttonW[i]->getHeight()/2);
		buttonW[i]->addActionListener(new SetKeyActionListener());
		cPlayer->add(buttonW[i]);
	}
	labelW[9]->setVisible(false);
	buttonW[9]->setVisible(false);
	
	labelTeamChat = new Label("Team chat: ");
    labelTeamChat->setPosition(getWidth() * 5 / 6 - labelTeamChat->getWidth(),
        getHeight() * 6 / 12);
    cPlayer->add(labelTeamChat);
	
	buttonTeamChat = new Button("TeamChat");
    buttonTeamChat->setPosition(getWidth() * 5 / 6,
        labelTeamChat->getY()+labelTeamChat->getHeight()/2-buttonTeamChat->getHeight()/2);
    buttonTeamChat->addActionListener(new SetKeyActionListener());
    cPlayer->add(buttonTeamChat);
	
	cSpectator = new CustomContainer();
	cSpectator->setDimension(Rectangle(0,getHeight()/3,getWidth(),getHeight()*2/3));
	cSpectator->setVisible(false);
	add(cSpectator);
	
	labelSMain = new Label("Main View: ");
    labelSMain->setPosition(getWidth() * 1 / 4 - labelSMain->getWidth(),
        getHeight() * 2 / 12);
    cSpectator->add(labelSMain);
	
	buttonSMain = new Button("Main View");
    buttonSMain->setPosition(getWidth() * 1 / 4,
        labelSMain->getY()+labelSMain->getHeight()/2-buttonSMain->getHeight()/2);
    buttonSMain->addActionListener(new SetKeyActionListener());
    cSpectator->add(buttonSMain);
	
	labelSRight = new Label("Right View: ");
    labelSRight->setPosition(getWidth() * 3 / 4 - labelSRight->getWidth(),
        getHeight() * 2 / 12);
    cSpectator->add(labelSRight);
	
	buttonSRight = new Button("Right View");
    buttonSRight->setPosition(getWidth() * 3 / 4,
        labelSRight->getY()+labelSRight->getHeight()/2-buttonSRight->getHeight()/2);
    buttonSRight->addActionListener(new SetKeyActionListener());
    cSpectator->add(buttonSRight);

	labelSLeft = new Label("Left View: ");
    labelSLeft->setPosition(getWidth() * 1 / 4 - labelSLeft->getWidth(),
        getHeight() * 4 / 12);
    cSpectator->add(labelSLeft);
	
	buttonSLeft = new Button("Left View");
    buttonSLeft->setPosition(getWidth() * 1 / 4,
        labelSLeft->getY()+labelSLeft->getHeight()/2-buttonSLeft->getHeight()/2);
    buttonSLeft->addActionListener(new SetKeyActionListener());
    cSpectator->add(buttonSLeft);
	
	labelSScreen = new Label("Split Screen: ");
    labelSScreen->setPosition(getWidth() * 3 / 4 - labelSScreen->getWidth(),
        getHeight() * 4 / 12);
    cSpectator->add(labelSScreen);
	
	buttonSScreen = new Button("SplitScreen");
    buttonSScreen->setPosition(getWidth() * 3 / 4,
        labelSScreen->getY()+labelSScreen->getHeight()/2-buttonSScreen->getHeight()/2);
    buttonSScreen->addActionListener(new SetKeyActionListener());
    cSpectator->add(buttonSScreen);
	
    buttonSpectator = new Button("Spectator");
    buttonSpectator->setPosition(getWidth()*11/12-buttonSpectator->getWidth(),
        getHeight() * 7 / 24);
    buttonSpectator->addActionListener(new SwitchControlActionListener());
    add(buttonSpectator);
	
    buttonPRight = new Button("Right");
    buttonPRight->setPosition(buttonSpectator->getX()-buttonPRight->getWidth(),
        getHeight() * 7 / 24);
    buttonPRight->addActionListener(new SwitchControlActionListener());
    add(buttonPRight);
	
    buttonPLeft = new Button("Left");
    buttonPLeft->setPosition(buttonPRight->getX()-buttonPLeft->getWidth(),
        getHeight() * 7 / 24);
    buttonPLeft->addActionListener(new SwitchControlActionListener());
    add(buttonPLeft);
	
	buttonSingle = new Button("Single");
    buttonSingle->setPosition(buttonPLeft->getX()-buttonSingle->getWidth(),
        getHeight() * 7 / 24);
    buttonSingle->addActionListener(new SwitchControlActionListener());
    add(buttonSingle);
	
	labelControl = new Label("Single Player:");
	labelControl->setPosition(getWidth()*1/12,
		buttonSingle->getY()+buttonSingle->getHeight()/2-labelControl->getHeight()/2);
	add(labelControl);
}

/**
Puts all widgets into the menu.
*/
MenuSettings::MenuSettings(){

    setDimension(Rectangle(0, 0, MENUWIDTH, MENUHEIGHT));

	cGfx = new ContGfx();
	add(cGfx);
	cGfx->setVisible(true);
	cGfx->setPosition(getWidth() / 2 - cGfx->getWidth() / 2,
		getHeight() / 2 + getHeight() / 12 - cGfx->getHeight() / 2);
	
	cControls = new ContControls();
	add(cControls);
	cControls->setVisible(false);
	cControls->setPosition(getWidth() / 2 - cControls->getWidth() / 2,
		getHeight() / 2 + getHeight() / 12 - cControls->getHeight() / 2);
	
    buttonGfx = new Button("Graphics");
    buttonGfx->setPosition(getWidth() * 1 / 12,
        getHeight() * 1 / 12);
    buttonGfx->addActionListener(new SwitchGfxActionListener());
    add(buttonGfx);
	
    buttonControls = new Button("Controls");
    buttonControls->setPosition(buttonGfx->getX()+buttonGfx->getWidth(),
        getHeight() * 1 / 12);
    buttonControls->addActionListener(new SwitchControlsActionListener());
    add(buttonControls);
	
    Button* buttonBack = new Button("Back");
    buttonBack->setPosition(getWidth() * 11 / 12 - buttonBack->getWidth(),
        getHeight() * 1 / 12);
    buttonBack->addActionListener(new BackActionListener());
    add(buttonBack);

}