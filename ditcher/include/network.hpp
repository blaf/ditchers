#ifndef NETWORKHEADER
#define NETWORKHEADER

#include "netlists.hpp"

#include <string>
#include <sstream>

#include "SDL_net.h"

#define BUFSIZE 1024

#define MAXGAMES    128
#define MAXCLIENTS  1024
#define MAXPLAYERS  2048

class Network{
    protected:
    istringstream instream;
    string datastring;
    string msgstring;
    char controlmaster;
    char controlslave;
    unsigned int found;
    char buffin[BUFSIZE];
    TCPsocket socket;
    SDLNet_SocketSet socketset;
    int msglen;
    bool parseMessage();
    bool init();
    bool send(ostringstream* outstream);

    public:
    string hostname;
    string clientname;
    int port;
    int clientid;
    ostringstream buffer;
    int status;
    bool inited;
    bool deaf;
    Game* game;
    Games games;
    Clients clients;

    Network();

    bool connect();

    void disconnect();

    bool send();

    int recv(bool blocking);

};

#endif
