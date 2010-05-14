#include "weapons.hpp"
#include "basic.hpp"
#include "global.hpp"
#include "game.hpp"
#include "players.hpp"
#include "robot.hpp"
#include "gfx.hpp"
#include "settings.hpp"
#include <iostream>
#include <cmath>

#include "SDL_rotozoom.h"
/**
Loads image set of the shot type.
*/
void ShotType::acquireImage(string input){
    SDL_Surface* imageLoad = gfx.loadImage(settings.loc_weaponsdir + "/" + input + ".png");
    gfx.createRotated(imageLoad, image, ROTCOUNT);
    SDL_FreeSurface(imageLoad);
    icon = gfx.loadImage(settings.loc_weaponsiconsdir + "/" + input + ".png");
    smallicon = rotozoomSurface(icon, 0, 0.5, 1);
}

/**
Initializes shot type with given parameters.
*/
ShotType::ShotType(string newName, string imgfile, int newEnergy, int newReload,
    int newSpeed, int newDitch, int newSplash, int newDamage, int newEnDamage,
    int newLivetime, int newBounces){

    acquireImage(imgfile);

    name     = newName;

    energy   = newEnergy;
    reload   = newReload;
    speed    = newSpeed;
    ditch    = newDitch;
    splash   = newSplash;
    damage   = newDamage;
    endamage = newEnDamage;
    bounces  = newBounces;
    livetime = newLivetime;

    id       = gameplay.shottypes.size();
}

Shot::~Shot(){
    owner->removeShot();
}

/**
Creates a new shot.
*/
Shot::Shot(ShotType* shtype, Pointf coo, Pointf direct, int ang, Robot* own){
    shottype = shtype;

    bounces  = shtype->bounces;
    livetime = shtype->livetime;

    justhit = false;
    coords = coo;
    direction = direct;
    angle = ang;
    owner = own;

    imageangle = ((angle+gameplay.rotangle/2) % 360) / gameplay.rotangle;
}

/**
If the shot is of a bouncing type, here the bounce is done.
*/
void Shot::bounce(){
    if (bounces > 1) bounces--;
    else if (bounces == 1) bounces = -1;
    else if (bounces < 0){
        if ((int)gameplay.rng.rnd(0, shottype->livetime) < livetime) bounces = 0;
    }
    if (bounces == 0) return;

    Point pol, por;

    int diresult = 0;
    double cf = 2;

    Pointf coo = coords.roundup().to<double>();
    for (double d = -1; d <= 1; d += (double)6 / ROTCOUNT){
        pol = (coo + direction.ccw() * sgn(shottype->speed) * cf - direction * d * cf * sgn(shottype->speed)).roundup();
        por = (coo + direction.cw()  * sgn(shottype->speed) * cf - direction * d * cf * sgn(shottype->speed)).roundup();
        if (gameplay.getTerrain(pol) || gameplay.getSolid(pol)) diresult--;
        if (gameplay.getTerrain(por) || gameplay.getSolid(por)) diresult++;
    }

    coords -= direction * sgn(shottype->speed);

    angle = modulo((angle + 180 - diresult * gameplay.rotangle), 360);
    direction = gameplay.dire[angle / gameplay.rotangle];

    coords += direction * sgn(shottype->speed);
}

/**
Performs movement and eventual explosion of a shot.
*/
void Shot::move(){
    livetime -= 1;
    bool justEnd = false;
    for (int i = 0; i <= abs(shottype->speed); i++){
        if (i != 0){
            coords += direction * sgn(shottype->speed);
        }

        if (gameplay.torus) coords.modulo(gameplay.mapsize);

        if (gameplay.getTerrain(coords.roundup()) || gameplay.getSolid(coords.roundup())){
            if (bounces != 0) bounce();
            if (bounces == 0) justEnd = true;
            }
        else if (livetime <= 0) justEnd = true;
        else{
            for (unsigned int plid = 0; plid < gameplay.players.size(); plid++) if ((!gameplay.players[plid]->robot->grave)
                && (gameplay.sqrDistance(coords, gameplay.players[plid]->robot->coords) < sqr(ROBOT_R))) justEnd = true;
        }
        if (justEnd){
            gameplay.explode(coords, owner, shottype);
            justhit = true;
            break;
        }
    }
}

/**
Returns information for "garbage collector"
to know that the shot may be disposed.
*/
bool Shot::forDeletion(){
         return justhit;
}

/**
Returns image of the shot.
*/
SDL_Surface* Shot::getImage(){
    return shottype->image[imageangle];
}
