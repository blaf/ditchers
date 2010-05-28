#ifndef SETTINGSHEADER
#define SETTINGSHEADER

#include <vector>
#include <string>

using namespace std;

#include "point.hpp"
#include "path.hpp"

class RobotType;

class Base : public Point{
public:
    
    int index;
    int type;
    
    Base(int type);
};

class AIType : public Directory{
    public:
    
    string name;
    
    AIType(string dirname, string pathname);
};

class Map : public Directory{
    public:

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

    Map(string dirname, string pathname);

    void computeHash();
};

class Local{
public:
    
    string name;
    
    RobotType* robottype;
    
    bool artificial;
    
    int scriptid;
    
    bool chosen;
    
    string getScript();
    
    string getScriptPath();
    
    Local();
};

class Settings{
    protected:
    
    void readLocations();
    
    void readSettings();
    
    void readAIs();
    
    void readMaps();
    
    void readRobots();
    
    void readLocals();
    
    Loader loader;
    
    public:
    
    string loc_statusimg;
    string loc_nopreviewimg;
    string loc_sndexplode;
    string loc_sndditch;
    string loc_sndfwd;
    string loc_sndbwd;
    string loc_bkgimg;
    string loc_splashimg;
    string loc_fontimg;
    string loc_weaponsdir;
    string loc_weaponsiconsdir;
    
    string clientname;
    string hostname;
    int port;

    Point gfxres;
    bool fullscreen;
    bool sound;
    
    vector<RobotType*> robottypes;
    vector<Local*> locals;
    vector<Map*> maps;
    vector<AIType*> ais;

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
