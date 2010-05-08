#include <iostream>
using namespace std;

#include "netlists.hpp"
#include "network.hpp"
#include "game.hpp"
#include "global.hpp"

Game::Game(){
}

Games::Games() : WrapMap<Game*>(){
    max = MAXGAMES;
}

Game* Games::add(){
    Game* newG = new Game();
    if (WrapMap<Game*>::add(newG) != 0){
        return(newG);
    }else{
        delete(newG);
        return 0;
    }
}

Game* Games::add(int newid){
    Game* newG = new Game();
    if (WrapMap<Game*>::add(newid, newG) != 0){
        return(newG);
    }else{
        delete(newG);
        return 0;
    }
}

void Games::remove(int remid){
    if (find(remid) != end()){
        defer(remid);
        clean();
    }
}

Game* Games::atIndex(int index){
    int i = 0;
    iterator it = begin();
    while ((i < index) && (it != end())){ it++; i++; }
    if (it != end()) return it->second; else return 0;
}

PrePlayer::PrePlayer(){
    local = -1;
    team  = 1;
}

bool PrePlayer::setOwner(int ownid){
    if (network.clients.find(ownid) != network.clients.end()){
        owner = network.clients[ownid];
        return true;
    }else return false;
}

int PrePlayer::getTeam(){
    if (team <= gameplay.teamscount) return team; else return 1;
}

PrePlayers::PrePlayers() : WrapBoth<PrePlayer*>(){
    max = MAXPLAYERS;
}

PrePlayer* PrePlayers::add(int newid, int index){
    PrePlayer* newP = new PrePlayer();
    if (WrapBoth<PrePlayer*>::add(index, newid, newP, gameplay.blob) != 0){
        return(newP);
    }else{
        delete(newP);
        return 0;
    }
}

unsigned int PrePlayers::size(){
    return vector<PrePlayer*>::size();
}

void PrePlayers::clear(){
    vector<PrePlayer*>::clear();
    WrapMap<PrePlayer*>::clear();
}

Client::Client(){
    game = 0;
}

Clients::Clients() : WrapMap<Client*>(){
    max = MAXCLIENTS;
}


Client* Clients::add(){
    Client* newC = new Client();
    if (WrapMap<Client*>::add(newC) != 0){
        return(newC);
    }else{
        delete(newC);
        return 0;
    }
}

Client* Clients::add(int newid){
    Client* newC = new Client();
    if (WrapMap<Client*>::add(newid, newC) != 0){
        return(newC);
    }else{
        delete(newC);
        return 0;
    }
}

bool Clients::remove(int remid){
    iterator it = find(remid);
    if (it != end()){
        Client* cl = readyiter->second;
        defer(cl->id);
        return true;
    }
    return false;
}

Client* Clients::atIndex(int index){
    int i = 0;
    iterator it = begin();
    while ((i < index) && (it != end())){ it++; i++; }
    if (it != end()) return it->second; else return 0;
}
