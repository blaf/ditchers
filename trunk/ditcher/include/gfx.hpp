#ifndef GFXHEADER
#define GFXHEADER

#include "basic.hpp"
#include "SDL.h"

class Gfx{
    public:
    SDL_Surface *screen;
    Point res;
    bool fullscreen;

    Gfx();

    bool setVideoMode();

    void paint(SDL_Surface* pict, Point loc);

    void setPixel(SDL_Surface *surf, int x, int y, int red, int green, int blue, int alpha);
    Uint32 getPixel(SDL_Surface *surf, int x, int y);

    SDL_Surface* loadImage(string path, bool alpha = true);

    SDL_Surface* crop(SDL_Surface *tobecropped, SDL_Rect *r);
    void createRotated(SDL_Surface* input, SDL_Surface* output[], int rotNum);

};

#endif // GFXHEADER
