#ifndef ROBOTTYPEHEADER
#define ROBOTTYPEHEADER

#include <string>
#include <map>

#include "basic.hpp"
#include "path.hpp"

#include "SDL.h"
#include "SDL_image.h"

using namespace std;

class RobotType : public Directory{
    public:

    static map<string, SDL_Surface**> imgs;

    int movestepscount;
    bool moveregress;
    bool moveblit;
    string* movesteps;

    int* shotstepscount;
    bool* shotblit;
    string** shotsteps;

    string name;
    string unique;

    int id;

    RobotType(string dirname, string pathname);

    static bool acquireImages();

    void putImage(Point middle, int rot, int movestep, int shotindex, int shotstep);

    void putImage(SDL_Surface* onto, Point middle, int rot, int movestep, int shotindex, int shotstep);
    
    SDL_Surface* getIcon();
};

#endif // ROBOTTYPEHEADER
