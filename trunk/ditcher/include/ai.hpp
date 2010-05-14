extern "C" {
        #include "lua5.1/lualib.h"
        #include "lua5.1/lauxlib.h"
}

#include <string>
using namespace std;

#include "point.hpp"

class Player;

class AI{
    public:

    /* VARIABLES */

    /* bypass */
    static AI* current;

    string script;

    string scriptpath;
    
    lua_State* state;

    Player* owner;

    /* GENERAL */
    AI(string file, string path, Player* player);

    ~AI();

    int start();

    int think();

    /* AUXILIARY */
    void checkargs(int argc);

    bool visible(Point pt, int r);

    bool visiblerect(Point pt1, Point pt2);

    int specrect(Point pt1, Point pt2);

    bool readrect(Point* pt1, Point* pt2);

    int acquireindex();

    int acquireindexgeneral();

    /* GAME SETTINGS */
    static int bounce_getmapinfo(lua_State* luastate);
    int getmapinfo();

    static int bounce_getsight(lua_State* luastate);
    int getsight();

    static int bounce_getrotcount(lua_State* luastate);
    int getrotcount();

    static int bounce_getradius(lua_State* luastate);
    int getradius();

    static int bounce_getmaxgrave(lua_State* luastate);
    int getmaxgrave();

    static int bounce_getmaxprotection(lua_State* luastate);
    int getmaxprotection();

    static int bounce_getshottypescount(lua_State* luastate);
    int getshottypescount();

    static int bounce_getplayerscount(lua_State* luastate);
    int getplayerscount();

    static int bounce_getteamscount(lua_State* luastate);
    int getteamscount();

    static int bounce_getmyid(lua_State* luastate);
    int getmyid();

    static int bounce_getmaxstatus(lua_State* luastate);
    int getmaxstatus();

    /* GAME STATE */
    static int bounce_gettime(lua_State* luastate);
    int gettime();

    static int bounce_gethomes(lua_State* luastate);
    int gethomes();

    static int bounce_getdeaths(lua_State* luastate);
    int getdeaths();

    static int bounce_getshots(lua_State* luastate);
    int getshots();

    static int bounce_getshottype(lua_State* luastate);
    int getshottype();

    static int bounce_putchat(lua_State* luastate);
    int putchat();

    static int bounce_getchat(lua_State* luastate);
    int getchat();

    static int bounce_getlog(lua_State* luastate);
    int getlog();

    static int bounce_getterrain(lua_State* luastate);
    int getterrain();

    static int bounce_gethomogenous(lua_State* luastate);
    int gethomogenous();

    static int bounce_getcoverage(lua_State* luastate);
    int getcoverage();

    static int bounce_getratio(lua_State* luastate);
    int getratio();

    static int bounce_getrockerage(lua_State* luastate);
    int getrockerage();

    static int bounce_getrockratio(lua_State* luastate);
    int getrockratio();

    /* MY ROBOT */
    static int bounce_getmypermanent(lua_State* luastate);
    int getmypermanent();

    static int bounce_getmyinfo(lua_State* luastate);
    int getmyinfo();

    /* ANOTHER ROBOT */
    static int bounce_getpermanent(lua_State* luastate);
    int getpermanent();

    static int bounce_getvisible(lua_State* luastate);
    int getvisible();

    static int bounce_getinfo(lua_State* luastate);
    int getinfo();
};
