#include "game.hpp"
#include "gfx.hpp"
#include "robot.hpp"
#include "weapons.hpp"
#include "network.hpp"
#include "userface.hpp"
#include "ai.hpp"
#include "quadtree.hpp"

#include <iostream>

#include "SDL_gfxPrimitives.h"

#define CORNER_RIGHT  1
#define CORNER_BOTTOM 2
#define CORNER_LEFT   4
#define CORNER_TOP    8

#define LETTER_W      8

GameView::~GameView(){
    delete(viewrect);
    delete(statrect);
    SDL_FreeSurface(status);
}

void GameView::setmiddle(){
    middle.set(viewrect->x + viewrect->w / 2, viewrect->y + viewrect->h / 2);
}

/**
Computes relative coordinates for displaying on screen.
*/
void GameView::setpictloc(Robot* rob){
    pictloc.x = middle.x - (int)ceil(rob->coords.x);
    pictloc.y = middle.y - (int)ceil(rob->coords.y);
}

/**
Initializes basic constants and variables and creates weapon types.
*/
GamePlay::GamePlay(){

    for (int i=0; i<=ROBOT_R*2; i++) Robot::testdist[i] = (ROBOT_R+1) * sqrt(1-sqr((double)(i-ROBOT_R) / (ROBOT_R+1)));

    keys = new bool[SDLK_LAST+1];
    rotangle = 360 / ROTCOUNT;

    for (int i = 0; i < ROTCOUNT; i++)
        dire[i].set(sin((double)(i*rotangle)/180*PI), -cos((double)(i*rotangle)/180*PI));

    shots = new List<Shot*>(1024);

    teamscount = 0;

    for (unsigned int i = 1; i <= 8; i++){
        teams.push_back(Team(i));
    }

    limit = 10;

    localplayers = 0;
    localhumans = 0;

    blob = true;

    ground  = 0;
    terrain = 0;
    solid   = 0;

    statusraw = 0;

    approved = false;
    started = false;
    ended = false;
    local = true;

    chatting = false;
}

/**
Returns string containing color of the given team.
*/
string GamePlay::getTeamColor(int index){
    if (index == 1) return "Red";
    else if (index == 2) return "Blue";
    else if (index == 3) return "Green";
    else if (index == 4) return "Yellow";
    else if (index == 5) return "Cyan";
    else if (index == 6) return "Purple";
    else if (index == 7) return "Grey";
    else if (index == 8) return "Black";
    else return "Rainbow";
}

/**
Returns square of distance of given points.
*/
double GamePlay::sqrDistance(Pointf point1, Pointf point2){
    Pointf dist(abs((int)(point1.x - point2.x)),abs((int)(point1.y - point2.y)));
    if (torus){
        if (dist.x > terrain->w / 2) dist.x = terrain->w - dist.x;
        if (dist.y > terrain->h / 2) dist.y = terrain->h - dist.y;
    }
    return sqr(dist.x)+sqr(dist.y);
}

/**
Returns square of distance of given points.
*/
double GamePlay::sqrDistance(double point1x, double point1y, double point2x, double point2y){
    return sqrDistance(Pointf(point1x, point1y), Pointf(point2x, point2y));
}

/**
Returns 'distance' between numbers taking orientation into account with a defined rectangle size.
*/
int GamePlay::distVector(double coord1, double coord2, int size){
    int distance = (int)ceil(coord1) - (int)ceil(coord2);
    if (torus){
        if (distance > size / 2) distance -= size;
        if (distance < -size / 2) distance += size;
    }
    return distance;
}

/**
Returns difference vector of given points with a defined rectangle size.
*/
Point GamePlay::distVector(Pointf pt1, Pointf pt2, Point size){
    return Point(distVector(pt1.x, pt2.x, size.x),
                 distVector(pt1.y, pt2.y, size.y));
}

/**
Returns difference vector of given points.
*/
Point GamePlay::distVector(Pointf pt1, Pointf pt2){
    return distVector(pt1, pt2, mapsize);
}

bool GamePlay::getPoint(SDL_Surface* surf, Point where){
    Uint32 pt = gfx.getPixel(surf, where.x, where.y);
    return ((pt != 0xFF00FFFF) && ((pt & 0x000000FF) != 0));
}

/**
Returns whether the given point contains rock.
*/
bool GamePlay::getSolid(Point where){
    return getPoint(solid, where);
}

/**
Returns whether the given point contains ground.
*/
bool GamePlay::getTerrain(Point where){
    return getPoint(terrain, where);
}

/**
Changes the ground at the given point.
*/
void GamePlay::setTerrain(Point where, int r, int g, int b, int a){
    gfx.setPixel(gameplay.terrain, where.x, where.y, r, g, b, a);
    qtMap->update(where.x, where.y);
}

/**
Makes a hole in the ground and affects robots. (wrapper)
*/
void GamePlay::explode(Pointf where, Robot* inflictor, ShotType* shottype){
    explode(where, shottype->ditch, inflictor, shottype, shottype->splash, shottype->damage, shottype->endamage);
}

/**
Makes a hole in the ground and affects robots.
*/
void GamePlay::explode(Pointf where, int size, Robot* inflictor, ShotType* shottype, int splash, int damage, int endamage){
    gameplay.ditch(where.roundup(), size);
    Player* pl;
    for (unsigned int plid = 0; plid < gameplay.players.size(); plid++){
        pl = gameplay.players[plid];
        if (gameplay.sqrDistance(where, pl->robot->coords) < sqr(ROBOT_R+splash)){
            pl->robot->wasHit(damage, endamage, inflictor, shottype);
        }
    }
}

/**
Makes a hole or incrush in the ground.
*/
void GamePlay::ditch(Point where, int size, bool original){
    int absize = abs(size);

    if (size == 0) return;
    else{
        if (size > 0)
            filledCircleRGBA(terrain, where.x, where.y, absize, 255, 0, 255, 255);
        else if (size < 0)
            filledCircleRGBA(terrain, where.x, where.y, absize, 127, 63, 0, 255);
        /* will update next parts of torus in non-original ditch */
        qtMap->updateRect(where.x-absize, where.y-absize, where.x+absize+1, where.y+absize+1);
    }

    if (torus && original){
        int cornerMask = 0;
        if (where.x > terrain->w - absize) cornerMask |= CORNER_RIGHT;
        if (where.y > terrain->h - absize) cornerMask |= CORNER_BOTTOM;
        if (where.x < absize) cornerMask |= CORNER_LEFT;
        if (where.y < absize) cornerMask |= CORNER_TOP;

        if (cornerMask){
            if (cornerMask & CORNER_RIGHT)
                ditch(Point(where.x-terrain->w, where.y), size, false);

            if (cornerMask & CORNER_BOTTOM)
                ditch(Point(where.x, where.y-terrain->h), size, false);

            if (cornerMask & CORNER_LEFT)
                ditch(Point(where.x+terrain->w, where.y), size, false);

            if (cornerMask & CORNER_TOP)
                ditch(Point(where.x, where.y+terrain->h), size, false);

            if ((cornerMask & CORNER_RIGHT) && (cornerMask & CORNER_BOTTOM))
                ditch(Point(where.x-terrain->w, where.y-terrain->h), size, false);

            if ((cornerMask & CORNER_BOTTOM) && (cornerMask & CORNER_LEFT))
                ditch(Point(where.x+terrain->w, where.y-terrain->h), size, false);

            if ((cornerMask & CORNER_LEFT) && (cornerMask & CORNER_TOP))
                ditch(Point(where.x+terrain->w, where.y+terrain->h), size, false);

            if ((cornerMask & CORNER_TOP) && (cornerMask & CORNER_RIGHT))
                ditch(Point(where.x-terrain->w, where.y+terrain->h), size, false);
        }
    }
}

/**
Blits the given layer of terrain on screen.
*/
void GamePlay::putTerrain(SDL_Surface* matter, SDL_Rect* clip, Point where){

        gfx.paint(matter, where);

        if (!torus) return;

        int cornerMask = 0;
        if (where.x > clip->x) cornerMask |= CORNER_RIGHT;
        if (where.y > clip->y) cornerMask |= CORNER_BOTTOM;
        if (where.x + matter->w < clip->x + clip->w) cornerMask |= CORNER_LEFT;
        if (where.y + matter->h < clip->y + clip->h) cornerMask |= CORNER_TOP;

        if (cornerMask){

                Point whereElse = where;

                if (cornerMask & CORNER_RIGHT){
                        whereElse.x = where.x-matter->w;
                        whereElse.y = where.y;
                        gfx.paint(matter, whereElse);
                }
                if (cornerMask & CORNER_BOTTOM){
                        whereElse.x = where.x;
                        whereElse.y = where.y-matter->h;
                        gfx.paint(matter, whereElse);
                }
                if (cornerMask & CORNER_LEFT){
                        whereElse.x = where.x+matter->w;
                        whereElse.y = where.y;
                        gfx.paint(matter, whereElse);
                }
                if (cornerMask & CORNER_TOP){
                        whereElse.x = where.x;
                        whereElse.y = where.y+matter->h;
                        gfx.paint(matter, whereElse);
                }
                if ((cornerMask & CORNER_RIGHT) && (cornerMask & CORNER_BOTTOM)){
                        whereElse.x = where.x-matter->w;
                        whereElse.y = where.y-matter->h;
                        gfx.paint(matter, whereElse);
                }
                if ((cornerMask & CORNER_BOTTOM) && (cornerMask & CORNER_LEFT)){
                        whereElse.x = where.x+matter->w;
                        whereElse.y = where.y-matter->h;
                        gfx.paint(matter, whereElse);
                }
                if ((cornerMask & CORNER_LEFT) && (cornerMask & CORNER_TOP)){
                        whereElse.x = where.x+matter->w;
                        whereElse.y = where.y+matter->h;
                        gfx.paint(matter, whereElse);
                }
                if ((cornerMask & CORNER_TOP) && (cornerMask & CORNER_RIGHT)){
                        whereElse.x = where.x-matter->w;
                        whereElse.y = where.y+matter->h;
                        gfx.paint(matter, whereElse);
                }
        }
}

/**
Shows score table on the screen.
*/
void GamePlay::putScore(SDL_Rect* clip){
    if (teamscount){
        int r = 255; int g = 255; int b = 255;
        int i = 0;
        boxRGBA(gfx.screen, clip->x + clip->w / 8, clip->y + clip->h / 2 - 48 - 16 * (players.size() + activeteams * 2) / 2,
            clip->x + clip->w / 8 * 7, clip->y + clip->h / 2 + 16 * (players.size() + activeteams * 2) / 2, 0, 0, 0, 191);
        stringRGBA(gfx.screen, clip->x + clip->w / 5 + 16, clip->y + clip->h / 2 - 24 - 16 * (players.size()+activeteams*2) / 2,
                   "Teams & Players", r, g, b, 255);
        stringRGBA(gfx.screen, clip->x + clip->w / 3 * 2 - 4*8, clip->y + clip->h / 2 - 24 - 16 * (players.size()+activeteams*2) / 2,
            "Points", r, g, b, 255);
        stringRGBA(gfx.screen, clip->x + clip->w / 15 * 11, clip->y + clip->h / 2 - 24 - 16 * (players.size()+activeteams*2) / 2,
            "Deaths", r, g, b, 255);
        for (unsigned int teamindex = 0; teamindex < teams.size(); teamindex++) if (teams[teamindex].players > 0){
            if (teams[teamindex].points >= gameplay.limit){ r = 0; g = 255; b = 0; }

            stringRGBA(gfx.screen, clip->x + clip->w / 5 + 16, clip->y + clip->h / 2 - 16 * (players.size()+activeteams*2) / 2 + i * 16,
                       (getTeamColor(teams[teamindex].id)+" team").c_str(), r * 3 / 4, g * 3 / 4, b * 3 / 4, 255);
            stringRGBA(gfx.screen, clip->x + clip->w / 3 * 2, clip->y + clip->h / 2 - 16 * (players.size()+activeteams*2) / 2 + i * 16,
                intToString(teams[teamindex].points).c_str(), r, g, b, 255);
            stringRGBA(gfx.screen, clip->x + clip->w / 15 * 11, clip->y + clip->h / 2 - 16 * (players.size()+activeteams*2) / 2 + i * 16,
                intToString(teams[teamindex].deaths).c_str(), r, g, b, 255);

            for (unsigned int plid = 0; plid < players.size(); plid++) if (players[plid]->teamid == teams[teamindex].id){
                i++;
                stringRGBA(gfx.screen, clip->x + clip->w / 5 + 16, clip->y + clip->h / 2 - 16 * (players.size()+activeteams*2) / 2 + i * 16,
                    players[plid]->name.c_str(), r * 3 / 4, g * 3 / 4, b * 3 / 4, 255);
                stringRGBA(gfx.screen, clip->x + clip->w / 3 * 2, clip->y + clip->h / 2 - 16 * (players.size()+activeteams*2) / 2 + i * 16,
                    intToString(players[plid]->points).c_str(), r, g, b, 255);
                stringRGBA(gfx.screen, clip->x + clip->w / 15 * 11, clip->y + clip->h / 2 - 16 * (players.size()+activeteams*2) / 2 + i * 16,
                    intToString(players[plid]->deaths).c_str(), r, g, b, 255);
            }
            if (teams[teamindex].points >= gameplay.limit){ r = 255; g = 255; b = 255; }
            i+=2;
        }
    }else{
        boxRGBA(gfx.screen, clip->x + clip->w / 8, clip->y + clip->h / 2 - 32 - 16 * players.size() / 2,
            clip->x + clip->w / 8 * 7, clip->y + clip->h / 2 + 16 * (players.size() + 1) / 2, 0, 0, 0, 127);
        int r = 255; int g = 255; int b = 255;
        stringRGBA(gfx.screen, clip->x + clip->w / 5 + 16, clip->y + clip->h / 2 - 16 * players.size() / 2 - 16,
                   "Players", r, g, b, 255);
        stringRGBA(gfx.screen, clip->x + clip->w / 3 * 2 - 4*8, clip->y + clip->h / 2 - 16 * players.size() / 2 - 16,
                   "Points", r, g, b, 255);
        stringRGBA(gfx.screen, clip->x + clip->w / 15 * 11, clip->y + clip->h / 2 - 16 * players.size() / 2 - 16,
                   "Deaths", r, g, b, 255);
        for (unsigned int plid = 0; plid < players.size(); plid++){
            if (players[plid]->points >= gameplay.limit){ r = 0; g = 255; b = 0; }
            stringRGBA(gfx.screen, clip->x + clip->w / 5 + 16, clip->y + clip->h / 2 - 16 * players.size() / 2 + plid * 16,
                players[plid]->name.c_str(), r * 3 / 4, g * 3 / 4, b * 3 / 4, 255);
            stringRGBA(gfx.screen, clip->x + clip->w / 3 * 2, clip->y + clip->h / 2 - 16 * players.size() / 2 + plid * 16,
                intToString(players[plid]->points).c_str(), r, g, b, 255);
            stringRGBA(gfx.screen, clip->x + clip->w / 15 * 11, clip->y + clip->h / 2 - 16 * players.size() / 2 + plid * 16,
                intToString(players[plid]->deaths).c_str(), r, g, b, 255);
            if (players[plid]->points >= gameplay.limit){ r = 255; g = 255; b = 255; }
        }
    }
}

/**
Puts a bordered text line on the screen.
*/
void GamePlay::putLine(int offset, SDL_Rect* clip, string* message, int r, int g, int b){
    int bw = ((r+g+b) > 63 * 3) ? 0 : 127;
    stringRGBA(gfx.screen, clip->x + 5, clip->y + 6 + offset * 16, message->c_str(), bw, bw, bw, 255);
    stringRGBA(gfx.screen, clip->x + 7, clip->y + 6 + offset * 16, message->c_str(), bw, bw, bw, 255);
    stringRGBA(gfx.screen, clip->x + 6, clip->y + 5 + offset * 16, message->c_str(), bw, bw, bw, 255);
    stringRGBA(gfx.screen, clip->x + 6, clip->y + 7 + offset * 16, message->c_str(), bw, bw, bw, 255);
    stringRGBA(gfx.screen, clip->x + 6, clip->y + 6 + offset * 16, message->c_str(),  r,  g,  b, 255);
}

/**
Puts a text on the screen.
*/
void GamePlay::putText(int* p_offset, SDL_Rect* clip, string* message, int r, int g, int b){
    int offset = *p_offset;
    string linep;
    string line = *message;
    int fline = 0;
    int wrapn;
    while (line.length() > 0){
        wrapn = clip->w / LETTER_W - fline - 1;
        linep = line.substr(0, wrapn);
        if (fline) linep = "  "+linep;
        if ((int)line.length() >= wrapn) line = line.substr(wrapn); else line = "";
        putLine(offset, clip, &linep, r, g, b);
        fline = 2; /* indentation of non-first lines */
        offset++;
    }
    *p_offset = offset;
}

/**
Puts chat and log messages on the screen.
*/
void GamePlay::putLog(SDL_Rect* clip, int clipteamid){

    int ii = 0;

    int chi  = chat.size();
    int chii = 5;
    while ((chi > 0) && (chii > 0)){
        chi--;
        if ((!chat[chi].pl) || (chat[chi].pl->teamid==clipteamid)) chii--;
    }

    string chatp;
    for (unsigned int i = chi; i < chat.size(); i++){
        if (chat[i].cl) chatp = (chat[i].pl ? chat[i].pl->name + "@" : "") + chat[i].cl->name + ": " + chat[i].text;
        else chatp = (chat[i].pl ? chat[i].pl->name + ": " : "") + chat[i].text;

        if (!chat[i].pl) putText(&ii, clip, &(chatp), 255, 255, 255);
        else if (chat[i].pl->teamid==clipteamid) putText(&ii, clip, &(chatp), 255, 0, 0);
    }

    int li  = log.size();
    while((li > 0) && (log[li-1].stamp > chronos - (5000 / DELAY))) li--;

    //    while (!log.empty() && (chronos - log.front().stamp > 5000 / DELAY)) log.pop_front();

    for (unsigned int i = li; i < log.size(); i++){
        string logtext;

        if (log[i].killer == log[i].killed)
            logtext = log[i].killed->name + " (" + intToString(log[i].killed->points) + ") killed himself with his "
                + log[i].weapon->name;
        else if (log[i].killer)
            logtext = log[i].killer->name + " (" + intToString(log[i].killer->points) + ") killed "
                + log[i].killed->name + " (" + intToString(log[i].killed->points) + ") with his " + log[i].weapon->name;
        else
            logtext = log[i].killed->name + " (" + intToString(log[i].killed->points) + ") died of energy failure";

        putText(&ii, clip, &(logtext), 127, 255, 127);
    }
}

/**
Puts chat input field on the screen.
*/
void GamePlay::putChat(SDL_Rect* clip){
    string chatc;
    chatc = (chattype == 0 ? "m" : intToString(locals[chattype-1]->teamid))+(">"+message)+"|";
    int wrapn = viewsize / LETTER_W - 3;
    int ii = viewsize / 16 - 2 - message.length() / wrapn;

    if (chattype == 0) putText(&ii, clip, &chatc, 255, 255, 255);
    else putText(&ii, clip, &chatc, 255, 0, 0);
}

/**
Performs shots and robots activity.
*/
void GamePlay::compute(){

    if (ended) return;

    /* Bullets */
    for (Shot* shot = shots->begin(); !shots->end(); shot = shots->next()){
        shot->move();
    }
    for (Shot* shot = shots->begin(); !shots->end(); shot = shots->next()){
        if (shot->forDeletion()){
            delete(shot);
            shots->remove(shot);
        }
    }

    /* Robots */
    for (unsigned int plid = 0; plid < players.size(); plid++){
        /* Manage keyboard */
        players[plid]->setAction();
        /* Robot activity */
        players[plid]->robot->everyLoop();
    }
}

/**
Puts terrain, robots, shots and chat and log messages on screen.
*/
void GamePlay::putGraphics(){

    for (unsigned int otplid = 0; otplid < players.size(); otplid++) if (!players[otplid]->robot->grave) players[otplid]->robot->setStat();

    if (spectators && spectchange){
        SDL_SetClipRect(gfx.screen, 0);
        SDL_FillRect(gfx.screen, 0, SDL_MapRGB(gfx.screen->format, 0, 0, 0));
        spectchange = false;
    }

    Player* pl;
    Robot* rob;
    Robot* otrob;

    Point location;

    for (unsigned int plid = 0; plid < players.size(); plid++) if (((players[plid]->local) && (players[plid]->human)) || (players[plid]->gameview)){
        pl = players[plid];
        rob = pl->robot;

        SDL_SetClipRect(gfx.screen, pl->gameview->viewrect);
        pl->gameview->setpictloc(rob);

        /* Paint terrain */

        if (!torus) SDL_FillRect( gfx.screen, NULL, SDL_MapRGBA(gfx.screen->format, 15, 15, 15, 255) );

        putTerrain( ground, pl->gameview->viewrect, pl->gameview->pictloc);
        putTerrain(terrain, pl->gameview->viewrect, pl->gameview->pictloc);
        putTerrain(  solid, pl->gameview->viewrect, pl->gameview->pictloc);


        /* Paint death stains */
        for (unsigned int otplid = 0; otplid < players.size(); otplid++){
            otrob = players[otplid]->robot;
            location = pl->gameview->middle + distVector(otrob->coords, rob->coords);
            if (otrob->grave < 0)
                filledCircleRGBA(gfx.screen, location.x, location.y, ROBOT_R, 127, 0, 0, (255 - 255 * (otrob->grave + otrob->maxGrave) / otrob->maxGrave));
        }

        /* Paint robots */
        for (unsigned int otplid = 0; otplid < players.size(); otplid++){
            otrob = players[otplid]->robot;

            location = pl->gameview->middle + distVector(otrob->home, rob->coords);

            filledCircleRGBA(gfx.screen, location.x, location.y, ROBOT_R + abs(chronos % 7 - 3),
                abs((chronos * 5) % 256 - 128), abs((chronos * 7) %  256 - 128), abs((chronos * 9) % 256 - 128), 64);

            if (otrob->grave) continue;

            location = pl->gameview->middle + distVector(otrob->coords, rob->coords);

            gfx.paintc(otrob->getImage(), location);

            if (otrob->respawn){
                filledCircleRGBA(gfx.screen, location.x, location.y, ROBOT_R, 0,
                    127 + 12 * (abs(otrob->respawn % 8) - 4), 0, 127);
            }

            if (rob->sight(otrob->coords.roundup(), 0)){
                gfx.paintc(otrob->statimage, location - Point(0, ROBOT_R + otrob->statimage->h / 2));
                gfx.paintc(otrob->nameimage, location + Point(0, ROBOT_R + otrob->nameimage->h / 2));
            }
        }

        /* Paint bullets */
        for (Shot* shot = shots->begin(); !(shots->end()); shot = shots->next()){
            location = pl->gameview->middle + distVector(shot->coords, rob->coords);
            gfx.paintc(shot->getImage(), location);
        }

        /* Critical view coloring */
        if (!rob->grave){
            if (rob->health < rob->maxHealth / 5){
                boxRGBA(gfx.screen, pl->gameview->viewrect->x, pl->gameview->viewrect->y,
                pl->gameview->viewrect->x + pl->gameview->viewrect->w,
                pl->gameview->viewrect->y + pl->gameview->viewrect->h,
                127, 0, 0, (255 - 255 * rob->health / (rob->maxHealth / 5)));
            }

            if (rob->energy < rob->maxEnergy / 5){
                boxRGBA(gfx.screen, pl->gameview->viewrect->x, pl->gameview->viewrect->y,
                    pl->gameview->viewrect->x + pl->gameview->viewrect->w,
                    pl->gameview->viewrect->y + pl->gameview->viewrect->h,
                    0, 0, 0, (255 - 255 * rob->energy / (rob->maxEnergy / 5)));
            }
        }

        putLog(pl->gameview->viewrect, pl->teamid);

        if ((rob->grave) || ended) putScore(pl->gameview->viewrect);

        if (chatting && ((chattype == 0) || (locals[chattype-1] == pl))) putChat(pl->gameview->viewrect);

        pl->displayStatus();
    }

    SDL_Flip(gfx.screen);

}

/**
Adds a message to chat buffer.
*/
bool GamePlay::updateChat(string msg, bool teamonly, Player* pl){
        /* send only non-empty; team messages not allowed if no teams set */
    if ((msg != "") && (!teamonly || teamscount) && (!pl || (msg.length() <= pl->letterlimit))){
        if (pl) pl->letterlimit -= msg.length();
        if (local){
            ChatRec chatrec;
            chatrec.text  = msg;
            chatrec.pl    = teamonly ? pl : 0;
            chatrec.cl    = 0;
            chatrec.stamp = gameplay.chronos;
            chat.push_back(chatrec);
        }else{
            if (teamonly) network.buffer << "gn " << pl->index << " " << network.clientid << " " << msg;
            else network.buffer << "gm " << network.clientid << " " << msg;
            network.send();
        }
        return true;
    }
    return false;
}

/**
Handles pressed key.
*/
void GamePlay::keyPressed(SDL_keysym keysym){

    int sym = keysym.sym; int code = keysym.unicode;

    if (chatting){
        if (sym == SDLK_BACKSPACE) message = message.substr(0,message.length()-1);
        else if ((code >= 0x20) && (code < 0x80)) message += (char)code; /* printable ascii */
        else if (sym == SDLK_ESCAPE){ message = ""; chatting = false; }
        else if (sym == SDLK_RETURN){
            updateChat(message, (chattype != 0), (chattype != 0) ? locals[chattype-1] : 0);
            message = "";
            chatting = false;
        }
        return;
    }

    if (sym==SDLK_m){
        chattype = 0;
        chatting = true;
    }else if ((teamscount != 0) && !spectators){
        if (sym==SDLK_n){
            chattype = 1;
            chatting = true;
        }else if ((localhumans == 2) && (sym==SDLK_c)){
            chattype = 2;
            chatting = true;
        }
    }

    keys[sym] = true;

    if (sym == SDLK_ESCAPE){
        if (!local){
            network.buffer << "gx";
            network.send();
            userface.message = "Game aborted by you";
        }
        quit = true;
    }

    if (sym==SDLK_f){
        gfx.fullscreen = !gfx.fullscreen;
        gfx.setVideoMode();
    }

    if (sym==SDLK_p){
        qtMap->paint();
    }

    if (sym==SDLK_l){
        chat.clear();
        log.clear();
    }

    if (sym==SDLK_r){
        delete(qtMap);
        qtMap = new QuadTreeComposite(terrain, solid);
    }

    if (spectators){
        if (sym==SDLK_BACKSPACE){
            spectchange = true;
            spectators = 3-spectators;
        }
        if (sym==SDLK_RETURN){
            spectchange = true;
            if (players.size() == 2){
                Player* plswap = spectated[0];
                spectated[0] = spectated[1];
                spectated[1] = plswap;
            }else{
                unsigned int spindex = spectated[0]->index;
                while (players[spindex]->spectated){
                    spindex++; if (spindex >= players.size()) spindex = 0;
                }
                spectated[0]->gameview = 0;
                spectated[0]->spectated = false;
                spectated[0] = players[spindex];
                spectated[0]->spectated = true;
            }
        }
        if (sym==SDLK_TAB){
            spectchange = true;
            if (players.size() == 2){
                Player* plswap = spectated[0];
                spectated[0] = spectated[1];
                spectated[1] = plswap;
            }else{
                unsigned int spindex = spectated[1]->index;
                while (players[spindex]->spectated){
                    spindex++; if (spindex >= players.size()) spindex = 0;
                }
                spectated[1]->gameview = 0;
                spectated[1]->spectated = false;
                spectated[1] = players[spindex];
                spectated[1]->spectated = true;
            }
        }
        if (spectchange){
            if (spectators == 1){
                spectated[0]->gameview = gameview[2];
                spectated[1]->gameview = 0;
            }else{
                spectated[0]->gameview = gameview[0];
                spectated[1]->gameview = gameview[1];
            }
        }
    }

}

/**
Sets (and sends) player action according to information about pressed keys.
*/
void GamePlay::manageKeyboard(){

    Player* pl;

    if ((!local) && (chronos % 25 == 0)){
        for (unsigned int plid = 0; plid < players.size(); plid++){
            pl  = players[plid];
            network.buffer << "gt " << chronos << " " << plid
                << " " << pl->robot->coords.x << " " << pl->robot->coords.y << " " << pl->laststamp;
            network.send();
        }
    }

    for (unsigned int plid = 0; plid < players.size(); plid++){
        pl  = players[plid];

        if (pl->local){
            pl->setMask();

            if (!local){
                network.buffer << "gc " << chronos << " " << plid << " " << pl->actionmask;
                network.send();
            }else{
                pl->pushMask(chronos, pl->actionmask);
            }
        }
    }
}

/**
Receives players actions from server.
*/
void GamePlay::updateKeybMasks(){

    if (local) return;

    bool areOnTime;
    bool blocking   = false;
    int  noResponse = 0;
    Player* pl;

    do{
        areOnTime = true;
        network.recv(blocking);
        if (blocking) noResponse+=DELAY;
        for (unsigned int plid = 0; plid < players.size(); plid++){
            pl  = players[plid];
            //if (!pl->local){
                if (pl->laststamp < chronos) areOnTime = false;
                blocking = true;
            //}
        }
        if ((noResponse > 4000) || (network.status < 0)){
            userface.message = "Server did not respond";
            quit = true;
        }
    }while((!areOnTime) && (!quit));


}

/**
Returns milliseconds since game start.
*/
Uint32 GamePlay::getticks(){
    return SDL_GetTicks() - initticks;
}

/**
Waits to keep the correct FPS.
*/
void GamePlay::delay(){

    int duration = getticks() - ticks;

    statscount++;
    statssum += duration;

    int waittime = DELAY - duration;

        //cout << "slow! refresh " << DELAY << "ms;   delay " << -waittime << "ms" << endl;
        //cout << "fast: refresh " << DELAY << "ms; reserve " << waittime << "ms" << endl;

    if (waittime < 0) ticks = getticks() - DELAY; else SDL_Delay(waittime);

    ticks += DELAY;

}

/**
Manages SDL events (mainly keyboard input).
*/
void GamePlay::events(){

    chronos++;

    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch( event.type ){
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
            keyPressed(event.key.keysym);
            break;
        case SDL_KEYUP:
            keys[event.key.keysym.sym] = false;
            break;
            default:
            break;
        }
    }

}

/**
The main game loop. Manages user input, graphics and computing.
*/
void GamePlay::mainLoop(){

    events();

    manageKeyboard();

    putGraphics();

    delay();

    updateKeybMasks();

    compute();

}

/**
Manages the whole game, initializes it, performs it and finishes it.
*/
void GamePlay::theGame(){

    init();

    while (!quit) mainLoop();

    finish();

}
