#ifndef MENUPLAYERSHEADER
#define MENUPLAYERSHEADER

#include "userface.hpp"

class MenuPlayers : public Menu{
	public:

	class PlayersListModel;
	class RobotListModel;
	class AIListModel;
	class ScriptListModel;

	class NewActionListener;
	class RemoveActionListener;
	class PlayersSelectionListener;
	class RobotSelectionListener;
	class AISelectionListener;
	class ScriptSelectionListener;
	class NameKeyListener;

	Button* buttonBack;
	Button* buttonNew;
	Button* buttonRemove;
	Button* buttonStartGame;

	ListBox* listPlayers;

    Icon* iconRobot;
    Image* imageRobot;
    
	CustomContainer* contPlayer;

	TextField* fieldName;
	DropDown* dropRobot;
	DropDown* dropAI;
    Label* labelScript;
    DropDown* dropScript;

	void onLeave();

	MenuPlayers();

};

#endif // MENUPLAYERSHEADER
