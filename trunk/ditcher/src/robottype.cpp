#include "robottype.hpp"
#include "global.hpp"
#include "gfx.hpp"

RobotType::RobotType(string dirname, string pathname){
    path = pathname;
    dir  = dirname;
    acquireImage(path+"/"+dir+"/robot.png");
}

/**
Loads robot type's image and saves all rotational positions.
*/
bool RobotType::acquireImage(string input){
	SDL_Surface* imageLoad = gfx.loadImage(input);
    if (imageLoad == NULL) return false;
	gfx.createRotated(imageLoad, image, ROTCOUNT);
	SDL_FreeSurface(imageLoad);
	return true;
}
