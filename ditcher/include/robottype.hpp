#ifndef ROBOTTYPEHEADER
#define ROBOTTYPEHEADER

#include <string>
#include "basic.hpp"
#include "path.hpp"

#include "SDL.h"
#include "SDL_image.h"

using namespace std;

class RobotType : public Directory{
    public:

	SDL_Surface* image[ROTCOUNT];

    string name;
    string unique;

    int id;

    RobotType(string dirname, string pathname);

	bool acquireImage(string input);
};

#endif // ROBOTTYPEHEADER
