#ifndef MENUCREATEGAMEHEADER
#define MENUCREATEGAMEHEADER

#include "userface.hpp"

class MenuCreateGame : public Menu{
	public:

	class MapListModel;

	class NextActionListener;

    class MapSelectionListener;

    TextField* fieldName;
	DropDown* dropMap;
    Image* imageMap;
    Icon* iconMap;
    Label* labelSize;
    Label* labelPlayers;
    Button* buttonBack;
	Button* buttonNext;

	MenuCreateGame();

    void onEnter();

    void setMapImage();
};

#endif // MENUCREATEGAMEHEADER
