#ifndef GAMEHEADER
#define GAMEHEADER

#define TERRWIDTH 2048
#define TERRHEIGHT 1536

#include <vector>
#include <deque>

#include <string>
#include <sstream>

#include "basic.hpp"
#include "players.hpp"
#include "rng.hpp"
#include "list.hpp"
#include "global.hpp"
#include "netlists.hpp"

#include "SDL_mixer.h"

#define DELAY 40

class Player;
class Team;
class Robot;
class ShotType;
class Shot;
class QuadTreeComposite;

class GameView{
    public:
    SDL_Rect* viewrect;
    SDL_Rect* statrect;
    SDL_Surface* status;
    Point middle;
    Point pictloc;

    ~GameView();

    void setmiddle();
    void setpictloc(Robot* rob);
};

struct NoteRec{
    int stamp;
};

struct LogRec : NoteRec{
    Player* killer;
    Player* killed;
    ShotType* weapon;
};

struct ChatRec : NoteRec{
    string text;
    Player* pl;
    Client* cl;
};

class GamePlay{
    public:
    Pointf dire[ROTCOUNT];

    int initticks;

    bool blob;
    int chronos;
    Uint32 ticks;
    Uint32 aiticks;
    Uint32 aitime;
    int aiplayers;
    int teamscount;
    int activeteams;
    int limit;
    bool ended;
    Point res;
    vector<Player*> players;

    SDL_Surface* statusraw;
    SDL_Surface* status;
    
    SDL_Surface* ground;
    SDL_Surface* terrain;
    SDL_Surface* solid;
    QuadTreeComposite* qtMap;

    Point mapsize;
    bool torus;
    bool approved;
    bool started;
    bool local;
    int localplayers;
    Player* locals[2];
    int localhumans;
    int viewsize;

    GameView* gameview[3];

    RNG rng;
    bool* keys;
    int rotangle;
    List<Shot*>* shots;
    bool quit;
    PrePlayers preplayers;

    vector<ShotType*> shottypes;

    int spectators;
    bool spectchange;
    Player* spectated[2];

    int chattype;
    bool chatting;
    string message;

    Mix_Chunk* sndexplode;
    Mix_Chunk* sndditch;
    int ditchannel;
    double ditchvolume;
    Mix_Chunk* sndfwd;
    Mix_Chunk* sndbwd;

    vector<Team> teams;

    deque<LogRec>  log;
    deque<ChatRec> chat;

    long statssum;
    long statscount;

    void loadWeapons();
    GamePlay();

    double sqrDistance(Pointf point1, Pointf point2);

    double sqrDistance(double point1x, double point1y, double point2x, double point2y);

    Point distVector(Pointf pt1, Pointf pt2, Point size);

    Point distVector(Pointf pt1, Pointf pt2);

    int distVector(double coord1, double coord2, int size);

    static string getTeamColor(int index);

    void createMap();

    void createClips();

    void importPlayers();

    void setPlayers();

    void startAIs();

    void init();

    void finish();

    void ditch(Point where, int size, bool original = true);

    bool getPoint(SDL_Surface* surf, Point where);

    bool getSolid(Point where);

    bool getTerrain(Point where);

    void setTerrain(Point where, int r, int g, int b, int a);

    void explode(Pointf where, Robot* inflictor, ShotType* shottype);

    void explode(Pointf where, int size, Robot* inflictor, ShotType* shottype, int splash, int damage, int endamage);

    bool updateChat(string msg, bool teamonly, Player* pl);

    Uint32 getticks();

    double volume(Point source);

    bool playSound(Point source, Mix_Chunk* sound);

    void theGame();

    protected:
    void putTerrain(SDL_Surface* terrain, SDL_Rect* clip, Point where);

    void putLine(int offset, SDL_Rect* clip, string* message, int r, int g, int b);

    void putText(int* p_offset, SDL_Rect* clip, string* message, int r, int g, int b);

    void putLog(SDL_Rect* clip, int clipteamid);

    void putScore(SDL_Rect* clip);

    void putChat(SDL_Rect* clip);

    void keyPressed(SDL_keysym keysym);

    void compute();

    void putGraphics();

    void delay();

    void receivePlayersActions();

    void acquirePlayersActions();

    void events();

    void mainLoop();
};

#endif
