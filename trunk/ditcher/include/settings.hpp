#ifndef SETTINGSHEADER
#define SETTINGSHEADER

#include <vector>
#include <string>

using namespace std;

#include "point.hpp"

class RobotType;

class Local{
    public:

    string name;

    RobotType* robottype;

    bool artificial;

    string script;

    int scriptid;

    bool chosen;

    Local();
};

class Base : public Point{
    public:

    int index;
    int type;

    Base(int type);
};

class Map{
    public:

    string dir;
    string name;
    string unique;
    string hash;
    
    bool soilfile;

    bool rockfile;

    bool basefile;

    bool conffile;

    Point size;

    bool torus;
    bool blob;

    int limit;

    vector<Base*> bases;
    int basetype;

    Map(string dirname);

    void computeHash();
};

class Settings{
    protected:
    
    void readSettings();
    
    void readAIs();
    
    void readMaps();
    
    void readRobots();
    
    void readLocals();
    
    public:

    string clientname;
    string hostname;
    int port;

    Point gfxres;
    bool fullscreen;

    vector<RobotType*> robottypes;
    vector<Local*> locals;
    vector<Map*> maps;
    vector<string> ais;

    int defaultai;

    Map* map;

    string gamename;
    string mapunique;
    string maphash;

    void load();

    Settings();

    void writeLocals();

    void writeSettings();
};

#endif // SETTINGSHEADER
