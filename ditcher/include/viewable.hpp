#ifndef VIEWABLEHEADER
#define VIEWABLEHEADER

#include "basic.hpp"
#include "SDL_image.h"

class Viewable {
    public:
    Pointf coords;
    int angle;
    Pointf direction;
    double speed;
    virtual SDL_Surface* getImage() = 0;
    virtual ~Viewable(); // implemented in robot.cpp (empty)
};

#endif // VIEWABLEHEADER
