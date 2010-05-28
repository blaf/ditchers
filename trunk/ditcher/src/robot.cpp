#include "robot.hpp"
#include "global.hpp"
#include "weapons.hpp"
#include "game.hpp"
#include "gfx.hpp"
#include "robottype.hpp"

#include "SDL_gfxPrimitives.h"

Viewable::~Viewable(){}

double Robot::testdist[ROBOT_R*2+1];

int Robot::maxShots = 16;
int Robot::maxSpeed = 5;
int Robot::maxHealth = 1000;
int Robot::maxEnergy = 5000;
int Robot::maxGrave = 100;
int Robot::maxRespawn = 50;

Robot::~Robot(){
    SDL_FreeSurface(nameimage);
    SDL_FreeSurface(statimage);
}

/**
Creates robot, initializes it and sets its owner.
*/
Robot::Robot(Player* pl){
    health = maxHealth;
    energy = maxEnergy;
    reload = 0;
    maxReload = 1;
    activeshots = 0;
    weapon = 0;
    speed = 0;
    angle = 0;
    setDirection();
    grave = 0;
    respawn = maxRespawn;
    SDL_Surface* statimagex = SDL_CreateRGBSurface(SDL_HWSURFACE, ROBOT_R * 2 + 1, ROBOT_R, 32, 0, 0, 0, 0);
    statimage = SDL_DisplayFormatAlpha(statimagex);
    SDL_FreeSurface(statimagex);
    SDL_FillRect(statimage, NULL, SDL_MapRGBA(statimage->format, 0, 0, 0, 0));
    owner = pl;
    robottype = owner->robottype;
    setName();
}

/**
Creates a surface with robot's name.
*/
void Robot::setName(){
    nameimage = SDL_CreateRGBSurface(SDL_HWSURFACE, owner->name.length() * 8, 8, 32, 0, 0, 0, 0);
    SDL_FillRect(nameimage, NULL, SDL_MapRGBA(nameimage->format, 255, 0, 255, 255));
    SDL_SetColorKey(nameimage, SDL_SRCCOLORKEY, SDL_MapRGBA(nameimage->format, 255, 0, 255, 255));

    int r = 255; int g = 255; int b = 255;;
    if (gameplay.teamscount){
             if (owner->teamid == 1){ r = 255; g = 0  ; b = 0  ; }
        else if (owner->teamid == 2){ r = 63 ; g = 63 ; b = 255; }
        else if (owner->teamid == 3){ r = 31 ; g = 255; b = 31 ; }
        else if (owner->teamid == 4){ r = 255; g = 255; b = 0  ; }
        else if (owner->teamid == 5){ r = 0  ; g = 223; b = 223; }
        else if (owner->teamid == 6){ r = 223; g = 63 ; b = 223; }
        else if (owner->teamid == 7){ r = 127; g = 127; b = 127; }
        else if (owner->teamid == 8){ r = 31 ; g = 31 ; b = 31 ; }
    }

    stringRGBA(nameimage, 0, 0, owner->name.c_str(), r, g, b, 255);
}

/**
Sets robot to dead state and updates score and log.
*/
void Robot::die(Robot* inflictor, ShotType* shottype){
        gameplay.ditch(coords.roundup(), DEATH_R);

        grave   = -maxGrave;

        owner->deaths++;
        if (gameplay.teamscount) owner->team->deaths++;

        if (inflictor){
            if (gameplay.teamscount && (inflictor->owner->teamid != owner->teamid)){
                inflictor->owner->points++;
                inflictor->owner->team->points++;
                if (inflictor->owner->team->points >= gameplay.limit) gameplay.ended = true;
            }else if (!gameplay.teamscount && (inflictor != this)){
                inflictor->owner->points++;
                if (inflictor->owner->points >= gameplay.limit) gameplay.ended = true;
            }else{
                owner->points--;
                if (gameplay.teamscount) owner->team->points--;
            }
        }else{
            owner->points--;
            if (gameplay.teamscount) owner->team->points--;
        }

        LogRec logrec;
        logrec.killed = this->owner;
        logrec.killer = inflictor ? inflictor->owner : 0;
        logrec.weapon = shottype;
        logrec.stamp = gameplay.chronos;
        gameplay.log.push_back(logrec);

    gameplay.playSound(coords.roundup(), gameplay.sndexplode);
}

/**
Modifies energy and/or reload if needed and performs robot's action.
*/
void Robot::everyLoop(){

    owner->letterlimit++;

    if (respawn) respawn--;
    else if (grave){
        if (grave == -1){
            grave  = maxGrave;
            coords = home;
            reload = 0;
            health = maxHealth;
            energy = maxEnergy;
        }else if (grave < 0){
            grave++;
        }else if (grave > 0){
            grave--;
            if (!grave) respawn = maxRespawn;
        }
    }

    if (!grave){

        if (reload) reload--;

        int otherHome = 0;
        Player* pl;

        for (unsigned int plid = 0; plid < gameplay.players.size(); plid++){
            pl = gameplay.players[plid];
            if (pl && (gameplay.sqrDistance(coords.x, coords.y, pl->robot->home.x, pl->robot->home.y) < sqr(80)))
            {
                if (pl->robot->owner->teamid == owner->teamid) { otherHome = 2; break; }
                else if (otherHome < 1) otherHome = 1;
            }
        }

        if (otherHome == 2){
            health+=3;
            energy+=50;
        }else if (otherHome == 1){
            energy+=25;
        }else{
            energy-=1;
            if (energy <= 0) die(0, 0);
        }

        bound(&health, 0, maxHealth);
        bound(&energy, 0, maxEnergy);

        action();

        gameplay.ditch(coords.roundup(), ROBOT_R);
    }
}

/**
If the robot may shoot reload and energy is modified.
*/
bool Robot::tryShot(){
    if ((reload > 0) || (energy < gameplay.shottypes[weapon]->energy) || (activeshots >= maxShots))
        return false;
    else{
        reload += gameplay.shottypes[weapon]->reload;
        maxReload = gameplay.shottypes[weapon]->reload;
        activeshots++;
        energy -= gameplay.shottypes[weapon]->energy;
        return true;
    }
}

/**
Number of active shots is decreased.
*/
bool Robot::removeShot(){
    if (activeshots > 0){
        activeshots--;
        return true;
    }else return false;
}

/**
Returns rotational position.
*/
int Robot::getImageAngle(){
    int imageangle = ((angle+gameplay.rotangle/2) % 360) / gameplay.rotangle;
    return imageangle;
}

/**
Sets direction according to angle.
*/
void Robot::setDirection(){
    direction = gameplay.dire[angle / gameplay.rotangle];
}

/**
Draws health and energy indicators to the tiny status image.
*/
void Robot::setStat(){
    SDL_Rect  rectenergy,  recthealth;
    int      levelenergy, levelhealth;

    rectenergy.x = 0; rectenergy.y = statimage->h / 5;
    rectenergy.w = statimage->w; rectenergy.h = statimage->h / 5;

    levelenergy = rectenergy.w * energy / maxEnergy;

    boxRGBA(statimage, rectenergy.x + levelenergy, rectenergy.y,
        rectenergy.x + rectenergy.w, rectenergy.y + rectenergy.h, 63, 63, 63, 255);

    boxRGBA(statimage, rectenergy.x, rectenergy.y,
        rectenergy.x + levelenergy, rectenergy.y + rectenergy.h, 255, 255, 0, 255);

    recthealth.x = 0; recthealth.y = statimage->h / 5 * 3;
    recthealth.w = statimage->w; recthealth.h = statimage->h / 5;

    levelhealth = recthealth.w * health / maxHealth;

    boxRGBA(statimage, recthealth.x + levelhealth, recthealth.y,
        recthealth.x + recthealth.w, recthealth.y + recthealth.h, 63, 63, 63, 255);

    boxRGBA(statimage, recthealth.x, recthealth.y,
        recthealth.x + levelhealth, recthealth.y + recthealth.h, 0, 127, 255, 255);

}

/**
Returns actual image of the robot.
*/
SDL_Surface* Robot::getImage(){
    return robottype->image[getImageAngle()];
}

/**
Paints the home = base at the given point to the map.
*/
void Robot::paintHome(Point pthome, int basetype){
    int homesize = 80;
    if (basetype > 0){
        filledCircleRGBA(gameplay.terrain, pthome.x, pthome.y, homesize, 255, 0, 255, 255);
        filledCircleRGBA(gameplay.solid, pthome.x, pthome.y, homesize, 63, 63, 95, 255);
        filledCircleRGBA(gameplay.solid, pthome.x, pthome.y, homesize - 20, 255, 0, 255, 255);
        boxRGBA(gameplay.solid, pthome.x - ROBOT_R * 3 / 2, pthome.y - homesize,
            pthome.x + ROBOT_R * 3 / 2, pthome.y + homesize, 255, 0, 255, 255);
        if (basetype == 2)
            boxRGBA(gameplay.solid, pthome.x - homesize, pthome.y - ROBOT_R * 3 / 2,
                pthome.x + homesize, pthome.y + ROBOT_R * 3 / 2, 255, 0, 255, 255);
    }
}

/**
Paints the home = base at the given point to the map with topology in mind.
*/
void Robot::createHome(int basetype){
    home = coords;

    int homesize = 80;

    Point temp = home.toInt();
        paintHome(temp, basetype);
    if (gameplay.torus){
        if (temp.x < homesize) paintHome(Point(temp.x + gameplay.mapsize.x, temp.y), basetype);
        if (temp.y < homesize) paintHome(Point(temp.x, temp.y + gameplay.mapsize.y), basetype);
        if (temp.x >= gameplay.mapsize.x - homesize)
            paintHome(Point(temp.x - gameplay.mapsize.x, temp.y), basetype);
        if (temp.y >= gameplay.mapsize.y - homesize)
            paintHome(Point(temp.x, temp.y - gameplay.mapsize.y), basetype);
        if ((temp.x < homesize) && (temp.y < homesize))
            paintHome(Point(temp.x + gameplay.mapsize.x, temp.y + gameplay.mapsize.y), basetype);
        if ((temp.x >= gameplay.mapsize.x - homesize) && (temp.y < homesize))
            paintHome(Point(temp.x - gameplay.mapsize.x, temp.y + gameplay.mapsize.y), basetype);
        if ((temp.x < homesize) && (temp.y >= gameplay.mapsize.y - homesize))
            paintHome(Point(temp.x + gameplay.mapsize.x, temp.y - gameplay.mapsize.y), basetype);
        if ((temp.x >= gameplay.mapsize.x - homesize) && (temp.y >= gameplay.mapsize.y - homesize))
            paintHome(Point(temp.x - gameplay.mapsize.x, temp.y - gameplay.mapsize.y), basetype);
    }

    coords = home;
}

/**
Paints the home = base at the given point to the map with topology in mind.
*/
void Robot::wasHit(int damage, int endamage, Robot* inflictor, ShotType* shottype){
    if (grave || respawn) return;
    health -= damage;
    energy -= endamage;
    if ((health <= 0) || (energy < 0)) die(inflictor, shottype);
}

bool Robot::sight(Point pt, int r){
    if (gameplay.torus){
        if ((abs(gameplay.distVector(coords.x, pt.x, gameplay.mapsize.x))
            <= gameplay.viewsize / 2 + r)
            && ((abs(gameplay.distVector(coords.y, pt.y, gameplay.mapsize.y))
            <= gameplay.viewsize / 2 + r)))
            return true;
        else return false;
    }else{
        if ((abs((int)(coords.x - pt.x)) <= gameplay.viewsize / 2 + r)
            && (abs((int)(coords.y - pt.y)) <= gameplay.viewsize / 2 + r))
            return true;
            else return false;
    }
}

/**
Performs robot's actions according to player's decision.
*/
void Robot::action(){

    if (grave) return;

    if (actionset.r) {
        angle = modulo((angle + gameplay.rotangle), 360);
        setDirection();
    }
    if (actionset.l) {
        angle = modulo((angle - gameplay.rotangle), 360);
        setDirection();
    }
    if ((actionset.u) || (actionset.d)) {

        int orient;

        if (actionset.u){
            orient = 1;
            gameplay.playSound(coords.roundup(), gameplay.sndfwd);
        }else{
            orient = -1;
            gameplay.playSound(coords.roundup(), gameplay.sndbwd);
        }

        /* stopped by too much mud? */
        bool mud = countSpeed(orient);

        if (!mud && (speed == 0)){

            int tempa = angle;
            Pointf tdirection;

            int i;

            for (i=gameplay.rotangle; i<90; i+=gameplay.rotangle){

                double speed1, speed2;
                bool mud1, mud2;
                angle=modulo((tempa + i), 360);
                setDirection(); mud1 = countSpeed(orient); speed1 = speed;
                angle=modulo((tempa - i), 360);
                setDirection(); mud2 = countSpeed(orient); speed2 = speed;

                if ((speed1 > speed2) || (mud1 && !mud2)){
                    angle=modulo((tempa + i), 360);
                    setDirection(); countSpeed(orient);
                    break;
                }else if ((speed1 < speed2) || (!mud1 && mud2)){
                    angle=modulo((tempa - i), 360);
                    setDirection(); countSpeed(orient);
                    break;
                }else if (mud1 && mud2){
                    speed = 0;
                    break;
                }

            }

            angle=tempa;
            if (i<90) speed *= cos((double)i / 180 * PI); else speed = 0;

        }

        if (speed > 0){

            coords += direction * orient * speed;

            energy -= 3;

            if (gameplay.torus) coords.modulo(gameplay.mapsize);
        }
        setDirection();
    }

    if (actionset.weapon) {
        weapon++;
        if (weapon == gameplay.shottypes.size()) weapon = 0;
    }

    if ((actionset.fire) && (tryShot())){
        gameplay.shots->add(new Shot(gameplay.shottypes[weapon],
            coords + direction * (1 + ROBOT_R) * sgnB(gameplay.shottypes[weapon]->speed),
            direction, angle, this));
    }
}

/**
Checks movement options in the given direction, counts
speed and returns whether movement is possible.
*/
bool Robot::countSpeed(int orient){

    Player* pl;
    Robot* otherRobot;
    speed = maxSpeed;
    Pointf delta = direction * orient * speed;

    for (unsigned int plid = 0; plid < gameplay.players.size(); plid++){
        pl  = gameplay.players[plid];
        if ((pl == owner) || (pl->robot->grave)) continue;

        otherRobot = pl->robot;

        double sqrDistNew = gameplay.sqrDistance(otherRobot->coords, coords+delta);
        if (sqrDistNew <= sqr(2*ROBOT_R)){
            double sqrDist = gameplay.sqrDistance(otherRobot->coords, coords);
            if (sqrDist > sqrDistNew){
                speed = 0;
                break;
            }
        }
    }

    if (speed > 0){
        speed = maxSpeed - abs(orient) + orient;

        double diminish = 0.8 / (double)(ROBOT_R*2+1);
        Point tested;
        double limit = maxSpeed;
        for (int j=0; (j<=limit); j++){
            for (int i=-ROBOT_R; i<=ROBOT_R; i++){
                tested = (coords + direction * orient * (j + testdist[i+ROBOT_R]) + direction.cw() * i).roundup();

                if (gameplay.torus) tested.modulo(gameplay.mapsize); else tested.outbox(gameplay.mapsize);

                if (gameplay.getSolid(tested)) limit = (limit > j) ? ((j < 0) ? 0 : j) : limit;
                else if ((j>0) && gameplay.getTerrain(tested)) speed = speed - diminish;

            }
        }

        if (speed < 0) speed = 0;
        if (speed >= limit) { speed = limit; return false; }
        return true;

    }else return false;

}
