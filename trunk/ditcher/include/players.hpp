#ifndef PLAYERSHEADER
#define PLAYERSHEADER

class Robot;
class RobotType;
class GameView;
class AI;

#include <queue>
#include <vector>
#include <string>

#include "point.hpp"

using namespace std;

#define MAXROBOTS 16

struct ActionSet {
    int u;
    int d;
    int l;
    int r;
    int fire;
    int weapon;
};
struct KeySet : ActionSet {
    int w[9];
};

class Team{
    public:
    int players;
    int points;
    int deaths;
    int id;

    Team(int newid);
};

class Player{
    public:
    int     id;
    int     order;
    int     index;
    bool    local;
    bool    human;
    int     teamid;
    string  script;
    Team*   team;
    AI*     ai;
    int     actionmask;
    string  name;

    unsigned int letterlimit;

    Robot*  robot;
    RobotType*  robottype;
    KeySet*  controlset;
    unsigned int weaponlock;
    std::queue<int> keybmasks;
    int     laststamp;

    int     points;
    int     deaths;

    GameView* gameview;

    bool spectated;

    Player();

    ~Player();

    void initAI(string file, string path);

    int startAI();

    void setControls(int whese);

    void displayIndicator(Point pt1, Point pt2, int what, bool horizontal);

    void displayStatus();

    bool setAction();

    void setMask();

    static void* setMaskThread(void* arg)
	 {
		 Player* pl = (Player*)arg;
		 pl->setMask();
		 return NULL;
	 }

    void pushMask(int timestamp, int keybmask);
};

#endif
