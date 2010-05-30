#include "gfx.hpp"
#include "global.hpp"
#include "settings.hpp"
#include "guichan/sdl/sdlpixel.hpp"
#include "guichan.hpp"

#include "SDL_image.h"
#include "SDL_gfxPrimitives.h"
#include "SDL_rotozoom.h"

/**
Stores information about graphics.
*/
Gfx::Gfx(){
        res = settings.gfxres;
        fullscreen = settings.fullscreen;
}

/**
Loads an image and returns the surface containing it.
*/
SDL_Surface* Gfx::loadImage(string path, bool alpha){
    SDL_Surface* temp = IMG_Load(path.c_str());
    SDL_Surface* img;
    if (alpha){
        img = temp;
        //img = SDL_DisplayFormatAlpha(temp); /* This doesn't work with some SDL versions */
    }else{
        img = SDL_DisplayFormat(temp);
        SDL_FreeSurface(temp);
    }
    return img;
}

bool Gfx::initSDL(){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0){
        return gfx.setVideoMode();
    }else return false;
}

/**
Switches screen/window resolution and fullscreen mode.
*/
bool Gfx::setVideoMode(){
    Uint32 screenFlags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_RLEACCEL;
        if (fullscreen) screenFlags |= SDL_FULLSCREEN;

        screen = SDL_SetVideoMode(res.x, res.y, 32, screenFlags);
        if ( screen == NULL ) {
                if (fullscreen){
                        cerr << "Couldn't set " << res.x << "x" << res.y
                << "x32 FULLSCREEN video mode: " << SDL_GetError() << "; switching to WINDOWED" << endl;
                        fullscreen = false;
                        setVideoMode();
                        return false;
                }else{
                    cerr << "Couldn't set " << res.x << "x" << res.y
                << "x32 WINDOWED video mode: " << SDL_GetError() << "; Fatal error" << endl;
                        exit(1);
                }
        }
        return true;
}

/**
Crops surface to the rectangle and returns it a new surface.
*/
SDL_Surface* Gfx::crop(SDL_Surface *tobecropped, SDL_Rect *r){

    SDL_SetAlpha(tobecropped, 0, SDL_ALPHA_OPAQUE);
    SDL_Surface *croppedx = SDL_CreateRGBSurface(SDL_HWSURFACE, r->w, r->h, 32, 0, 0, 0, 0);
    SDL_Surface *cropped = SDL_DisplayFormatAlpha(croppedx);
    SDL_FillRect(cropped, NULL, SDL_MapRGBA(cropped->format, 0, 0, 0, 0));

    SDL_BlitSurface(tobecropped,r,cropped,NULL);
    return cropped;
}

/**
Rotates input surface and each rotation stores into the returned array.
*/
void Gfx::createRotated(SDL_Surface* input, SDL_Surface* output[], int rotNum){
    SDL_Surface* temp;
    SDL_Rect tempClip;
    tempClip.w = input->w * 3 / 2;
    tempClip.h = input->h * 3 / 2;
    tempClip.x = 0;
    tempClip.y = 0;
    for (int i = 0; i < rotNum; i++){
        temp = rotozoomSurface(input, 360-i*(360/rotNum), 1, 1);
        tempClip.x = (temp->w - tempClip.w) / 2 + 1;
        tempClip.y = (temp->h - tempClip.h) / 2 + 1;
        output[i] = crop(temp, &tempClip);
    }
}

/**
Puts the surface to the specified position on a surface.
*/
void Gfx::paint(SDL_Surface* pict, SDL_Surface* onto, Point loc){
    if (!pict) return;
    SDL_Rect rloc; rloc.x = loc.x; rloc.y = loc.y;
    SDL_BlitSurface(pict, NULL, onto, &rloc);
}

/**
Puts the surface on a surface with its center at the specified position.
*/
void Gfx::paintc(SDL_Surface* pict, SDL_Surface* onto, Point loc){
    if (!pict) return;
    SDL_Rect rloc; rloc.x = loc.x - pict->w/2; rloc.y = loc.y - pict->h/2;
    SDL_BlitSurface(pict, NULL, onto, &rloc);
}

/**
Puts the surface to the specified position on screen.
*/
void Gfx::paint(SDL_Surface* pict, Point loc){
    paint(pict, screen, loc);
}

/**
Puts the surface on screen with its center at the specified position.
*/
void Gfx::paintc(SDL_Surface* pict, Point loc){
    paintc(pict, screen, loc);
}

/**
Sets the value of the given pixel in the given surface.
*/
void Gfx::setPixel(SDL_Surface *surface, int x, int y, int red, int green, int blue, int alpha){
    if (x == surface->w) x -= surface->w;
    if (y == surface->h) y -= surface->h;
    pixelRGBA(surface, x, y, red, green, blue, alpha);
}


/**
Returns the value of the given pixel in the given surface.
*/
Uint32 Gfx::getPixel(SDL_Surface *surface, int x, int y){
    if (x == surface->w) x -= surface->w;
    if (y == surface->h) y -= surface->h;
    if ((x >= surface->w) || (y >= surface->h) || (x < 0) || (y < 0)) return 255;
    gcn::Color col = gcn::SDLgetPixel(surface, x, y);
    return (col.r * 256 * 256 * 256) + (col.g * 256 * 256) + (col.b * 256) + col.a;
}
