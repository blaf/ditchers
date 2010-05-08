#ifndef USERSHEADER
#define USERSHEADER

#include <map>
#include <string>

using namespace std;

class Game;
class Games;
class PrePlayer;
class PrePlayers;
class Client;
class Clients;
class RobotType;

#include "template.hpp"

class Game{
	public:
	string name;
	string mapname;
    string maphash;
    unsigned int id;
	map<int, Client*> clients;

	Game();
};

class Games : public WrapMap<Game*>{
    public:

    Games();

    Game* add();

    Game* add(int newid);

    void remove(int remid);

    Game* atIndex(int index);
};

class PrePlayer{
    public:
    Client* owner;
    Game*   game;
    RobotType* robottype;
    string  name;
    int     team;

    int     local;

    int     id;
    int     index;

    PrePlayer();

    bool setOwner(int ownid);
    int getTeam();
};

class PrePlayers : public WrapBoth<PrePlayer*>{
    public:

    unsigned int provider;

    PrePlayers();

    PrePlayer* add(int newid, int index);

    unsigned int size();

    void clear();
};

class Client{
    public:

    enum State{ CHOOSE, JOIN, WAIT, PLAY };

    int         id;
    string      name;
    State       state;
    Game*       game;

    Client();
};

class Clients : public WrapMap<Client*>{
    public:

    iterator readyiter;

    Clients();

    Client* add();

    Client* add(int newid);

    bool remove(int remid);

    Client* atIndex(int index);
};


#endif // USERSHEADER
