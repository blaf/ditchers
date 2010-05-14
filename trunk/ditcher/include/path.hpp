#ifndef PATHHEADER
#define PATHHEADER

#include <vector>
#include <string>
using namespace std;

class Directory{
    public:
    string path;
    string dir;

    string wholePath();
};

class Loader{
public:
    vector<string> paths;
    
    string getHomeDitchers();
    
    string locateFile(string file);
    
    string locateDir(string dir);
    
    Loader();
};

#endif