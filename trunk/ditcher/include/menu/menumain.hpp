#ifndef MENUMAINHEADER
#define MENUMAINHEADER

#include "userface.hpp"

class MenuMain : public Menu{
	public:

	static const int buttonCount = 6;

	Button* button[buttonCount];

	class MenuCreateGameActionListener;
	class MenuConnectActionListener;
	class MenuGfxActionListener;
	class MenuPlayersActionListener;
    class MenuCreditsActionListener;
    class QuitActionListener;

	MenuMain();

};

#endif // MENUMAINHEADER
