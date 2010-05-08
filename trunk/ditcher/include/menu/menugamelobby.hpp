#ifndef MENUGAMELOBBY
#define MENUGAMELOBBY

#include "userface.hpp"

class MenuGameLobby : public Menu{
        public:

        class ChatTextField;

        class PlayersListModel;
        class LocalPlayersListModel;
        class TeamsListModel;
        class TeamListModel;

        class ChatActionListener;
        class StartGameActionListener;
        class AddPlayerActionListener;
        class RemovePlayerActionListener;

        class PlayersSelectionListener;
        class TeamsSelectionListener;
        class TeamSelectionListener;

        class LimitSliderActionListener;
        class LimitLeftActionListener;
        class LimitRightActionListener;

        DropDown* dropTeams;
        DropDown* dropTeam;

        Slider* sliderLimit;
        Button* buttonLimitLeft;
        Button* buttonLimitRight;
        TextField* fieldLimit;

        Button* buttonBack;

        ListBox* listPlayers;
        ListBox* listLocalPlayers;

        Button* buttonAddPlayer;
        Button* buttonRemovePlayer;

        ScrollArea* scrollChat;
        TextBox* textChat;
        ChatTextField* fieldChat;
        Button* buttonChat;

        Button* buttonStartGame;

        void chatAction();

        void onEnter();
        void onLeave();

        void update();

        void limitSliderAction(int inc);

        MenuGameLobby();
};

#endif // MENUGAMELOBBY
