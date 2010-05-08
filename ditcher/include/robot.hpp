#ifndef ROBOTHEADER
#define ROBOTHEADER

#include <string>
using namespace std;

#include "viewable.hpp"
#include "players.hpp"

#include "SDL.h"
#include "SDL_image.h"

#define ROBOT_R 12
#define DEATH_R ROBOT_R * 2

class ShotType;
class RobotType;
class Player;

class Robot : public Viewable {

    private:

    int activeshots;

    public:

    static double testdist[ROBOT_R*2+1];

    Player* owner;

    SDL_Surface* statimage;
    SDL_Surface* nameimage;

    RobotType* robottype;

    static int maxShots;

    static int maxSpeed;

    static int maxHealth;
    int health;
    static int maxEnergy;
    int energy;

    static int maxGrave;
    int grave;
    static int maxRespawn;
    int respawn;

    int maxReload;
    int reload;

    unsigned int weapon;

    ActionSet actionset;

    Pointf home;

    Robot(Player* pl);

    ~Robot();

    void setName();

    void die(Robot* inflictor, ShotType* shottype);

    void everyLoop();

    void action();

    bool tryShot();

    bool removeShot();

    int getImageAngle();

    void setDirection();

    void setStat();

    SDL_Surface* getImage();

    void paintHome(Point pthome, int basetype);

    void createHome(int basetype);

    void wasHit(int damage, int endamage, Robot* inflictor, ShotType* shottype);

    bool sight(Point pt, int r);

    void setOnScreen();

    bool countSpeed(int orient);

    void displayStatus();
};
#endif
