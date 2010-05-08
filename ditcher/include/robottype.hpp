#ifndef ROBOTTYPEHEADER
#define ROBOTTYPEHEADER

#include <string>
#include "basic.hpp"

#include "SDL.h"
#include "SDL_image.h"

using namespace std;

class RobotType{
    public:

	SDL_Surface* image[ROTCOUNT];

    string dir;
    string name;
    string unique;

    int id;

    RobotType(string dirname);

	bool acquireImage(string input);
};

#endif // ROBOTTYPEHEADER
