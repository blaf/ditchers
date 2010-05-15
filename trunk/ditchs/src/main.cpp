/**
@mainpage Ditchs
Ditchs is the server part of the Ditchers application. It is required only for network game.

It is a lightweight application that does only the necessary work.
*/

#include <string>
#include <sstream>
#include <iostream>
#include <ctime>
#include <csignal>

#include "SDL.h"
#include "SDL_main.h"
#include "SDL_net.h"

#include "data.hpp"

/**
Gathers information about all clients connected to the server and sends it to the given client.
*/
void sendClients(Client* cl){
    /* players ids and names in list */

    buffer << "sw";

    for (Clients::iterator it = clients.begin(); it != clients.end(); it++){
        Client* cl = it->second;
        if ((cl != 0) && (cl->name[0] != '\0')){
            buffer << " " << cl->id << " " << cl->name;
        }
    }

    cl->singleCast();
}

/**
Gathers information about all available games on the server and sends it to the given client.
*/
void sendGames(Client* cl){
    /* games ids and names in list */

    buffer << "sg";

    for (Games::iterator it = games.begin(); it != games.end(); it++){
        Game* gm = it->second;
        if ((gm != 0) && (gm->name[0] != '\0') && (gm->started == false)){
            buffer << " " << gm->id << " " << gm->name << " " << gm->mapname << " " << gm->maphash;
        }
    }

    cl->singleCast();
}

/**
Gathers information about all players in the game and sends it to the given client.
*/
void sendPlayers(Client* cl){
    /* players ids and names in list */

    if (cl->game == 0) return;

    buffer << "lp";
    for (unsigned int i = 0; i < cl->game->players.size(); i++){
        Player* pl = cl->game->players.byIndex(i);
        if (((pl != 0) && pl->name[0] != '\0')){
            buffer << " " << pl->id << " " << pl->index << " " << pl->owner->id
                << " " << pl->name << " " << pl->robottype << " " << pl->team;
        }
    }

    cl->singleCast();
}

/**
Cleanly catches the SIGINT.
*/
void sigcatch(int sig)
{
    quit = true;
    mute || cout << endl;
}

/**
Main function manages the whole work. It initializes the socket, parses messages, adds and removes clients, games and players.
*/
int main(int argc, char *argv[])
{
    TCPsocket acceptsd;
    TCPsocket clientsd;
    IPaddress serverIP;
//    IPaddress *remoteIP;
    SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(MAXCLIENTS+1);

    int port = 8421;
    string arg;
    for(int i = 1; i < argc; i++){
        arg = argv[i];
        if ((arg == "-v") || (arg == "--verbose")) { verbose = true; mute = false; }
        else if ((arg == "-m") || (arg == "--mute")) { verbose = false; mute = true; }
        else if (((arg == "-p") || (arg == "--port")) && (i < argc - 1)){
            int tport = atoi(argv[i+1]);
            if (tport > 1023) port = tport;
            else cerr << "port not set, must be greater than 1023" << endl;
            if (tport > 0) i++;
        }else cerr << "unknown option: " << argv[i] << endl;
    }

    if (SDLNet_Init() < 0)
    {
        cerr << "SDLNet_Init: " << SDLNet_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    /* Resolving the host using NULL make network interface to listen */
    if (SDLNet_ResolveHost(&serverIP, NULL, port) < 0)
    {
        cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    /* Open a connection with the IP provided (listen on the host's port) */
    if (!(acceptsd = SDLNet_TCP_Open(&serverIP)))
    {
        cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    SDLNet_TCP_AddSocket(socketSet, acceptsd);

    (void) signal(SIGINT, sigcatch);
    atexit(SDL_Quit);

    int ready;
    int msglen = 0;
    istringstream instream;
    string datastring;
    string msgstring;
    char controlmaster;
    char controlslave;
    unsigned int found;
    int number;
    int playerid;
    int timestamp;
    char buffin[BUFSIZE];

    srand(time(NULL));
    initticks = ticks();
    mute || cout << ticks() << ": server started at port " << port << endl;

    while (!quit){

        ready = SDLNet_CheckSockets(socketSet, 60);

        if (ready == -1)
            cerr << ticks() << ": SDLNet_CheckSockets: " << SDLNet_GetError() << endl;

        if ((SDLNet_SocketReady(acceptsd)) && (clientsd = SDLNet_TCP_Accept(acceptsd))){
            int clid = clients.add(clientsd);
            if (clid != 0){
                SDLNet_TCP_AddSocket(socketSet, clientsd);
            }else
                mute || cout << ticks() << ": client refused - list full" << endl;
        }

        for(Client* cl = clients.beginReady(); !clients.endReady(); cl = clients.nextReady()){

            while (cl->sdopen && SDLNet_SocketReady(cl->sd)){
                if ((msglen = SDLNet_TCP_Recv(cl->sd, buffin, BUFSIZE-1)) > 0){
                    for (int i = 0; i < msglen-1; i++){
                        if (buffin[i]=='\0') buffin[i]=' ';
                    }
                    datastring += buffin;
                }else cl->sdopen = false;
                SDLNet_CheckSockets(socketSet, 0);
            }

            if (datastring != ""){
                if (datastring[datastring.length()-1]!='\n') cerr << "message broken" << endl;
            }

            while (datastring != ""){

                found = datastring.find_first_of('\n');
                msgstring = datastring.substr(0, found);

                datastring = datastring.substr(found+1);

                verbose && cout << ticks() << ": client #" << cl->id << " says: .:"
                    << msgstring << ":. " << msgstring.length() << "/" << datastring.length() << endl;


                instream.clear();
                instream.str(msgstring);

                instream >> controlmaster >> controlslave;

                switch(controlmaster){
                    case 's':{
                        switch(controlslave){
                            case 'n':{
                                sendClients(cl);
                                sendGames(cl);
                                instream >> cl->name >> cl->resx >> cl->resy;
                                buffer << "sn " << cl->id << " " << cl->name;
                                broadCast(true);
                                buffer << "si " << cl->id;
                                cl->singleCast();
                            break;}
                            case 'c':{
                                Game* newGame;
                                newGame = games.add();
                                if (newGame != 0){
                                    instream >> newGame->name >> newGame->mapname >> newGame->maphash
                                        >> newGame->blob >> newGame->maxplayers;
                                    newGame->setBlob();
                                    cl->setGame(newGame);
                                    buffer << "sa " << newGame->id << " " << newGame->name
                                        << " " << newGame->mapname << " " << newGame->maphash;
                                    broadCast(true);
                                }
                            break;}
                            case 'j':{
                                int gameid;
                                instream >> gameid;
                                if (cl->setGame(gameid)){
                                    buffer << "sj " << gameid;
                                    cl->singleCast();
                                    buffer << "lt " << cl->game->teams;
                                    cl->singleCast();
                                    buffer << "ll " << cl->game->limit;
                                    cl->singleCast();
                                    sendPlayers(cl);
                                }
                            break;}
                            case 'l':{
                                cl->setGame(0);
                            break;}
                            case 'm':{
                                buffer << msgstring;
                                cl->broadCast(false);
                            break;}
                        }
                    break;}
                    case 'l':{
                        switch(controlslave){
                            case 's':{
                                if (cl->game->start()){
                                    cl->game->countRes();
                                    mute || cout << ticks() << ": game #"
                                        << cl->game->id << " started; "
                                        << (int)cl->game->players.WrapMap<Player*>::size() << " players, "
                                        << (int)cl->game->clients.size() << " clients" << endl;
                                    buffer << "ls " << rand() <<
                                        " " << cl->game->resx << " " << cl->game->resy;
                                    cl->broadCast(false);
                                }
                            break;}
                            case 'l':{
                                instream >> cl->game->limit;
                                buffer << "ll " << cl->game->limit;
                                cl->broadCast(false);
                            break;}
                            case 't':{
                                instream >> cl->game->teams;
                                buffer << "lt " << cl->game->teams;
                                cl->broadCast(false);
                            break;}
                            case 'c':{
                                int chtid;
                                int chteam;
                                instream >> chtid >> chteam;
                                if (cl->players.find(chtid) != cl->players.end()){
                                    Player* pl = cl->players[chtid];
                                    pl->team = chteam;
                                    buffer << "lc " << pl->id << " " << pl->team;
                                    cl->broadCast(false);
                                }
                            break;}
                            case '+':{
                                int laddindex;
                                int laddartif;
                                string tempname;
                                string temprobottype;
                                Player* pl;
                                instream >> tempname >> temprobottype >> laddartif >> laddindex;
                                pl = cl->addPlayer(laddindex);
                                if (pl != 0){
                                    pl->artificial = laddartif;
                                    pl->name.assign(tempname);
                                    pl->robottype.assign(temprobottype);

                                    buffer << "l+ " << pl->id << " " << pl->index
                                        << " " << pl->owner->id << " " << pl->name << " " << pl->robottype;
                                }
                                cl->broadCast(false);
                            break;}
                            case '-':{
                                int remid;
                                instream >> remid;
                                if (cl->players.find(remid) != cl->players.end()){
                                    cl->game->removePlayer(remid, cl->id);
                                    cl->players.erase(remid);
                                }
                            break;}
                            case 'm':{
                                buffer << msgstring;
                                cl->broadCast(false);
                            break;}
                        }
                    break;}
                    case 'g':{
                        if (cl->game) switch(controlslave){
                            case 'c':{
                                instream >> timestamp >> playerid >> number;
                                    /* in-game information about players */

                                if (cl->game != 0){
                                    buffer << "gc " << timestamp << " " << playerid << " " << number;
                                    cl->broadCast(false);
                                }
                            break;}
                            case 't':{
                                double coordx, coordy;
                                instream >> timestamp >> playerid >> coordx >> coordy;
                                //if (rand() % 100 == 0) coordx++;
                                if (!cl->game->syncCheck(cl->id, timestamp, playerid, coordx, coordy)){
                                    mute || cout << ticks() << ": *** game #" << cl->game->id
                                        << " OUT OF SYNC ***" << endl;
                                    buffer << "gx Game out of sync";
                                    cl->broadCast(false);
                                    cl->game->remove();
                                }
                            break;}
                            case 'n':
                            case 'm':{
                                buffer << msgstring;
                                cl->broadCast(false);
                            break;}
                            case 'x':{
                                buffer << "gx Game aborted by a client";
                                cl->broadCast(true);
                                cl->game->remove();
                            break;}
                        }
                    break;}
                }
            }

            if (msglen <= 0){
                cl->setGame(0);
                buffer << "sq " << cl->id;
                SDLNet_TCP_DelSocket(socketSet, cl->sd);
                clients.remove(cl->id);
                broadCast(true);
            }

        }

        clients.clean();

    }

    buffer << "sx";
    broadCast(true);

    mute || cout << ticks() << ": server quit" << endl;

    SDLNet_TCP_Close(acceptsd);
    SDLNet_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
