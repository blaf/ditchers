#ifndef MENUNETGAMESHEADER
#define MENUNETGAMESHEADER

#include "userface.hpp"

class ChatTextField : public TextField{
    public:
    void keyPressed (KeyEvent &keyEvent);
    ChatTextField();
};

class MenuNetGames : public Menu{
        public:

    class ChatTextField;

        class GamesListModel;
    class ClientsListModel;

        class ChatActionListener;
    class CreateGameActionListener;
    class JoinGameActionListener;

        class GamesSelectionListener;
    class ClientsSelectionListener;

        Button* buttonBack;

        ListBox* listGames;
        Button* buttonJoin;
        Button* buttonCreate;

    Image* imageMap;
    Icon* iconMap;

    Label* labelMap;
    Label* labelSize;
    
    ScrollArea* scrollChat;
        TextBox* textChat;
        ChatTextField* fieldChat;
        Button* buttonChat;

        ListBox* listClients;

        CustomContainer* contGame;

        void chatAction();

        void onEnter();

        MenuNetGames();
};

#endif // MENUNETGAMESHEADER
