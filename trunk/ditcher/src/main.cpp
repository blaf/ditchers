/**
@mainpage Ditcher
Ditcher is the main part of the Ditchers application. It manages the whole gameplay and GUI.

Main classes are UserFace and GamePlay that care for GUI and gameplay and other important classes are Settings, Gfx and Network.
*/

#include <iostream>
#include <ctime>

#include <fstream>

using namespace std;

#include "main.hpp"

#include "ai.hpp"

#include "settings.hpp"
Settings settings;

#include "gfx.hpp"
Gfx gfx;

#include "userface.hpp"
UserFace userface;

#include "network.hpp"
Network network;

#include "game.hpp"
GamePlay gameplay;

#include "SDL.h"

/**
The main function. The intention is to keep it as lightweight
as possible, only the basic initializations are performed here.
Initializes SDL.
*/
int main(int argc, char *argv[]){

    srand(time(0));

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
            cerr << "SDL_Init: " << SDL_GetError() << "\n";
            exit(EXIT_FAILURE);
    }
    gameplay.initticks = SDL_GetTicks();

    atexit(cleanexit);

    gfx.setVideoMode();

    Uint32 splashticks = gameplay.getticks();
    userface.splash();

    SDL_WM_SetCaption("Ditchers", "ditchers");

    SDL_EnableUNICODE(1);

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    settings.load();

    gameplay.loadWeapons();

    //    cout << (int)gameplay.getticks() << ": loaded" << endl;
    int splashwait = 1000 - (gameplay.getticks() - splashticks);
    if (splashwait > 0) SDL_Delay(splashwait); /* will keep splashscreen displayed for a while */

    userface.theGui();

    exit(0);
}

void cleanexit(){
    SDL_Quit();
    if (!gameplay.local){
        if (gameplay.started){
            network.buffer << "gx";
            network.send();
        }else if (userface.current == (Menu*)userface.mGameLobby){
            network.buffer << "sl";
            network.send();
        }
        network.disconnect();
    }
    //cout << (int)gameplay.getticks() << ": exited" << endl;
}







