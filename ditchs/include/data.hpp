#include "datahead.hpp"

bool mute = false, verbose = false;
bool quit = false;

ostringstream buffer;

Uint32 initticks;
/**
Returns milliseconds since the start of server.
*/
Uint32 ticks(){
    return SDL_GetTicks() - initticks;
}

/**
Sends messages to all clients or to all clients not in a game.
*/
void broadCast(bool global){
    verbose && cout << ticks() << ": server broadcasts: .:" << buffer.str() <<":." << endl;
    for (Clients::iterator it = clients.begin(); it != clients.end(); it++) if (it->second != 0){
        if (it->second && ((global) || (it->second->game == 0))) it->second->cast();
    }
    buffer.str("");
}

Game::Game(){
    blob = true;
    players.game = this;
    teams = 0;
    limit = 10;
    started = false;
}

/**
Removes the game from list.
*/
void Game::remove(){
    if (!started){
        buffer << "sr " << id << " " << name;
        broadCast(true);
    }

    for (Clients::iterator it = clients.begin(); it != clients.end(); it++) if (it->second != 0){
        it->second->game = 0;
    }

    games.defer(id);
    games.clean();
}

/**
Removes the given client from the game.
*/
void Game::removeClient(int clid){
    clients[clid]->clearPlayers();
    clients.erase(clid);
    if (clients.size() == 0) remove();
}

/**
Removes the given player from the game.
*/
void Game::removePlayer(int plid, int clid){
    buffer << "l- " << plid;
    clients[clid]->broadCast(false);
    players.remove(players.getIndex(plid), blob);
}

/**
Starts the game.
*/
bool Game::start(){
    int plcnt = 0;
    for (map<int, Player*>::iterator it = ((map<int, Player*>)players).begin(); it != ((map<int, Player*>)players).end(); it++)
        plcnt++;

    if (plcnt < 2) return false;

    started = true;
    buffer << "sr " << id << " " << name;
    broadCast(true);

    sync = new vector<double>(clients.size());
    for (unsigned int i = 0; i < sync->size(); i++) sync->at(i) = 0;
    synced = 0;
    return true;
}

/**
Computes the optimal resolution according to clients' resolution.
*/
void Game::countRes(){
    resx = 100000; resy = 100000;
    for (map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++){
        it->second->countRes();
        resx = (resx > it->second->gameresx) ? it->second->gameresx : resx;
        resy = (resy > it->second->gameresy) ? it->second->gameresy : resy;
    }
}

/**
Checks whether no OOS occurs.
*/
bool Game::syncCheck(int clid, int timestamp, int plid, double coordx, double coordy){
    synced++;
    double record = coordx + coordy;

    int i = 0;
    for (map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++){
        if (it->second->id == clid){
            sync->at(i) += record;
            break;
        }
        i++;
    }

    if (synced == clients.size() * players.WrapMap<Player*>::size()){
        record = sync->at(0);
        int i = 0;
        for (map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++){
            if (record != sync->at(i)){
                return false;
            }
            sync->at(i) = 0;
            i++;
        }
        synced = 0;
        verbose && cout << ticks() << ": game #" << id << " sync ok" << endl;
    }
    return true;
}

/**
Sets players list to limited or unlimited.
*/
void Game::setBlob(){
    if (blob) players.vector<Player*>::clear();
    else players.vector<Player*>::resize(maxplayers);
}

Games::Games() : WrapMap<Game*>(){
    max = MAXGAMES;
}

/**
Adds a game to the list.
*/
Game* Games::add(){
    Game* newG = new Game();
    if (WrapMap<Game*>::add(newG) != 0){
        mute || cout << ticks() << ": game #" << newG->id << " created" << endl;
        return(newG);
    }else{
        free(newG);
        return 0;
    }
}

/**
Removes a game from the list.
*/
void Games::remove(int remid){
    if (find(remid) != end()){
        mute || cout << ticks() << ": game #" << remid << " cancelled" << endl;
        defer(remid);
    }
}

Player::Player(){
        team = 1;
}

Players::Players() : WrapBoth<Player*>(){
    max = MAXPLAYERS;
}

/**
Removes a player to the game.
*/
Player* Players::add(int index, Client* owner){
    Player* newP = new Player();
    if (WrapBoth<Player*>::add(index, newP, game->blob)){
        mute || cout << ticks() << ": player #" << newP->id << " joined" << endl;

        newP->owner = owner;
        owner->players[newP->id] = newP;

        newP->game  = owner->game;
//            if (newP->game != 0) newP->game->players[id] = newP;

        return(newP);
    }else{
        free(newP);
        return 0;
    }
}

Client::Client(TCPsocket newsd){
    sd      = newsd;
    sdopen  = true;
    game    = 0;
}

/**
Sets the game client is going to participate in.
*/
void Client::setGame(Game* newGame){
    if (game != 0){
        game->removeClient(id);
    }

    game = newGame;

    if (game != 0){
        players.clear();
        game->clients[id] = this;
    }
}

/**
Sets the game client is going to participate in by its ID.
*/
bool Client::setGame(int gameid){
    if (gameid == 0){
        Game* gm = 0;
        setGame(gm);
        return true;
    }else if ((games.find(gameid) != games.end()) && (!games[gameid]->started)){
        setGame(games[gameid]);
        return true;
    }else return false;
}

/**
Creates a player at the given index.
*/
Player* Client::addPlayer(int index){
    Player* pl = game->players.add(index, this);
    if (pl != 0) players[pl->id] = pl;
    return pl;
}

/**
Removes all players of the client.
*/
void Client::clearPlayers(){
    if (game != 0){
        for (map<int, Player*>::iterator it = players.begin(); it != players.end(); it++)
            game->removePlayer(it->first, id);
    }
    players.clear();
}

/**
Sends a message to the client.
*/
void Client::cast(){
    int msglen;
    string outdata = buffer.str() + "\n";
    verbose && cout << ticks() << ": server sends to #" << id << ": .:" << outdata << ":." << endl;
    msglen = outdata.length()+1;
    if (SDLNet_TCP_Send(sd, (void *)(outdata.c_str()), msglen) < msglen)
        cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
}

/**
Sends a message to the client only.
*/
void Client::singleCast(){
    cast();
    buffer.str("");
}

/**
Sends a message to clients in the same game.
*/
void Client::broadCast(bool except){
    verbose && cout << ticks() << ": server broadcasts: .:" << buffer.str() << ":." << endl;
    Client* cl;
    for (Clients::iterator it = clients.begin(); it != clients.end(); it++) if (it->second != 0){
        cl = it->second;
        if ((cl->game == game) && ((!except) || (cl != this))) cl->cast();
    }
    buffer.str("");
}

/**
Computes max view size of the given client.
*/
void Client::countRes(){
    int human = 0;
    gameresx = resx; gameresy = resy;

    for (map<int, Player*>::iterator it = players.begin(); it != players.end(); it++){
        if (!it->second->artificial){
            human++;
            if (human == 2){
                gameresx = resx / 2;
            }
        }
    }
}

Clients::Clients() : WrapMap<Client*>(){
    max = MAXCLIENTS;
}

/**
Adds a client to the list.
*/
bool Clients::add(TCPsocket newsd){

    int newid = newID();

    if (newid != 0){
        //modified = true;
        Client* cl = new Client(newsd);
        operator[](newid) = cl;
        cl->id = newid;
        cl->state = cl->CHOOSE;
        mute || cout << ticks() << ": client #" << newid << " joined" << endl;
    };
    return newid;

}

/**
Removes a client from the list.
*/
bool Clients::remove(int remid){
    iterator it = find(remid);
    if (it != end()){
        Client* cl = readyiter->second;
        cl->clearPlayers();
        mute || cout << ticks() << ": client #" << cl->id << " left" << endl;

        if (cl->game != 0){
            cl->game->removeClient(cl->id);
            cl->game = 0;
        }

        defer(cl->id);
        return true;
    }
    return false;
}

/**
Returns the first client that has something to say.
*/
Client* Clients::beginReady(){
    readyiter = begin();
    while ((readyiter != end()) &&
        ((!SDLNet_SocketReady(readyiter->second->sd)) || (readyiter->second == 0))) readyiter++;
    return readyiter->second;
}

/**
Returns the next client that has something to say.
*/
Client* Clients::nextReady(){
    readyiter++;
    while ((readyiter != end()) &&
        ((!SDLNet_SocketReady(readyiter->second->sd)) || (readyiter->second == 0))) readyiter++;
    return readyiter->second;
}

/**
Checks whether noone else has anything to say.
*/
bool Clients::endReady(){
    return readyiter == end();
}


