#ifndef WEAPONSHEADER
#define WEAPONSHEADER

#include <string>

using namespace std;

#include "global.hpp"
#include "viewable.hpp"

#include "SDL_image.h"
#include "SDL_mixer.h"

class Robot;

class ShotType{
    public:

    SDL_Surface* image[ROTCOUNT];
    SDL_Surface* icon;
    SDL_Surface* smallicon;

    Mix_Chunk* sound;
    Mix_Chunk* blowsound;

    string name;

    int energy;
    int reload;
    int speed;
    int ditch;
    int splash;
    int damage;
    int endamage;
    int bounces;
    int livetime;

    int id;

    void acquireImage(string input);

    ShotType(string name, string imgfile, int newEnergy, int newReload,
        int newSpeed, int newDitch, int newSplash, int newDamage, int newEnDamage,
        int newLivetime, int newBounces);
};

class Shot : public Viewable{
    public:

    ShotType* shottype;

    bool shot_for_deletion(Shot* shot);
    int livetime;
    int bounces;
    int imageangle;
    Robot* owner;
    bool justhit;

    virtual ~Shot();

    Shot();

    Shot(ShotType* shtype, Pointf coo, Pointf direct, int ang, Robot* own);

    void move();

    void bounce();

    bool forDeletion();

    SDL_Surface* getImage();

    int getSpeed();

    int getDitch();

    int getSplash();
};

#endif
