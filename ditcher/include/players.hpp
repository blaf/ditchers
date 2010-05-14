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

    void setControls(int arg_up, int arg_down, int arg_left, int arg_right, int arg_fire, int arg_weapon,
        int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8,
        int arg_9);

    void displayIndicator(Point pt1, Point pt2, int what, bool horizontal);

    void displayStatus();

    bool setAction();

    void setMask();

    void pushMask(int timestamp, int keybmask);
};

#endif
