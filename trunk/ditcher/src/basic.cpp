#include "basic.hpp"
#include <sstream>

/**
Converts integer to C++ string.
*/
string intToString(int i){
    stringstream ss;
    string s;
    ss << i;
    s = ss.str();
    return s;
}

/**
Converts C++ string to integer.
*/
int stringToInt(string s){
    istringstream is(s);
    int i;
    is >> i;
    return i;
}

/**
Special signum, returns -1 if zero.
*/
int sgnB(int a){
        return (a > 0) ? 1 : -1;
}

/**
Squares the parameter.
*/
double sqr(double a){
    return a*a;
}

/**
Squares the parameter.
*/
int sqr(int a){
    return a*a;
}

/**
Returns signum of the parameter.
*/
int sgn(int a){
        return (a < 0) ? -1 : ((a = 0) ? 0 : 1);
}

/**
Returns the number put into [0, range).
*/
int modulo(int a, int range){
    a %= range;
    while (a < 0) a += range;
    return a;
}

/**
Checks whether an integer is within specified boundaries
and adjusts its value if necessary.
*/
void bound(int* pWhat, int lower, int upper){
    int what = *pWhat;
    what = (lower > what) ? lower : what;
    what = (upper < what) ? upper : what;
    *pWhat = what;
}
