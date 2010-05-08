#ifndef USERSHEADER
#define USERSHEADER

#define BUFSIZE 1024

#define MAXGAMES    128
#define MAXCLIENTS  1024
#define MAXPLAYERS  64

#define NAMLEN 32

#include <map>
#include <vector>
#include <stack>

#include "SDL_net.h"

#include "template.hpp"

using namespace std;

class Game;
class Games;
class Player;
class Players;
class Client;
class Clients;


class Player{
    public:
    Client* owner;
    Game*   game;
    string  name;
    string  robottype;
    int     id;
    int     index;
    int     artificial;
    int     team;

    Player();
};

class Players : public WrapBoth<Player*>{
    public:

    Game*   game;

    Players();

    Player* add(int index, Client* owner);

    void remCl(int clid);

    bool removeAll(Client* remOwner);
};

class Game{
    public:
    string name;
    string mapname;
    string maphash;
    unsigned int id;
    map<int, Client*> clients;
    Players players;
    int blob;
    int maxplayers;
    int limit;
    int resx;
    int resy;
    int teams;
    bool started;

    vector<double>* sync;
    unsigned int synced;

    Game();

    void remove();

    void removeClient(int clid);

    void removePlayer(int plid, int clid);

    bool start();

    void countRes();

    bool syncCheck(int clid, int timestamp, int plid, double coordx, double coordy);

    void setBlob();
};

class Games : public WrapMap<Game*>{
    public:

    Games();

    Game* add();

    void remove(int remid);

}games;

class Client{
    public:

    enum State{ CHOOSE, JOIN, WAIT, PLAY };

    TCPsocket   sd;
    bool        sdopen;
    int         id;
    string      name;
    State       state;
    Game*       game;
    map<int, Player*> players;

    int     resx;
    int     resy;
    int     gameresx;
    int     gameresy;

    Client(TCPsocket newsd);

    void setGame(Game* newGame);

    bool setGame(int gameid);

    void broadCast(bool except);

    void singleCast();

    void cast();

    Player* addPlayer(int index);

    void clearPlayers();

    void countRes();
};

class Clients : public WrapMap<Client*>{
    public:

    iterator readyiter;

    Clients();

    bool add(TCPsocket newsd);

    bool remove(int remid);

    Client* beginReady();

    Client* nextReady();

    bool endReady();

}clients;

#endif // USERSHEADER
