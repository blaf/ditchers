#ifndef MENUCONNECTHEADER
#define MENUCONNECTHEADER

#include "userface.hpp"

class MenuConnect : public Menu{
	public:

	class ConnectActionListener;

	ListBox* listLocPlayers;

	TextField* fieldAddress;
	TextField* fieldPort;
	TextField* fieldName;

	Button* buttonBack;
	Button* buttonConnect;

	MenuConnect();

	void acquireData();

	void onEnter();

	void onLeave();
};

#endif // MENUCONNECTHEADER
