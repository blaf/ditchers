#ifndef GUIHEADER
#define GUIHEADER

#include "basic.hpp"
#include "guichan.hpp"

#include <string>
using namespace std;

#include "SDL_image.h"

#define MENUWIDTH 640
#define MENUHEIGHT 480

using namespace gcn;

class CustomContainer : public Container {
    public:

    void initWidgets();

    static void setColor(gcn::Widget* widget);
};

class Menu : public CustomContainer {
    protected:

    Container* parent;

    void init(Container* par);

    public:

	Menu* preceding;

	class BackActionListener;

	Menu();

	Menu(Container* parent);

    virtual void onEnter();

	virtual void onLeave();

	virtual void update();

	static bool inRange(ListBox* listbox);

	static bool inRange(DropDown* dropdown);

	static string cleanname(string dirtyname);
};

class Menu::BackActionListener : public ActionListener{
	void action(const ActionEvent& actionEvent);
};


class MsgBox : public Menu{
    public:

    ActionListener* backActionListener;

    Label* labelMessage;
    Button* buttonOK;

    void setText(string msg);

    MsgBox();
};

class MenuMain;
class MenuCreateGame;
class MenuPlayers;
class MenuSettings;
class MenuConnect;
class MenuNetGames;
class MenuGameLobby;
class MenuCredits;

class UserFace{
    protected:

    Gui* gui;

    void createMenus();
    
    void getBkgImage();
    
    MsgBox* msgBox;
    
    public:

	bool quit;

	CustomContainer* container;

    Menu* current;

	MenuMain* mMain;
	MenuCreateGame* mCreateGame;
	MenuSettings* mSettings;
	MenuPlayers* mPlayers;
	MenuConnect* mConnect;
	MenuNetGames* mNetGames;
	MenuGameLobby* mGameLobby;
    MenuCredits* mCredits;
	
	bool setkey;

    SDL_Surface* splashimg;
    SDL_Surface* bkg;
	Point bkgLoc;

	string message;

    Image* imageMap;

	void init();

    void splash();
    
    void theGui();

    void setContainer();
    
    void switchTo(Menu* newMenu);

	void showMessage(string msg = "");

	void hideAll();

	void refreshBkg();

	void updateChat(string message, int chatlevel);

	void launchGame();

    UserFace();
};

#endif
