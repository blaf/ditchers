#include "robottype.hpp"
#include "robot.hpp"
#include "global.hpp"
#include "gfx.hpp"

map<string, SDL_Surface**> RobotType::imgs;

RobotType::RobotType(string dirname, string pathname){
    path = pathname;
    dir  = dirname;
    movestepscount = 0;
    moveblit = false;
    moveregress = false;
    shotstepscount = new int[9];
    for (int i = 0; i < 9; i++) shotstepscount[i] = 0;
    shotblit = new bool[9];;
    shotsteps = new string*[9];
}

/**
Loads robot type's image and saves all rotational positions.
*/
bool RobotType::acquireImages(){
    map<string,SDL_Surface**>::iterator it;
    for ( it=imgs.begin() ; it != imgs.end(); it++ ){
        string input = (*it).first;
        SDL_Surface* imageLoad = gfx.loadImage(input);
        if (imageLoad == NULL){
            for (int i = 0; i < ROTCOUNT; i++) (*it).second[i] = 0;
        }else{
            gfx.createRotated(imageLoad, (*it).second, ROTCOUNT);
            SDL_FreeSurface(imageLoad);
        }
    }
    return true;
}

void RobotType::putImage(Point middle, int rot, int movestep, int shotindex, int shotstep){
    putImage(gfx.screen, middle, rot, movestep, shotindex, shotstep);
}

void RobotType::putImage(SDL_Surface* onto, Point middle, int rot, int movestep, int shotindex, int shotstep){
    SDL_Surface* imgmoveblit;
    SDL_Surface* imgshotblit;

    bool shotblitnow = (shotstep == 0) || shotblit[shotindex];

    if (movestepscount > 0)
        imgmoveblit = imgs[movesteps[movestep]][rot];
    else
        imgmoveblit = imgs[wholePath()+"/robot.png"][rot];

    if (shotstep > 0){
        string temp = shotsteps[shotindex][shotstep];
        imgshotblit = imgs[shotsteps[shotindex][shotstep]][rot];
    }else
        imgshotblit = imgs[wholePath()+"/robot.png"][rot];

    if (!moveblit) gfx.paintc(imgmoveblit, onto, middle);
    if (!shotblitnow) gfx.paintc(imgshotblit, onto, middle);
    if (moveblit && shotblitnow){
        SDL_Surface* imgbase = imgs[wholePath()+"/robot.png"][rot];
        gfx.paintc(imgbase, onto, middle);
    }

    if (moveblit) gfx.paintc(imgmoveblit, onto, middle);
    if (shotblitnow) gfx.paintc(imgshotblit, onto, middle);
}

SDL_Surface* RobotType::getIcon(){
    SDL_Surface* img = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, ROBOT_R*2+3, ROBOT_R*2+3, 32, 0, 0, 0, 255);
    SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 119, 85, 51));
    putImage(img, Point(img->w / 2 + 1, img->h / 2 + 1), 0, 0, 0, 0);
    return img;
}
