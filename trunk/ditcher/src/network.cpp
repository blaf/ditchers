#include <iostream>

#include "network.hpp"
#include "game.hpp"
#include "settings.hpp"
#include "userface.hpp"
#include "players.hpp"
#include "robottype.hpp"

/**
Stores basic network information.
*/
Network::Network(){
    port = 8421;
    hostname = settings.hostname;
    clientname = settings.clientname;
}

/**
Initializes SDL_Net and allocates socket set.
*/
bool Network::init(){

    if (SDLNet_Init() < 0)
    {
        std::cerr << "SDLNet_Init: " << SDLNet_GetError() << "\n";
        return false;
    }

    socketset = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(socketset, socket);

    deaf = false;

    return true;
}

/**
Attempts a connection to the server.
*/
bool Network::connect(){

    if (hostname == "") return false;

    IPaddress serverIP;

    //    cout << "connecting to " << hostname << ":" << port << endl;
    clientid = 0;

    if (SDLNet_ResolveHost(&serverIP, hostname.c_str(), port) < 0){
        cerr << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << endl;
        return false;
    }else if (!(socket = SDLNet_TCP_Open(&serverIP))){
        cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
        return false;
    }
    gameplay.local = false;

    socketset = SDLNet_AllocSocketSet(2);
    SDLNet_TCP_AddSocket(socketset, socket);

    return true;
}

/**
Breaks the connection to the server.
*/
void Network::disconnect(){
    //    std::cout << "disconnected\n";

    SDLNet_TCP_Close(socket);
    gameplay.local = true;
}

/**
Parses the message and sends it to the server.
*/
bool Network::send(){

        /* 'deaf' flag means no sending */
    if (deaf){
        buffer.str("");
        return true;
    }

        /* conversion of stream to a message being sent */
    string outdata(buffer.str());
    //cout << "I say: .:" + outdata + ":.\n";
    buffer.str("");
    outdata += "\n"; /* newline indicates end of message */

    int msglen = outdata.length() + 1;

        /* sending raw c-string */
    if (SDLNet_TCP_Send(socket, (void *)(outdata.c_str()), msglen) < msglen){
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
        return false;
    }
    return true;
}

/**
Extracts information from received messages.
*/
bool Network::parseMessage(){

        /* conversion of received message to stream */
    instream.clear();
    instream.str(msgstring);

        /* first two characters indicate information type */
    instream >> controlmaster >> controlslave;

    switch (controlmaster){

    case 's': /* server global */

        switch (controlslave){

            case 'w': /* all clients */

                clients.clear();

                // ! concatenated cases

            case 'n': /* new clients */

                Client* cln;
                int     idn;

                while (instream.peek() != EOF){
                    instream >> idn;
                    cln = clients.add(idn);
                    instream >> cln->name;
                }

            break;

            case 'i': /* my client id */

                instream >> clientid;

            break;

            case 'q':

                int idq;
                while (instream.peek() != EOF){
                    instream >> idq;
                    clients.erase(idq);
                }

            break;

            case 'j':
                int idj;

                instream >> idj;

                if ((userface.current == (Menu*)userface.mNetGames) && (!games.freeID(idj))){
                    game = games[idj];
                    settings.mapunique = game->mapname;
                    userface.switchTo((Menu*)userface.mGameLobby);
                }

            break;

            case 'm': /* server lobby chat */
            {
                string chatmessage;
                chatmessage.assign(msgstring.substr(3));
                userface.updateChat(chatmessage, 0);
            }
            break;

            case 'g': /* game list update */

                Game*   gmg;
                int     idg;

                games.clear();

                while (instream.peek() != EOF){
                    instream >> idg;
                    gmg = games.add(idg);
                    instream >> gmg->name >> gmg->mapname >> gmg->maphash;
                }

            break;

            case 'a': /* add game */

                Game*   gma;
                int     ida;

                while (instream.peek() != EOF){
                    instream >> ida;
                    gma = games.add(ida);
                    instream >> gma->name >> gma->mapname >> gma->maphash;
                }

            break;

            case 'r': /* remove game */
                int     idr;
                while (instream.peek() != EOF){
                    instream >> idr;
                    games.remove(idr);
                }
            break;

            case 'x':{ /* remove game */
                if (gameplay.started){
                    gameplay.quit = true;
                    userface.message = "Server shut down";
                    disconnect();
                }else{
                    userface.switchTo((Menu*)userface.mConnect);
                    userface.showMessage("Server shut down");
                }
            break;}

        default:
                return false;
            break;
        }

    break;
    case 'l': /* game lobby */

        if (userface.current == (Menu*)userface.mGameLobby)
        switch (controlslave){

            case 't':{ /* teams count change */
                instream >> gameplay.teamscount;
            break;}

            case 'l':{ /* finish game limit */
                instream >> gameplay.limit;
            break;}

            case 'c':{ /* change player team */
                int chtid;
                int chteam;
                instream >> chtid >> chteam;
                PrePlayer* pl = gameplay.preplayers.byID(chtid);
                if (pl != 0) pl->team = chteam;
            break;}

            case 'p':{ /* players count and ditribution in list */
                int id;
                int index;
                int ownid;
                string robottype;
                PrePlayer* pl;

                while (instream.peek() != EOF){
                    instream >> id >> index;
                    pl = gameplay.preplayers.add(id, index);
                    instream >> ownid;
                    pl->setOwner(ownid);
                    instream >> pl->name >> robottype >> pl->team;

                    pl->robottype = settings.robottypes[0];
                    for (unsigned int i=0; i<settings.robottypes.size(); i++)
                        if (robottype == settings.robottypes[i]->unique){
                            pl->robottype = settings.robottypes[i];
                            break;
                        }

                }
            break;}

            case '+':{ /* add player */
                int idadd;
                int indexadd;
                int ownidadd;
                string robottypeadd;
                PrePlayer* pladd;

                while (instream.peek() != EOF){

                    instream >> idadd >> indexadd;
                    pladd = gameplay.preplayers.add(idadd, indexadd);
                    instream >> ownidadd;
                    pladd->setOwner(ownidadd);
                    instream >> pladd->name >> robottypeadd;

                    pladd->robottype = settings.robottypes[0];
                    for (unsigned int i=0; i<settings.robottypes.size(); i++)
                    if (robottypeadd == settings.robottypes[i]->unique){
                        pladd->robottype = settings.robottypes[i];
                        break;
                    }

                    if (ownidadd == clientid){
                        gameplay.localplayers++;
                        for (unsigned int i = 0; i < settings.locals.size(); i++)
                        if ((pladd->name == settings.locals[i]->name) && !settings.locals[i]->chosen){
                            pladd->local = i;
                            settings.locals[i]->chosen = true;
                            if (!settings.locals[i]->artificial) gameplay.localhumans++;
                            break;
                        }
                    }

                }

            break;}

            case '-':{ /* remove player */
                int idrem;

                while (instream.peek() != EOF){
                    instream >> idrem;
                    PrePlayer* pl = gameplay.preplayers.byID(idrem);
                    if ((pl != 0) && (pl->local >= 0)){
                        gameplay.localplayers--;
                        settings.locals[pl->local]->chosen = false;
                        if (!settings.locals[pl->local]->artificial) gameplay.localhumans--;
                    }
                    gameplay.preplayers.remove(gameplay.preplayers.getIndex(idrem), gameplay.blob);
                }
            break;}

            case 's':{ /* game may start; randomseed received */
                gameplay.approved = true;
                int randomseed;
                instream >> randomseed >> gameplay.res.x >> gameplay.res.y;
                gameplay.rng.setSeed(randomseed);
            break;}

            case 'm':{ /* game lobby chat */
                userface.updateChat(msgstring.substr(3), 1);
                break;}

                default:
                return false;
            break;
            }
        break;
        case 'g': /* game */

            if (gameplay.started) switch (controlslave){

            case 'c': /* in-game information about players */
                int timestamp;
                int plindex;
                int keybmask;
                Player* pl;

                instream >> timestamp >> plindex >> keybmask;
                if (gameplay.started){
                    pl = gameplay.players[plindex];
                    pl->pushMask(timestamp, keybmask);
                }
            break;

            case 'm':{ /* in-game chat */
                int clid;
                instream >> clid;

                instream.ignore(1);
                char buf[BUFSIZE];
                instream.get(buf, BUFSIZE);

                ChatRec chatrec;
                chatrec.text  = string(buf);
                chatrec.cl    = clients[clid];
                chatrec.pl    = 0;
                chatrec.stamp = gameplay.chronos;
                gameplay.chat.push_back(chatrec);
            break;}

            case 'n':{ /* in-game team chat */
                int clid, plid;
                instream >> plid >> clid;

                instream.ignore(1);
                char buf[BUFSIZE];
                instream.get(buf, BUFSIZE);

                ChatRec chatrec;
                chatrec.text  = string(buf);
                chatrec.cl    = clients[clid];
                chatrec.pl    = gameplay.players[plid];
                chatrec.stamp = gameplay.chronos;
                gameplay.chat.push_back(chatrec);
            break;}

            case 'x': /* exit game */
                gameplay.quit = true;
                userface.message = msgstring.substr(3);
            break;

            default:
                return false;
            break;
            }
        break;
    }

    if ((controlmaster == 's') || (controlmaster == 'l')){
        deaf = true;
        userface.current->update();
        deaf = false;
    }

    return true;
}

/**
Receives messages from server.
*/
int Network::recv(bool blocking){

    int blockTime = blocking ? DELAY : 0;

    SDLNet_CheckSockets(socketset, blockTime);

    while (SDLNet_SocketReady(socket)){
        if ((msglen = SDLNet_TCP_Recv(socket, buffin, BUFSIZE-1)) > 0){
            for (int i = 0; i < msglen-1; i++){
                if (buffin[i]=='\0') buffin[i]=' ';
            }
            datastring += buffin;
        }else break;
        SDLNet_CheckSockets(socketset, 0);
    }

    if (datastring != ""){
        if (datastring[datastring.length()-1]!='\n') cerr << "CHYBA" << endl;
        //cout << "->:" << endl << datastring << ":<-" << endl;
    }

    while (datastring != ""){

        found = datastring.find_first_of('\n');
        msgstring = datastring.substr(0, found);

        datastring = datastring.substr(found+1);

        //cout << "server says: .:" << msgstring << ":. " << msgstring.length() << "/" << datastring.length() << endl;

        parseMessage();

    }

    return msglen;
}
