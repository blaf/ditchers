#ifndef QUADTREEHEADER
#define QUADTREEHEADER

#include "quadtreetemplate.hpp"
#include "SDL.h"

class QuadTreeSurface : public QuadTree<bool>{
    SDL_Surface* source;

    virtual bool decide(int testx, int testy);

    virtual QuadTree<bool>* son(int newx1, int newy1, int newx2, int newy2);

    public:

    QuadTreeSurface(SDL_Surface* src);

    QuadTreeSurface(SDL_Surface* src, int newx1, int newy1, int newx2, int newy2);

    void paint();
};

class QuadTreeComposite : public QuadTree<int>{
    SDL_Surface* mud;
    SDL_Surface* rock;

    int rockerage;

    virtual int decide(int testx, int testy);

    virtual QuadTree<int>* son(int newx1, int newy1, int newx2, int newy2);

    int getrock(int rx1, int ry1, int rx2, int ry2);

    public:

    QuadTreeComposite(SDL_Surface* newmud, SDL_Surface* newrock);

    QuadTreeComposite(SDL_Surface* newmud, SDL_Surface* newrock, int newx1, int newy1, int newx2, int newy2);

    void setcoverage();

    int getrockerage(int rx1, int ry1, int rx2, int ry2);

    double getrockratio(int rx1, int ry1, int rx2, int ry2);

    bool homogenous(int rx1, int ry1, int rx2, int ry2, int val);

    void paint();
};

#endif