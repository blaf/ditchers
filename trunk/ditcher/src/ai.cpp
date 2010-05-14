#include "ai.hpp"
#include "players.hpp"
#include "robot.hpp"
#include "game.hpp"
#include "quadtree.hpp"
#include "weapons.hpp"
#include <iostream>
using namespace std;

AI* AI::current = 0;

/* GENERAL */
/**
Registers all functions that AI scripts may call.
*/
AI::AI(string file, string path, Player* player){
    script       = file;
    scriptpath   = path;
    owner   = player;
    state   = lua_open();
    luaL_openlibs(state);

    lua_register(state, "getrotcount",      bounce_getrotcount);
    lua_register(state, "getradius",        bounce_getradius);

    lua_register(state, "getmaxgrave",      bounce_getmaxgrave);
    lua_register(state, "getmaxprotection", bounce_getmaxprotection);

    lua_register(state, "getmapinfo",       bounce_getmapinfo);
    lua_register(state, "getsight",         bounce_getsight);

    lua_register(state, "getweaponscount",  bounce_getshottypescount);
    lua_register(state, "getplayerscount",  bounce_getplayerscount);
    lua_register(state, "getteamscount",    bounce_getteamscount);

    lua_register(state, "getmaxstatus",     bounce_getmaxstatus);

    lua_register(state, "gettime",          bounce_gettime);

    lua_register(state, "gethomes",         bounce_gethomes);
    lua_register(state, "getdeaths",        bounce_getdeaths);

    lua_register(state, "getshots",         bounce_getshots);
    lua_register(state, "getshottype",      bounce_getshottype);

    lua_register(state, "putchat",          bounce_putchat);
    lua_register(state, "getchat",          bounce_getchat);

    lua_register(state, "getlog",           bounce_getlog);

    lua_register(state, "at",               bounce_getterrain);
    lua_register(state, "hom",              bounce_gethomogenous);

    lua_register(state, "cov",              bounce_getcoverage);
    lua_register(state, "covr",             bounce_getratio);
    lua_register(state, "rock",             bounce_getrockerage);
    lua_register(state, "rockr",            bounce_getrockratio);

    lua_register(state, "getmydesignation", bounce_getmypermanent);
    lua_register(state, "getmyinfo",        bounce_getmyinfo);

    lua_register(state, "getdesignation",   bounce_getpermanent);
    lua_register(state, "getvisible",       bounce_getvisible);
    lua_register(state, "getinfo",          bounce_getinfo);
}

AI::~AI(){
    lua_close(state);
}

/**
Sets 'require' search path and starts AI script.
*/
int AI::start(){
    current = this;
    lua_pushstring(state, "package");
    lua_gettable(state, LUA_GLOBALSINDEX);
    lua_pushstring(state, "path");
    lua_pushstring(state, (scriptpath+"/"+script+"/?.lua;"+scriptpath+"/?.lua").c_str());
    lua_settable(state, -3);
    return luaL_dofile(state, (scriptpath+"/"+script+"/main.lua").c_str());
}

/**
Hands control to the script by calling its 'main' function.
*/
int AI::think(){
        current = this;

        lua_getglobal(state, "main");
        lua_call(state, 0, 1);
        int keybmask = (int)lua_tointeger(state, -1);
        lua_pop(state, 1);
        return keybmask;
}

/* AUXILIARY */

/**
Checks whether function was given a proper amount of integer parameters.
*/
void AI::checkargs(int argc){
    if (lua_gettop(state) != argc){
        lua_pushstring(state, "incorrect argument count");
        lua_error(state);
    }

    for (int i=1; i<=argc; i++)
        if (!lua_isnumber(state, i)){
            lua_pushstring(state, ("incorrect "+intToString(i)+". argument").c_str());
            lua_error(state);
        }
}


/**
Checks whether the current player can see the specified point.
*/
bool AI::visible(Point pt, int r){
    return owner->robot->sight(pt, r);
}

/**
Correctly sets coordinations of the given rectangle.
*/
bool AI::readrect(Point* pt1, Point* pt2){
    pt1->set(lua_tointeger(state, 1), lua_tointeger(state, 2));
    pt2->set(lua_tointeger(state, 3), lua_tointeger(state, 4));
    if (gameplay.torus){
        if (pt1->x < 0) pt1->x += gameplay.mapsize.x;
        if (pt2->x < 0) pt2->x += gameplay.mapsize.x;
        if (pt1->x >= gameplay.mapsize.x) pt1->x -= gameplay.mapsize.x;
        if (pt2->x >= gameplay.mapsize.x) pt2->x -= gameplay.mapsize.x;
        if (pt1->y < 0) pt1->y += gameplay.mapsize.y;
        if (pt2->y < 0) pt2->y += gameplay.mapsize.y;
        if (pt1->y >= gameplay.mapsize.y) pt1->y -= gameplay.mapsize.y;
        if (pt2->y >= gameplay.mapsize.y) pt2->y -= gameplay.mapsize.y;
    }
    if ((!pt1->inbox(gameplay.mapsize)) || (!pt2->inbox(gameplay.mapsize))) return false;
    return true;
}

/**
Checks whether the given rectangle is all visible by the current player.
*/
bool AI::visiblerect(Point pt1, Point pt2){
    bool vis = owner->robot->sight(pt1.roundup(), 0) && owner->robot->sight(pt2.roundup(), 0);

    if (vis && gameplay.torus){
        int viewr = gameplay.viewsize / 2;
        Point rpt2 = pt2;
        if (pt1.x > pt2.x) pt2.x += gameplay.mapsize.x;
        if (pt1.y > pt2.y) pt2.y += gameplay.mapsize.y;
        Point vpt1, vpt2;

        bool cont = true;
        if (gameplay.viewsize < gameplay.mapsize.x){
            cont = false;
            int i;
            for (int it=0; it<=2; it++){
                if (it == 2) i = -1; else i = it;
                vpt1.x = (int)(owner->robot->coords.x) - viewr + i*gameplay.mapsize.x;
                vpt2.x = (int)(owner->robot->coords.x) + viewr + i*gameplay.mapsize.x;
                if ((pt1.x >= vpt1.x) && (rpt2.x <= vpt2.x)) {cont = true; break;}
            }
        }

        if (cont && (gameplay.viewsize < gameplay.mapsize.y)){
            cont = false;
            int i;
            for (int it=0; it<=2; it++){
                if (it == 2) i = -1; else i = it;
                vpt1.y = (int)(owner->robot->coords.y) - viewr + i*gameplay.mapsize.y;
                vpt2.y = (int)(owner->robot->coords.y) + viewr + i*gameplay.mapsize.y;
                if ((pt1.y >= vpt1.y) && (rpt2.y <= vpt2.y)) {cont = true; break;}
            }
        }

        vis = vis && cont;
    }

    return vis;
}

/**
Checks what type of rectangle is the given one.
0 -- casual, 1 -- inverted, 2 -- not whole in the map
*/
int AI::specrect(Point pt1, Point pt2){
    if (!gameplay.torus){
        if ((pt1.x > pt2.x) || (pt1.y > pt2.y)) return 1;
        else if ((pt2.x < 0) || (pt1.x >= gameplay.mapsize.y)
            || (pt2.y < 0) || (pt1.y >= gameplay.mapsize.y)) return 2;
        else return 0;
    }else return 0;
}

/**
Extracts the only argument as an index of a visible player.
*/
int AI::acquireindex(){

        checkargs(1);

        int index = lua_tointeger(state, 1);

        if ((index < 0) || ((unsigned int)index >= gameplay.players.size())
                || (gameplay.players[index]->robot->grave != 0)
                || !visible(gameplay.players[index]->robot->coords.roundup(), ROBOT_R))
                index = -1;

        return index;
}

/**
Extracts the only argument as an index of a player.
*/
int AI::acquireindexgeneral(){

        checkargs(1);

        int index = lua_tointeger(state, 1);

        if ((index < 0) || ((unsigned int)index >= gameplay.players.size()))
                index = -1;

        return index;
}

/* GAME SETTINGS */
/**
Returns number of rotational positions.
*/
int AI::bounce_getrotcount(lua_State* luastate){ return (current->getrotcount()); }
int AI::getrotcount(){
        lua_pushnumber(state, ROTCOUNT);
        return 1;
}

/**
Returns radius of a robot and robot explosion.
*/
int AI::bounce_getradius(lua_State* luastate){ return (current->getradius()); }
int AI::getradius(){
    lua_pushnumber(state, ROBOT_R);
    lua_pushnumber(state, DEATH_R);
    return 2;
}

/**
Returns number of game loops before a player moves home and before he respawns.
*/
int AI::bounce_getmaxgrave(lua_State* luastate){ return (current->getmaxgrave()); }
int AI::getmaxgrave(){
        lua_pushnumber(state, Robot::maxGrave);
        return 1;
}

/**
Returns number of game loops when a player has absolute protection.
*/
int AI::bounce_getmaxprotection(lua_State* luastate){ return (current->getmaxprotection()); }
int AI::getmaxprotection(){
        lua_pushnumber(state, Robot::maxRespawn);
        return 1;
}

/**
Returns number of weapon types.
*/
int AI::bounce_getshottypescount(lua_State* luastate){ return (current->getshottypescount()); }
int AI::getshottypescount(){
        lua_pushnumber(state, gameplay.shottypes.size());
        return 1;
}

/**
Returns size and topology of the map.
*/
int AI::bounce_getmapinfo(lua_State* luastate){ return (current->getmapinfo()); }
int AI::getmapinfo(){
        lua_pushnumber(state, gameplay.mapsize.x);
        lua_pushnumber(state, gameplay.mapsize.y);
        lua_pushboolean(state, gameplay.torus);
        return 3;
}

/**
Returns visibility radius -- half the square side.
*/
int AI::bounce_getsight(lua_State* luastate){ return (current->getsight()); }
int AI::getsight(){
        lua_pushnumber(state, gameplay.viewsize / 2);
        return 1;
}

/**
Returns number of players in the game.
*/
int AI::bounce_getplayerscount(lua_State* luastate){ return (current->getplayerscount()); }
int AI::getplayerscount(){
        lua_pushnumber(state, gameplay.players.size());
        return 1;
}

/**
Returns number of teams in the game.
*/
int AI::bounce_getteamscount(lua_State* luastate){ return (current->getteamscount()); }
int AI::getteamscount(){
        lua_pushnumber(state, gameplay.teamscount);
        return 1;
}

/**
Returns maximum amount of health and energy.
*/
int AI::bounce_getmaxstatus(lua_State* luastate){ return (current->getmaxstatus()); }
int AI::getmaxstatus(){
        lua_pushnumber(state, Robot::maxHealth);
        lua_pushnumber(state, Robot::maxEnergy);
        return 2;
}

/* GAME STATE */
/**
Returns number of game loops since the beginning of the game.
*/
int AI::bounce_gettime(lua_State* luastate){ return (current->gettime()); }
int AI::gettime(){
        lua_pushnumber(state, gameplay.chronos);
        return 1;
}

/**
Returns value of the terrain.
0 -- free, 1 -- terrain, 2 -- rock, -1 -- not visible
*/
int AI::bounce_getterrain(lua_State* luastate){ return (current->getterrain()); }
int AI::getterrain(){

        checkargs(2);

        Point pt(lua_tointeger(state, 1), lua_tointeger(state, 2));

        if (gameplay.torus) pt.modulo(gameplay.mapsize);

        int terrtype;
        if (!pt.inbox(gameplay.mapsize)) terrtype = 2;
        else if (!visible(pt, 0)) terrtype = -1;
        else if (gameplay.getSolid(pt)) terrtype = 2;
        else if (gameplay.getTerrain(pt)) terrtype = 1;
        else terrtype = 0;

    if (terrtype >= 0){
        lua_pushnumber(state, terrtype);
        return 1;
    }else{
        return 0;
    }

}

/**
Checks whether the given rectangle is whole visible and homogenous.
0 -- not homogenous, 1 -- homogenous, -1 -- not visible
*/
int AI::bounce_gethomogenous(lua_State* luastate){ return (current->gethomogenous()); }
int AI::gethomogenous(){

    checkargs(5);

    Point pt1, pt2;
    bool inside = readrect(&pt1, &pt2);

    int ter = lua_tointeger(state, 5);

    bool vis = visiblerect(pt1, pt2);

    if (!vis)
        return 0;
    else{

        bool hom;
        switch (specrect(pt1, pt2)){
            case 0:{
                hom = gameplay.qtMap->homogenous(pt1.x, pt1.y, pt2.x, pt2.y, ter);
                if ((!inside) && (!gameplay.torus)) hom = ((ter == 2) || (ter == 4)) ? hom : false;
            break;}
            case 1:{
                hom = true;
            break;}
            case 2:{
                hom = ((ter == 2) || (ter == 4)) ? true : false;
            break;}
        }

        lua_pushnumber(state, hom ? 1 : 0);
        return 1;
    }

}

/**
Checks whether the given rectangle is whole visible and counts how much it is covered.
-1 -- not visible, nonnegative -- coverage in pixels
*/
int AI::bounce_getcoverage(lua_State* luastate){ return (current->getcoverage()); }
int AI::getcoverage(){
    
    checkargs(4);
    
    Point pt1, pt2;
    readrect(&pt1, &pt2);
    
    bool vis = visiblerect(pt1, pt2);
    
    if (!vis)
        return 0;
    else{
        
        int cov;
        switch (specrect(pt1, pt2)){
        case 0:{
            cov = gameplay.qtMap->getcoverage(pt1.x, pt1.y, pt2.x, pt2.y);
            break;}
        case 1:{
            cov = 0;
            break;}
        case 2:{
            cov = gameplay.qtMap->size(pt1.x, pt1.y, pt2.x, pt2.y);
            break;}
        }
        
        lua_pushnumber(state, cov);
        return 1;
    }
}

/**
Checks whether the given rectangle is whole visible and counts what part is covered.
-1 -- not visible, nonnegative -- coverage in range [0..1]
*/
int AI::bounce_getratio(lua_State* luastate){ return (current->getratio()); }
int AI::getratio(){
    
    checkargs(4);
    
    Point pt1, pt2;
    readrect(&pt1, &pt2);
    
    bool vis = visiblerect(pt1, pt2);
    
    if (!vis)
        return 0;
    else{
        double cov;
        switch (specrect(pt1, pt2)){
        case 0:{
            cov = gameplay.qtMap->getratio(pt1.x, pt1.y, pt2.x, pt2.y);
            break;}
        case 1:{
            cov = 0;
            break;}
        case 2:{
            cov = 1;
            break;}
        }
        lua_pushnumber(state, cov);
        return 1;
    }
}

/**
Checks whether the given rectangle is whole visible and counts how much it is covered with rock.
-1 -- not visible, nonnegative -- coverage in pixels
*/
int AI::bounce_getrockratio(lua_State* luastate){ return (current->getrockratio()); }
int AI::getrockratio(){
    
    checkargs(4);
    
    Point pt1, pt2;
    readrect(&pt1, &pt2);
    
    bool vis = visiblerect(pt1, pt2);
    
    if (!vis)
        return 0;
    else{
        
        double rock;
        switch (specrect(pt1, pt2)){
        case 0:{
            rock = gameplay.qtMap->getrockratio(pt1.x, pt1.y, pt2.x, pt2.y);
            break;}
        case 1:{
            rock = 0;
            break;}
        case 2:{
            rock = 1;
            break;}
        }
        
        lua_pushnumber(state, rock);
        return 1;
    }
}

/**
Checks whether the given rectangle is whole visible and counts what part is covered with rock.
-1 -- not visible, nonnegative -- coverage in range [0..1]
*/
int AI::bounce_getrockerage(lua_State* luastate){ return (current->getrockerage()); }
int AI::getrockerage(){
    
    checkargs(4);
    
    Point pt1, pt2;
    readrect(&pt1, &pt2);
    
    bool vis = visiblerect(pt1, pt2);
    
    if (!vis)
        return 0;
    else{
        
        int rock;
        switch (specrect(pt1, pt2)){
        case 0:{
            rock = gameplay.qtMap->getrockerage(pt1.x, pt1.y, pt2.x, pt2.y);
            break;}
        case 1:{
            rock = 0;
            break;}
        case 2:{
            rock = gameplay.qtMap->size(pt1.x, pt1.y, pt2.x, pt2.y);
            break;}
        }
        
        lua_pushnumber(state, rock);
        return 1;
    }
}

/**
Pushes a chat message.
*/
int AI::bounce_putchat(lua_State* luastate){ return (current->putchat()); }
int AI::putchat(){
    if (lua_gettop(state) != 2){
        lua_pushstring(state, "incorrect argument count");
        lua_error(state);
    }
    if (!lua_isboolean(state, 1)){
        lua_pushstring(state, "incorrect 1. argument");
        lua_error(state);
    }
    if (!lua_isstring(state, 2)){
        lua_pushstring(state, "incorrect 2. argument");
        lua_error(state);
    }

    bool teamonly = (lua_toboolean(state, 1) != 0);
    string msg    = lua_tostring(state, 2);

    lua_pushboolean(state, gameplay.updateChat(msg, teamonly, owner));

    return 1;
}

/**
Checks for messages in a given time interval and returns a table of records..
No argument -- all messages, 1 argument -- only this loop, 2 arguments -- interval
*/
int AI::bounce_getchat(lua_State* luastate){ return (current->getchat()); }
int AI::getchat(){

    int stampmin = 0;
    int stampmax = gameplay.chronos-1;
    switch(lua_gettop(state)){
        case 0:{
            stampmin = 0; stampmax = gameplay.chronos-1;
        break;}
        case 1:{
            stampmin = stampmax = lua_tointeger(state, 1);
            if (stampmin >= gameplay.chronos) stampmin = stampmax = gameplay.chronos-1;
        break;}
        case 2:{
            stampmin = lua_tointeger(state, 1);
            stampmax = lua_tointeger(state, 2);
        break;}
        default:{
            checkargs(3);
        break;}
    }

    int ii = 1;
    lua_createtable(state, gameplay.chat.size()*3, 0);
    int luatable = lua_gettop(state);

    for (unsigned int i = 0; i < gameplay.chat.size(); i++)
        if (((!gameplay.chat[i].pl) || (gameplay.chat[i].pl->teamid == owner->teamid))
            && (stampmin<=gameplay.chat[i].stamp) && (gameplay.chat[i].stamp<=stampmax)){
            lua_pushstring(state, gameplay.chat[i].text.c_str());
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, gameplay.chat[i].stamp);
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, gameplay.chat[i].pl ? gameplay.chat[i].pl->index : -1);
            lua_rawseti(state, luatable, ii);
            ii++;
        }

    return 1;
}

/**
Checks for log messages in a given time interval and returns a table of records.
No argument -- all messages, 1 argument -- only this loop, 2 arguments -- interval
*/
int AI::bounce_getlog(lua_State* luastate){ return (current->getlog()); }
int AI::getlog(){

    int stampmin = 0;
    int stampmax = gameplay.chronos-1;
    switch(lua_gettop(state)){
        case 0:{
            stampmin = 0; stampmax = gameplay.chronos-1;
        break;}
        case 1:{
            stampmin = stampmax = lua_tointeger(state, 1);
            if (stampmin >= gameplay.chronos) stampmin = stampmax = gameplay.chronos-1;
        break;}
        case 2:{
            stampmin = lua_tointeger(state, 1);
            stampmax = lua_tointeger(state, 2);
        break;}
        default:{
            checkargs(3);
        break;}
    }

    int ii = 1;
    lua_createtable(state, gameplay.log.size()*4, 0);
    int luatable = lua_gettop(state);

    for (unsigned int i = 0; i < gameplay.log.size(); i++)
        if ((stampmin<=gameplay.log[i].stamp) && (gameplay.log[i].stamp<=stampmax)){
            lua_pushnumber(state, gameplay.log[i].stamp);
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, gameplay.log[i].killer ? gameplay.log[i].killer->index : -1);
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, gameplay.log[i].killed->index);
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, gameplay.log[i].weapon ? gameplay.log[i].weapon->id : -1);
            lua_rawseti(state, luatable, ii);
            ii++;
        }

    return 1;
}

/**
Checks for homes in sight and returns a table of records.
*/
int AI::bounce_gethomes(lua_State* luastate){ return (current->gethomes()); }
int AI::gethomes(){

    int ii = 1;
    lua_createtable(state, gameplay.players.size()*2, 0);
    int luatable = lua_gettop(state);

    for (unsigned int i = 0; i < gameplay.players.size(); i++)
        if (visible(gameplay.players[i]->robot->home.roundup(), ROBOT_R)){
            lua_pushnumber(state, gameplay.players[i]->robot->home.x);
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, gameplay.players[i]->robot->home.y);
            lua_rawseti(state, luatable, ii);
            ii++;
        }

    return 1;
}

/**
Checks for death stains in sight and returns a table of records.
*/
int AI::bounce_getdeaths(lua_State* luastate){ return (current->getdeaths()); }
int AI::getdeaths(){

    int ii = 1;
    lua_createtable(state, gameplay.players.size()*2, 0);
    int luatable = lua_gettop(state);

    for (unsigned int i = 0; i < gameplay.players.size(); i++)
        if ((gameplay.players[i]->robot->grave < 0)
            && visible(gameplay.players[i]->robot->coords.roundup(), ROBOT_R)){
            lua_pushnumber(state, gameplay.players[i]->robot->coords.x);
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, gameplay.players[i]->robot->coords.y);
            lua_rawseti(state, luatable, ii);
            ii++;
            lua_pushnumber(state, -gameplay.players[i]->robot->grave);
            lua_rawseti(state, luatable, ii);
            ii++;
        }

    return 1;
}

/**
Returns list of all shots in sight as a table of records.
*/
int AI::bounce_getshots(lua_State* luastate){ return (current->getshots()); }
int AI::getshots(){

        int shotscount = 0;
        for (Shot* shot = gameplay.shots->begin(); ! gameplay.shots->end(); shot =  gameplay.shots->next())
            if (visible(shot->coords.roundup(), 5)) shotscount++;

        int i = 1;
        lua_createtable(state, shotscount*8, 0);
        int luatable = lua_gettop(state);

        for (Shot* shot = gameplay.shots->begin(); ! gameplay.shots->end(); shot =  gameplay.shots->next())
            if (visible(shot->coords.roundup(), 5)){
                lua_pushboolean(state, shot->owner->owner->teamid == owner->teamid);
                lua_rawseti(state, luatable, i);
                i++;
                lua_pushnumber(state, shot->coords.x);
                lua_rawseti(state, luatable, i);
                i++;
                lua_pushnumber(state, shot->coords.y);
                lua_rawseti(state, luatable, i);
                i++;
                lua_pushnumber(state, shot->speed);
                lua_rawseti(state, luatable, i);
                i++;
                lua_pushnumber(state, shot->angle);
                lua_rawseti(state, luatable, i);
                i++;
                lua_pushnumber(state, shot->direction.x);
                lua_rawseti(state, luatable, i);
                i++;
                lua_pushnumber(state, shot->direction.y);
                lua_rawseti(state, luatable, i);
                i++;
                lua_pushnumber(state, shot->shottype->id);
                lua_rawseti(state, luatable, i);
                i++;
            }

        return 1;
}

/**
Returns list of all weapon types and its parameters.
*/
int AI::bounce_getshottype(lua_State* luastate){ return (current->getshottype()); }
int AI::getshottype(){

        checkargs(1);

        int i = lua_tointeger(state, 1);
    if ((i >= 0) && (i < (int)gameplay.shottypes.size())){
        lua_pushnumber(state, gameplay.shottypes[i]->energy);
        lua_pushnumber(state, gameplay.shottypes[i]->reload);
        lua_pushnumber(state, gameplay.shottypes[i]->speed);
        lua_pushnumber(state, gameplay.shottypes[i]->ditch);
        lua_pushnumber(state, gameplay.shottypes[i]->splash);
        lua_pushnumber(state, gameplay.shottypes[i]->damage);
        lua_pushnumber(state, gameplay.shottypes[i]->endamage);
        lua_pushnumber(state, gameplay.shottypes[i]->bounces);
        lua_pushnumber(state, gameplay.shottypes[i]->livetime);
        return 9;
    }else return 0;
}

/* MY ROBOT */
/**
Returns all permanent information about the current robot in this order:
    ID, name, team ID
*/
int AI::bounce_getmypermanent(lua_State* luastate){ return (current->getmypermanent()); }
int AI::getmypermanent(){
    lua_pushnumber(state, owner->index);
    lua_pushstring(state, owner->name.c_str());
    lua_pushnumber(state, owner->teamid);
    return 3;
}

/**
Returns all current information about the current robot in this order:
    location, direction, angle, speed, health, energy, protection, grave, weapon ID, reload
*/
int AI::bounce_getmyinfo(lua_State* luastate){ return (current->getmyinfo()); }
int AI::getmyinfo(){
    lua_pushnumber(state, owner->robot->coords.x);
    lua_pushnumber(state, owner->robot->coords.y);
    lua_pushnumber(state, owner->robot->direction.x);
    lua_pushnumber(state, owner->robot->direction.y);
    lua_pushnumber(state, owner->robot->angle);
    lua_pushnumber(state, owner->robot->speed);
    lua_pushnumber(state, owner->robot->health);
    lua_pushnumber(state, owner->robot->energy);
    lua_pushnumber(state, owner->robot->respawn);
    lua_pushnumber(state, owner->robot->grave);
    lua_pushnumber(state, owner->robot->weapon);
    lua_pushnumber(state, owner->robot->reload);
    return 12;
}


/* OTHER ROBOT */
/**
Returns all permanent information about the given robot in this order:
    name, team ID
*/
int AI::bounce_getpermanent(lua_State* luastate){ return (current->getpermanent()); }
int AI::getpermanent(){
    int index = acquireindexgeneral();
    if (index >= 0){
        Player* pl = gameplay.players[index];
        lua_pushstring(state, pl->name.c_str());
        lua_pushnumber(state, pl->teamid);
        return 2;
    }else return 0;
}

/**
Returns whether the given robot is visible by current player.
*/
int AI::bounce_getvisible(lua_State* luastate){ return (current->getvisible()); }
int AI::getvisible(){
    
    int index = acquireindex();
    
    lua_pushboolean(state, (index >= 0));
    
    return 1;
}

/**
Returns all current information about the current robot in this order:
    location, direction, angle, speed, health, energy, protection
*/
int AI::bounce_getinfo(lua_State* luastate){ return (current->getinfo()); }
int AI::getinfo(){
    int index = acquireindex();
    if (index >= 0){
        Robot* rob = gameplay.players[index]->robot;
        lua_pushnumber(state, rob->coords.x);
        lua_pushnumber(state, rob->coords.y);
        lua_pushnumber(state, rob->direction.x);
        lua_pushnumber(state, rob->direction.y);
        lua_pushnumber(state, rob->angle);
        lua_pushnumber(state, rob->speed);
        lua_pushnumber(state, rob->health);
        lua_pushnumber(state, rob->energy);
        lua_pushboolean(state, rob->respawn > 0);
        return 9;
    }else return 0;
}
