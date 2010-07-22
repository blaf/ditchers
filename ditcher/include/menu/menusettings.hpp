#ifndef MENUSETTINGSHEADER
#define MENUSETTINGSHEADER

#include "userface.hpp"

class ContGfx : public CustomContainer{
        public:
		
        class ResolutionListModel;

        class DropResActionListener;
        class SoundActionListener;
        class FScreenActionListener;

        DropDown* dropRes;
        CheckBox* checkFullScreen;
        CheckBox* checkSound;

        ContGfx();
};

class ContControls : public CustomContainer{
	public:
	
	class SwitchControlActionListener;
	class SetKeyActionListener;
	
	Label*  labelChat;
	Button* buttonChat;
	Label*  labelFScreen;
	Button* buttonFScreen;
	Label*  labelSound;
	Button* buttonSound;
	Label*  labelQuit;
	Button* buttonQuit;
	
	Button* buttonSingle;
	Button* buttonPLeft;
	Button* buttonPRight;
	Button* buttonSpectator;
	
	CustomContainer* cPlayer;
	
	Label*  labelUp;
	Button* buttonUp;
	Label*  labelDown;
	Button* buttonDown;
	Label*  labelLeft;
	Button* buttonLeft;
	Label*  labelRight;
	Button* buttonRight;
	Label*  labelFire;
	Button* buttonFire;
	Label*  labelWeapon;
	Button* buttonWeapon;
	Label*  labelTeamChat;
	Button* buttonTeamChat;
	Label*  labelW[10];
	Button* buttonW[10];
	
	CustomContainer* cSpectator;
		
	Label*  labelSMain;
	Button* buttonSMain;
	Label*  labelSLeft;
	Button* buttonSLeft;
	Label*  labelSRight;
	Button* buttonSRight;
	Label*  labelSScreen;
	Button* buttonSScreen;
	
	int current;
	
	Button* setButton;
	int*    setKey;
	
	void setCaptions();

	ContControls();
};

class MenuSettings : public Menu{
	public:
	
	class SwitchGfxActionListener;
	class SwitchControlsActionListener;
	
	Button* buttonGfx;
	Button* buttonControls;

	ContGfx* cGfx;
	ContControls* cControls;

	Button* buttonBack;
	
	void onEnter();
	void onLeave();

	MenuSettings();
};

#endif // MENUSETTINGSHEADER
