#include "players.hpp"
#include "global.hpp"
#include "robot.hpp"
#include "game.hpp"
#include "weapons.hpp"
#include "gfx.hpp"
#include "ai.hpp"
#include "settings.hpp"

#include "SDL_gfxPrimitives.h"

#define ACTION_UP     1
#define ACTION_DOWN   2
#define ACTION_LEFT   4
#define ACTION_RIGHT  8
#define ACTION_FIRE   16
#define ACTION_WEAPON 32

Team::Team(int newid){
    id = newid;
    points = 0;
    deaths = 0;
}

Player::Player(){
    laststamp   = 0;
    robot       = 0;
    ai          = 0;
    controlset  = 0;
    actionmask  = 0;
    points      = 0;
    deaths      = 0;
    gameview    = 0;
    spectated   = false;
    local       = true;
    name        = "";
    letterlimit = 8;
    weaponlock  = 100;
}

/**
Cleans memory of player's robot and eventual AI.
*/
Player::~Player(){
        if (!human) delete(ai);
        delete(robot);
        delete(controlset);
}

/**
Creates AI interpreter.
*/
void Player::initAI(string file, string path){
    ai = new AI(file, path, this);
}

/**
Runs AI script initialization.
*/
int Player::startAI(){
        return ai->start();
}

/**
Sets control keys of human player.
*/
void Player::setControls(int whese){
	PlayerControl* pc;
	if (whese == 0) pc = &settings.controls.single;
	else if (whese == 1) pc = &settings.controls.right;
	else if (whese == 2) pc = &settings.controls.left;
	
    controlset = new KeySet();
    controlset->u = pc->up;
    controlset->d = pc->down;
    controlset->l = pc->left;
    controlset->r = pc->right;
    controlset->fire = pc->fire;
    controlset->weapon = pc->weapon;
    for (int i = 0; i < 10; i++) controlset->w[i] = pc->w[i];
}

/**
Extracts player's action from the actionmask.
*/
bool Player::setAction(){
    if (keybmasks.empty()) actionmask = 0;
    else{
        actionmask = keybmasks.front();
        keybmasks.pop();
    }
    robot->actionset.u      = actionmask & ACTION_UP;
    robot->actionset.d      = actionmask & ACTION_DOWN;
    robot->actionset.l      = actionmask & ACTION_LEFT;
    robot->actionset.r      = actionmask & ACTION_RIGHT;
    robot->actionset.fire   = actionmask & ACTION_FIRE;
    robot->actionset.weapon = actionmask & ACTION_WEAPON;
    return true;
}

/**
Sets the actionmask according to keyboard
or runs AI script to obtain it.
*/
void Player::setMask(){
    actionmask = 0;
    if (gameplay.ended) return;

    if (human){
        if (gameplay.chatting) return;
        if (gameplay.keys[controlset->u]) actionmask |= ACTION_UP;
        if (gameplay.keys[controlset->d]) actionmask |= ACTION_DOWN;
        if (gameplay.keys[controlset->l]) actionmask |= ACTION_LEFT;
        if (gameplay.keys[controlset->r]) actionmask |= ACTION_RIGHT;

        for (unsigned int i = 0; i < 9; i++) 
            if (gameplay.keys[controlset->w[i]]) weaponlock = i;

        if (((weaponlock < 100) && (weaponlock != robot->weapon)) 
            || ((weaponlock == 100) && gameplay.keys[controlset->weapon]))
            { actionmask |= ACTION_WEAPON; gameplay.keys[controlset->weapon] = false; }

        if (weaponlock == robot->weapon) weaponlock = 100;

        if ((weaponlock == 100) && gameplay.keys[controlset->fire])
            actionmask |= ACTION_FIRE;
    }else{
        if (ai) actionmask = ai->think();
    }
}

/**
Displays indicator into the given rectangle.
what: 1 - energy, 2 - health, 3 - reload
*/
void Player::displayIndicator(Point pt1, Point pt2, int what, bool horizontal){
    double fill;
    int steps = 32;
    int r, g, b;

    if (what == 1){
        fill = (double)robot->energy / robot->maxEnergy; if (fill < 0) fill = 0; else if (fill > 1) fill = 1;
        r = 255 - (int)(64*fill); g = 127 + (int)(64*fill); b = 0;
    }else if (what == 2){
        fill = (double)robot->health / robot->maxHealth; if (fill < 0) fill = 0; else if (fill > 1) fill = 1;
        r = 255 - (int)(255*fill); g = (int)(127*fill); b = (int)(191*fill);
    }else if (what == 3){
        if (robot->reload == 0) fill = 1; else fill = 1 - (double)robot->reload / robot->maxReload;
        r = 255-(int)(191*fill); g = (int)(255*fill); b = 0;
    }else return;

    int max;
    double level;

    if (horizontal){
        max = pt2.x - pt1.x;
        level = (pt2.x - pt1.x) * fill;
    }else{
        max = pt2.y - pt1.y;
        level = (pt2.y - pt1.y) * fill;
    }

    double step = (double)max / steps;
    int opaque = (int)floor(floor(level / step) * step);
    double rem = level - opaque;
    double remopac = rem / step;

    if (horizontal){
        if (opaque > 0)   boxRGBA(gfx.screen, pt1.x, pt1.y, pt1.x + opaque - 1, pt2.y,            r, g, b, 127);
        if (rem > 0)     boxRGBA(gfx.screen, pt1.x + opaque, pt1.y, pt1.x + opaque + (int)step - 1, pt2.y, r, g, b, (int)(127 * remopac));
    }else{
        if (level > 0)   boxRGBA(gfx.screen, pt1.x, pt2.y - opaque + 1, pt2.x, pt2.y,            r, g, b, 127);
        if (rem > 0)     boxRGBA(gfx.screen, pt1.x, pt2.y - opaque - (int)step + 1, pt2.x, pt2.y - opaque, r, g, b, (int)(127 * remopac));
    }
}

/**
Displays health, energy, reloading and weapon indicators.
*/
void Player::displayStatus(){
        SDL_SetClipRect(gfx.screen, gameview->statrect);
    gfx.paint(gameview->status, Point(gameview->statrect->x, gameview->statrect->y));

    SDL_Rect rectupper,  rectlower,  rectenergy,  recthealth,  rectreload;
        Point weapoint, smweapoint;


    bool horizontal = gameview->statrect->h <= gameview->statrect->w;

    SDL_Rect horect;
    if (horizontal) horect = *(gameview->statrect);
    else{
        horect.x = gameview->statrect->y;
        horect.y = gameview->statrect->x;
        horect.w = gameview->statrect->h;
        horect.h = gameview->statrect->w;
    }

    rectupper.x = horect.x + horect.w * 3 / 10;
    rectupper.y = horect.y + horect.h / 7;
    rectupper.w = horect.w * 3 / 5;
    rectupper.h = horect.h * 2 / 7;

    rectlower.x = horect.x + horect.w * 3 / 10;
    rectlower.y = horect.y + horect.h * 4 / 7;
    rectlower.w = horect.w * 3 / 5;
    rectlower.h = horect.h * 2 / 7;

    rectreload.x = horect.x + horect.w / 20;
    rectreload.y = horect.y + horect.h / 7;
    rectreload.w = horect.w / 20;
    rectreload.h = horect.h * 5 / 7;

    weapoint.set(horect.x + horect.w / 5,
        horect.y + horect.h / 2);

    if (!horizontal){
        rectenergy.x = rectlower.y;
        rectenergy.y = rectlower.x;
        rectenergy.w = rectlower.h;
        rectenergy.h = rectlower.w;

        recthealth.x = rectupper.y;
        recthealth.y = rectupper.x;
        recthealth.w = rectupper.h;
        recthealth.h = rectupper.w;
        
        SDL_Rect rectreload2 = rectreload;
        rectreload.x = rectreload2.y;
        rectreload.y = rectreload2.x;
        rectreload.w = rectreload2.h;
        rectreload.h = rectreload2.w;
        
        weapoint.set(weapoint.y, weapoint.x);
    }else{
        rectenergy = rectupper;
        recthealth = rectlower;
    }

    displayIndicator(Point(rectenergy.x, rectenergy.y),
                     Point(rectenergy.x + rectenergy.w, rectenergy.y + rectenergy.h), 1, horizontal);

    displayIndicator(Point(recthealth.x, recthealth.y),
                     Point(recthealth.x + recthealth.w, recthealth.y + recthealth.h), 2, horizontal);

    displayIndicator(Point(rectreload.x, rectreload.y),
                     Point(rectreload.x + rectreload.w, rectreload.y + rectreload.h), 3, !horizontal);

    /*    stringRGBA(gfx.screen, rectenergy.x + rectenergy.w / 2 - 23, rectenergy.y + rectenergy.h / 2 - 4, "Energy", 0, 0, 255, 255);

    stringRGBA(gfx.screen, recthealth.x + recthealth.w / 2 - 23, recthealth.y + recthealth.h / 2 - 4, "Health", 191, 191, 0, 255);*/

    SDL_Surface* surfweap   = gameplay.shottypes[robot->weapon]->icon;
    SDL_Surface* surfsmweap = gameplay.shottypes[robot->weapon]->smallicon;
    
    rectangleRGBA(gfx.screen, weapoint.x - surfweap->w / 2 - 1, weapoint.y - surfweap->h / 2 - 1,
                  weapoint.x  + surfweap->w / 2, weapoint.y  + surfweap->h / 2, 63, 63, 63, 255);
    
    if (horizontal){
        for (int i = 1; i <= 4; i++){
            smweapoint.x = weapoint.x - surfsmweap->w / 2;
            smweapoint.y = weapoint.y - surfweap->h / 2 - (5-i)*surfsmweap->h;
            rectangleRGBA(gfx.screen, smweapoint.x - 1, smweapoint.y - 1,
                        smweapoint.x  + surfsmweap->w, smweapoint.y + surfsmweap->h, 63, 63, 63, 255);
            smweapoint.y = weapoint.y + surfweap->h / 2 + (i-1)*surfsmweap->h;
            rectangleRGBA(gfx.screen, smweapoint.x - 1, smweapoint.y - 1,
                        smweapoint.x  + surfsmweap->w, smweapoint.y + surfsmweap->h, 63, 63, 63, 255);
        }
        int ii;
        for (int i = 1; i <= 4; i++){
            smweapoint.x = weapoint.x - surfsmweap->w / 2;
            smweapoint.y = weapoint.y - surfweap->h / 2 - (5-i)*surfsmweap->h;
            ii = (robot->weapon+4+i) % 9;
            surfsmweap = gameplay.shottypes[ii]->smallicon;
            gfx.paint(surfsmweap, smweapoint);
            ii = (robot->weapon+i) % 9;
            smweapoint.y = weapoint.y + surfweap->h / 2 + (i-1)*surfsmweap->h;
            surfsmweap = gameplay.shottypes[ii]->smallicon;
            gfx.paint(surfsmweap, smweapoint);
        }
    }else{
        for (int i = 1; i <= 4; i++){
            smweapoint.x = weapoint.x - surfweap->w / 2 - (5-i)*surfsmweap->w;
            smweapoint.y = weapoint.y - surfsmweap->h / 2;
            rectangleRGBA(gfx.screen, smweapoint.x - 1, smweapoint.y - 1,
                          smweapoint.x  + surfsmweap->w, smweapoint.y + surfsmweap->h, 63, 63, 63, 255);
            smweapoint.x = weapoint.x + surfweap->w / 2 + (i-1)*surfsmweap->w;
            rectangleRGBA(gfx.screen, smweapoint.x - 1, smweapoint.y - 1,
                          smweapoint.x  + surfsmweap->w, smweapoint.y + surfsmweap->h, 63, 63, 63, 255);
        }
        int ii;
        for (int i = 1; i <= 4; i++){
            smweapoint.x = weapoint.x - surfweap->w / 2 - (5-i)*surfsmweap->w;
            smweapoint.y = weapoint.y - surfsmweap->h / 2;
            ii = (robot->weapon+4+i) % 9;
            surfsmweap = gameplay.shottypes[ii]->smallicon;
            gfx.paint(surfsmweap, smweapoint);
            ii = (robot->weapon+i) % 9;
            smweapoint.x = weapoint.x + surfweap->w / 2 + (i-1)*surfsmweap->w;
            surfsmweap = gameplay.shottypes[ii]->smallicon;
            gfx.paint(surfsmweap, smweapoint);
        }
    }

    gameplay.shottypes[robot->weapon]->icon;

    weapoint.x -= surfweap->w / 2;
    weapoint.y -= surfweap->h / 2;

    gfx.paint(surfweap, weapoint);

}
/**
Pushes new actionmask to the queue.
*/
void Player::pushMask(int timestamp, int keybmask){
        keybmasks.push(keybmask);
        laststamp = timestamp;
}
