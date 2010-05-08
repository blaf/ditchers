#ifndef BASICHEADER
#define BASICHEADER

#define PI 3.141592654

#define ROTCOUNT 40

#include "point.hpp"
#include <string>

using namespace std;

string intToString(int i);
int stringToInt(string s);

int sgnB(int a);
double absF(double a);
int sqr(int a);
double sqr(double a);

int sgn(int a);

int modulo(int a, int range);

void bound(int* pWhat, int lower, int upper);

#endif
