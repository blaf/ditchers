#include "rng.hpp"

/**
Initializes the generator with a hard-coded seed.
*/
RNG::RNG(){
    seed = 511159777;
}

/**
Sets a new seed for the generator.
*/
void RNG::setSeed(Uint32 newSeed){
    seed = newSeed;
}

/**
Computes a new seed and returns a pseudorandom number in given range.
*/
Uint32 RNG::rnd(Uint32 a, Uint32 b){
    seed = (seed * 828033797 + 497372027) % 722973373;
    return (seed % (b - a)) + a;
}
