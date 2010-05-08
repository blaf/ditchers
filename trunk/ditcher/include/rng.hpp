#ifndef RNGHEADER
#define RNGHEADER

#include "SDL.h"

class RNG{
    protected:

    Uint32 seed;

    public:

    void setSeed(Uint32 newSeed);

    Uint32 rnd(Uint32 a, Uint32 b);

    RNG();
};

#endif //RNGHEADER
