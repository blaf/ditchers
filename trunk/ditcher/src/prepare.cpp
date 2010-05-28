#include "game.hpp"
#include "gfx.hpp"
#include "global.hpp"
#include "settings.hpp"
#include "robot.hpp"
#include "network.hpp"
#include "userface.hpp"
#include "weapons.hpp"
#include "quadtree.hpp"
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

#include "SDL_rotozoom.h"
#include "savepng/IMG_savepng.h"

#include <iostream>
#include <string>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

/**
Initializes all weapon types.
*/
void GamePlay::loadWeapons(){
    //                                  title           file         ene cad spd dit spl  dmg endmg  ttl bnc
    shottypes.push_back(new ShotType("machine gun",      "bullet",    25,  4, 10,  6,  0, 150,    0, 100, 0));
    shottypes.push_back(new ShotType("rear rocket",      "rear",     100, 12, -4, 12, 12, 300,    0, 300, 0));
    shottypes.push_back(new ShotType("sniper rifle",     "sniper",   250, 48, 40,  3,  0, 750,    0, 200, 0));
    shottypes.push_back(new ShotType("flame thrower",    "flame",     20,  1,  8,  0,  8,  50,    0,  12, 0));
    shottypes.push_back(new ShotType("bouncing grenade", "grenade",  150, 16,  6, 10, 18, 400,    0,  80, 3));
    shottypes.push_back(new ShotType("land mine",        "mine",     200, 32,  0, 32, 32, 600,    0, 500, 0));
    shottypes.push_back(new ShotType("incrush",          "earth",    800, 32,  2,-28,  0,   0,    0,  80, 0));
    shottypes.push_back(new ShotType("EMP shockwave",    "emp",      250, 24, 12,  0, 18,  50, 1200,  80, 0));
    shottypes.push_back(new ShotType("recharger",        "recharge", 105,  1, 16,  0,  0,   0, -100,  80, 0));

    statusraw  = gfx.loadImage(settings.loc_statusimg);
    sndexplode = Mix_LoadWAV(settings.loc_sndexplode.c_str());
    sndfwd = Mix_LoadWAV(settings.loc_sndfwd.c_str());
    sndbwd = Mix_LoadWAV(settings.loc_sndbwd.c_str());
}

/**
Creates view clips according to number of players,
graphic resolution and eventual spectator mode.
*/
void GamePlay::createClips(){

    for (int i = 0; i < 3; i++) gameview[i] = new GameView();

    int statsize;
    int clipsize;

    if (res.x > res.y){ viewsize = res.y; clipsize = res.x; }else{ viewsize = res.x; clipsize = res.y; }
    statsize = clipsize - viewsize;

        gameview[2]->viewrect     = new SDL_Rect();
        gameview[2]->viewrect->w  = viewsize;
        gameview[2]->viewrect->h  = viewsize;
        gameview[2]->viewrect->x  = (gfx.res.x - clipsize) / 2;
        gameview[2]->viewrect->y  = (gfx.res.y - viewsize) / 2;

        gameview[2]->statrect     = new SDL_Rect();
        gameview[2]->statrect->w  = statsize;
        gameview[2]->statrect->h  = viewsize;
        gameview[2]->statrect->x  = gameview[2]->viewrect->x + viewsize;
        gameview[2]->statrect->y  = gameview[2]->viewrect->y;

    int viewsize2; /* avoids view shrinking if spectating */

    if (spectators && (res.x > gfx.res.x / 2)){
        Point res2 = res;
        res2.x = gfx.res.x / 2;
        if (res2.x > res2.y){ viewsize2 = res2.y; clipsize = res2.x; }else{ viewsize2 = res2.x; clipsize = res2.y; }
        statsize = clipsize - viewsize2;
    }else{
        viewsize2 = viewsize;
    }

        gameview[0]->viewrect     = new SDL_Rect();
        gameview[0]->viewrect->w  = viewsize2;
        gameview[0]->viewrect->h  = viewsize2;
        gameview[0]->viewrect->x  = gfx.res.x / 2 + (gfx.res.x / 2 - viewsize2) / 2;
        gameview[0]->viewrect->y  = (gfx.res.y - clipsize) / 2;

        gameview[1]->viewrect     = new SDL_Rect();
        gameview[1]->viewrect->w  = viewsize2;
        gameview[1]->viewrect->h  = viewsize2;
        gameview[1]->viewrect->x  = (gfx.res.x / 2 - viewsize2) / 2;
        gameview[1]->viewrect->y  = (gfx.res.y - clipsize) / 2;

        gameview[0]->statrect     = new SDL_Rect();
        gameview[0]->statrect->w  = gameview[0]->viewrect->w;
        gameview[0]->statrect->h  = statsize;
        gameview[0]->statrect->x  = gameview[0]->viewrect->x;
        gameview[0]->statrect->y  = gameview[0]->viewrect->y + viewsize2;

        gameview[1]->statrect     = new SDL_Rect();
        gameview[1]->statrect->w  = gameview[1]->viewrect->w;
        gameview[1]->statrect->h  = statsize;
        gameview[1]->statrect->x  = gameview[1]->viewrect->x;
        gameview[1]->statrect->y  = gameview[1]->viewrect->y + viewsize2;

    for (int i = 0; i < 3; i++){
        gameview[i]->setmiddle();
        int angle; double ratiox, ratioy;
        if (i == 2){
            angle = 270;
            ratiox = (double)gameview[i]->statrect->w / statusraw->h;
            ratioy = (double)gameview[i]->statrect->h / statusraw->w;
            SDL_Surface* temp = rotozoomSurface(statusraw, angle, 1, 0);
            SDL_Surface* temp2 = rotozoomSurfaceXY(temp, 0, ratiox, ratioy, 0);
            SDL_FreeSurface(temp);
            SDL_Rect crop = *(gameview[i]->statrect);
            crop.x = temp2->w - gameview[i]->statrect->w;
            crop.y = temp2->h - gameview[i]->statrect->h;
            gameview[i]->status = gfx.crop(temp2, &crop);
            SDL_FreeSurface(temp2);
        }else{
            angle = 0;
            ratiox = (double)gameview[i]->statrect->w / statusraw->w;
            ratioy = (double)gameview[i]->statrect->h / statusraw->h;
            gameview[i]->status = rotozoomSurfaceXY(statusraw, angle, ratiox, ratioy, 0);
        }
    }

}

/**
Creates the map according to settings.map information.
Creates a map preview if required.
*/
void GamePlay::createMap(){

    string pathfile;

    gameplay.torus = settings.map->torus;

    mapsize = settings.map->size;

    SDL_Rect tiler;

    SDL_Surface* temp;

    if (settings.map->rockfile){
        pathfile = settings.map->wholePath()+"/rock.png";
        temp = gfx.loadImage(pathfile, false);
        if (mapsize.x == 0) mapsize.x = temp->w;
        if (mapsize.y == 0) mapsize.y = temp->h;
        solid=SDL_CreateRGBSurface(SDL_HWSURFACE, mapsize.x, mapsize.y, 32, 0, 0, 0, 0);
        for (int i = 0; i < (mapsize.x-1)/temp->w+1; i++)
            for (int j = 0; j < (mapsize.y-1)/temp->h+1; j++){
                tiler.x = temp->w * i; tiler.y = temp->h * j;
                SDL_BlitSurface(temp, NULL, solid, &tiler);
            }
        SDL_SetColorKey(solid, SDL_SRCCOLORKEY, SDL_MapRGBA( solid->format, 255, 0, 255, 255 ));
    }
    if (settings.map->soilfile){
        pathfile = settings.map->wholePath()+"/soil.png";
        temp = gfx.loadImage(pathfile, false);
        if (mapsize.x == 0) mapsize.x = temp->w;
        if (mapsize.y == 0) mapsize.y = temp->h;
        terrain=SDL_CreateRGBSurface(SDL_HWSURFACE, mapsize.x, mapsize.y, 32, 0, 0, 0, 0);
        for (int i = 0; i < (mapsize.x-1)/temp->w+1; i++)
            for (int j = 0; j < (mapsize.y-1)/temp->h+1; j++){
                tiler.x = temp->w * i; tiler.y = temp->h * j;
                SDL_BlitSurface(temp, NULL, terrain, &tiler);
            }
        SDL_SetColorKey(terrain, SDL_SRCCOLORKEY, SDL_MapRGBA( terrain->format, 255, 0, 255, 255 ));
    }
    if (settings.map->basefile){
        pathfile = settings.map->wholePath()+"/base.png";
        temp = gfx.loadImage(pathfile, false);
        if (mapsize.x == 0) mapsize.x = temp->w;
        if (mapsize.y == 0) mapsize.y = temp->h;
        ground=SDL_CreateRGBSurface(SDL_HWSURFACE, mapsize.x, mapsize.y, 32, 0, 0, 0, 0);
        for (int i = 0; i < (mapsize.x-1)/temp->w+1; i++)
            for (int j = 0; j < (mapsize.y-1)/temp->h+1; j++){
                tiler.x = temp->w * i; tiler.y = temp->h * j;
                SDL_BlitSurface(temp, NULL, ground, &tiler);
            }
    }

    if (mapsize.x == 0) mapsize.x = 800;
    if (mapsize.y == 0) mapsize.y = 800;

    if (!settings.map->basefile){
        ground=SDL_CreateRGBSurface(SDL_HWSURFACE, mapsize.x, mapsize.y, 32, 0, 0, 0, 0);
        boxRGBA(ground, 0, 0, ground->w, ground->h, 63, 31, 0, 255);
    }

    if (!settings.map->soilfile){
        terrain=SDL_CreateRGBSurface(SDL_HWSURFACE, mapsize.x, mapsize.y, 32, 0, 0, 0, 0);
        boxRGBA(terrain, 0, 0, terrain->w, terrain->h, 127, 63, 0, 255);
        SDL_SetColorKey(terrain, SDL_SRCCOLORKEY, SDL_MapRGBA( terrain->format, 255, 0, 255, 255 ));
    }

    if (!settings.map->rockfile){
        solid=SDL_CreateRGBSurface(SDL_HWSURFACE, mapsize.x, mapsize.y, 32, 0, 0, 0, 0);
        boxRGBA(solid, 0, 0, terrain->w, terrain->h, 255, 0, 255, 255);
        SDL_SetColorKey(solid, SDL_SRCCOLORKEY, SDL_MapRGBA( solid->format, 255, 0, 255, 255 ));
    }

    string previewpath = settings.map->wholePath()+"/preview.png";
    if (!fs::exists(previewpath)){
        int biggersize = ((mapsize.x > mapsize.y) ? mapsize.x : mapsize.y);
        Sint16 prevx = (biggersize - mapsize.x) / 2;
        Sint16 prevy = (biggersize - mapsize.y) / 2;
        SDL_Rect posrect = {prevx, prevy, 0, 0};
        int prevsize = 96;
        double ratio = ((double)(prevsize - 2)) / biggersize;
        SDL_Surface* composed =SDL_CreateRGBSurface(SDL_HWSURFACE, biggersize, biggersize, 32, 0, 0, 0, 0);
        SDL_BlitSurface(ground, 0, composed, &posrect);
        SDL_BlitSurface(terrain, 0, composed, &posrect);
        SDL_BlitSurface(solid, 0, composed, &posrect);
        SDL_Surface* prepreview = rotozoomSurface(composed, 0, ratio, 1);
        SDL_FreeSurface(composed);
        SDL_Surface* preview = SDL_CreateRGBSurface(SDL_HWSURFACE, prevsize, prevsize, 32, 0, 0, 0, 0);
        SDL_Rect framerect = {1, 1, 0, 0};
        SDL_BlitSurface(prepreview, 0, preview, &framerect);
        SDL_FreeSurface(prepreview);
        IMG_SavePNG(previewpath.c_str(), preview, 9);
        SDL_FreeSurface(preview);
    }

}

/**
Sets controls, creates homes and sets eventual spectatorship.
*/
void GamePlay::setPlayers(){

    Player* pl;
    Robot*  rob;

    int hum = 0;

    for (unsigned int plid = 0; plid < players.size(); plid++){
        pl  = players[plid];

        if (!pl->robot){
            pl->robot = new Robot(pl);
        }

        if (pl->local){
            if (hum >= 2) pl->human = false; /* should never happen */

            if (pl->human){
                if (localhumans == 1) pl->gameview = gameview[2]; else pl->gameview = gameview[hum];
                locals[hum] = pl;
                if (localhumans == 1){
                    if (hum == 0) pl->setControls(SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN,
                        SDLK_BACKSPACE, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9);
                }else{
                    if (hum == 0) pl->setControls(SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN,
                        SDLK_BACKSPACE, SDLK_RSHIFT, SDLK_INSERT, SDLK_DELETE, SDLK_HOME, SDLK_PAGEUP, 
                        SDLK_PAGEDOWN, SDLK_END, SDLK_LEFTBRACKET, SDLK_RIGHTBRACKET);
                    if (hum == 1) pl->setControls(SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_TAB, SDLK_q,
                        SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9);
                }
                hum++;
            }
        }

        rob = pl->robot;
        int basetype;
        if ((plid < settings.map->bases.size()) && (settings.map->bases[pl->order]->x >= 0)){
            rob->home.set(settings.map->bases[pl->order]->x, settings.map->bases[pl->order]->y);
            rob->coords = rob->home;
            basetype = settings.map->bases[pl->order]->type;
        }else{
            bool homegen;
            int attempts = 0;
            Point home;
            Pointf homef;
            do{
                attempts++;
                if (gameplay.torus){
                    home.x = (rng.rnd(0, terrain->w));
                    home.y = (rng.rnd(0, terrain->h));
                }else{
                    home.x = (rng.rnd(80 + 2*ROBOT_R, terrain->w - 80 - 2*ROBOT_R));
                    home.y = (rng.rnd(80 + 2*ROBOT_R, terrain->h - 80 - 2*ROBOT_R));
                }
                homef = home.to<double>();
                homegen = true;
                for (unsigned int i = 0; i < plid; i++)
                    if (sqrDistance(homef, players[i]->robot->coords) < sqr(200)) homegen = false;
                if ((!homegen)
                    || gameplay.getSolid(Point(home.x - 81, home.y))
                    || gameplay.getSolid(Point(home.x + 81, home.y))
                    || gameplay.getSolid(Point(home.x, home.y - 81))
                    || gameplay.getSolid(Point(home.x, home.y + 81))
                    || gameplay.getSolid(Point(home.x - 81, home.y - ROBOT_R/2))
                    || gameplay.getSolid(Point(home.x + 81, home.y - ROBOT_R/2))
                    || gameplay.getSolid(Point(home.x - ROBOT_R/2, home.y - 81))
                    || gameplay.getSolid(Point(home.x - ROBOT_R/2, home.y + 81))
                    || gameplay.getSolid(Point(home.x - 81, home.y + ROBOT_R/2))
                    || gameplay.getSolid(Point(home.x + 81, home.y + ROBOT_R/2))
                    || gameplay.getSolid(Point(home.x + ROBOT_R/2, home.y - 81))
                    || gameplay.getSolid(Point(home.x + ROBOT_R/2, home.y + 81)))
                    homegen = false;
            }while(!homegen && (attempts < 50));
            rob->home = homef;
            rob->coords = home.to<double>();
            basetype = settings.map->basetype;
        }
        if (basetype) rob->createHome(basetype);

    }

    if (spectators){
        players[0]->gameview = gameview[2];
        players[0]->gameview->setpictloc(players[0]->robot);
        for (unsigned int i=0; i<2; i++){
            players[i]->spectated = true;
            spectated[i] = players[i];
        }
        spectchange = false;
    }
}

/**
Initializes AI scripts.
*/
void GamePlay::startAIs(){
    for (unsigned int plid = 0; plid < players.size(); plid++){
        if (!players[plid]->human && players[plid]->ai) players[plid]->startAI();
    }
}

/**
Converts lobby players to game players.
*/
void GamePlay::importPlayers(){
    PrePlayer* ppl;
    Player* pl;

    activeteams = 0;
    for(unsigned int i = 0; i < teams.size(); i++){
        teams[i].points = 0;
        teams[i].deaths = 0;
        teams[i].players = 0;
    }

    players.clear();
    aiplayers = 0;
    for (unsigned int i = 0; i < preplayers.size(); i++) if (preplayers.byIndex(i)){

        ppl = preplayers.vector<PrePlayer*>::at(i);
        pl = new Player();

        pl->order = i;
        pl->index = players.size();
        pl->id  = ppl->id;

        pl->name = ppl->name;
        pl->robottype = ppl->robottype;

        pl->local = (ppl->local >= 0);

        if (teamscount){
            pl->teamid = ppl->getTeam();
            pl->team = &(teams[pl->teamid-1]);
            pl->team->players++;
            if (pl->team->players == 1) activeteams++;
        }else{
            pl->teamid = pl->index;
            pl->team = 0;
        }

        if (pl->local) pl->human = !settings.locals[ppl->local]->artificial;
        else pl->human = true;
        if (!pl->human){
            aiplayers++;
            if (settings.locals[ppl->local]->scriptid >= 0)
                pl->initAI(settings.locals[ppl->local]->getScript(), settings.locals[ppl->local]->getScriptPath());
            else cerr << "error: no script directory \""
                << settings.locals[ppl->local]->getScript() << "\" exists" << endl;
        }

        players.push_back(pl);
    }

}

/**
Creates clips, map, prepares players.
*/
void GamePlay::init(){

    quit = false;

    shots = new List<Shot*>(1024);

    log.clear();
    chat.clear();

    importPlayers();

    if (local){
        rng.setSeed((Uint32)time(0));
        res = gfx.res;
        if (localhumans == 2) res.x /= 2;
    }

    spectators = (localhumans == 0) ? 1 : 0;

    createMap();

    if (quit) return;

    createClips();

    setPlayers();

    qtMap = new QuadTreeComposite(terrain, solid);

    SDL_ShowCursor(SDL_DISABLE);

    for (int i = 0; i <= SDLK_LAST; i++) keys[i] = false;

    ticks = getticks();
    aiticks = ticks;

    chronos = 0;
    started = true;

    startAIs();

    statssum = 0;
    statscount = 0;
}

/**
Cleans memory and resets game variables.
*/
void GamePlay::finish(){

    //    cout << "Average loop duration: " << (double)statssum / statscount << " ms" << endl;

    /* remove all shots */
    for (Shot* shot = shots->begin(); !shots->end(); shot = shots->next()){
        delete(shot);
        shots->remove(shot);
    }
    delete(shots);

    /* remove all players */
    for (unsigned int i = 0; i < players.size(); i++) delete(players[i]);
    players.clear();

    for (int i = 0; i < 3; i++) delete(gameview[i]);

    /* remove map quadtree */
    delete(qtMap);

    /* remove map surfaces */
    SDL_FreeSurface(ground);
    SDL_FreeSurface(terrain);
    SDL_FreeSurface(solid);

    SDL_FreeSurface(status);

    SDL_ShowCursor(SDL_ENABLE);
    quit = false;

    /* reset view */
    SDL_Rect allClip;
    allClip.x = 0; allClip.y = 0; allClip.w = gfx.res.x; allClip.h = gfx.res.y;
    SDL_SetClipRect(gfx.screen, &allClip);

    /* reset game variables */
    approved = false;
    ended = false;
    started = false;

}
