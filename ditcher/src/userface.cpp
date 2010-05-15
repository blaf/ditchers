#include "userface.hpp"
#include "menus.hpp"
#include "global.hpp"
#include "network.hpp"
#include "game.hpp"
#include "gfx.hpp"
#include "settings.hpp"

#include "guichan/sdl.hpp"

    /* *** UserFace *** */

UserFace::UserFace(){

}

/**
Switches GUI to message box.
*/
void UserFace::showMessage(string msg){
    if (msg != "") message = msg;
    msgBox->setText(message);
    switchTo(msgBox);
    message = "";
}

/**
Switches GUI to specified menu.
*/
void UserFace::switchTo(Menu* newMenu){

    current->onLeave();
        current->setVisible(false);

    if (newMenu->preceding == NULL)     newMenu->preceding = current;
    if (current->preceding == newMenu)  current->preceding = NULL;

    current = newMenu;

    current->onEnter();
    current->setVisible(true);

    refreshBkg();
}

/**
Hides whole GUI.
*/
void UserFace::hideAll(){
    SDL_FillRect(gfx.screen, 0, SDL_MapRGB(gfx.screen->format, 0, 0, 0));
}

/**
Adds a line to a chat box. Chat level 0 means server-level
communication, chat level 1 means chatting in game lobby.
*/
void UserFace::updateChat(string message, int chatlevel){

    if (chatlevel == 0){
        MenuNetGames* menu = userface.mNetGames;

        if (menu->textChat->getText()!="")
            menu->textChat->setText
                (message+"\n"+menu->textChat->getText());
        else
            menu->textChat->setText(message);
    }else if (chatlevel == 1){
        MenuGameLobby* menu = userface.mGameLobby;

        if (menu->textChat->getText()!="")
            menu->textChat->setText
                (message+"\n"+menu->textChat->getText());
        else
            menu->textChat->setText(message);
    }

}

/**
Sets dimension of the container that keeps the whole GUI.
*/
void UserFace::setContainer(){
        int contW = MENUWIDTH;
        int contH = MENUHEIGHT;
        int contX = (gfx.res.x - contW) / 2;
        int contY = (gfx.res.y - contH) / 2;
        container->setDimension(Rectangle(contX, contY, contW, contH));
        container->setOpaque(false);
}

/**
Calls constructors of all menus used in GUI.
*/
void UserFace::createMenus(){
        container = new Container();

        mMain = new MenuMain();
        mCreateGame = new MenuCreateGame();
        mGameLobby = new MenuGameLobby();
        mConnect = new MenuConnect();
        mGfx = new MenuGfx();
        mNetGames = new MenuNetGames();
        mPlayers = new MenuPlayers();
        mCredits = new MenuCredits();

        msgBox = new MsgBox();
        setContainer();
}

/**
Repaints background image.
*/
void UserFace::refreshBkg(){
    bkgLoc.x = (gfx.res.x - bkg->w) / 2;
    bkgLoc.y = (gfx.res.y - bkg->h) / 2;
    gfx.paint(bkg, bkgLoc);
}

/**
Loads background image from file.
*/
void UserFace::getBkgImage(){
    bkg = gfx.loadImage(settings.loc_bkgimg);

    bkgLoc.x = (gfx.res.x - bkg->w) / 2;
    bkgLoc.y = (gfx.res.y - bkg->h) / 2;
}

/**
Puts something on the screen to avoid black window during initialization.
*/
void UserFace::splash(){
    //    cout << (int)gameplay.getticks() << ": splash" << endl;
    SDL_FillRect(gfx.screen, 0, SDL_MapRGB(gfx.screen->format, 95, 63, 31));
    splashimg = gfx.loadImage(settings.loc_splashimg);
    gfx.paintc(splashimg, Point(gfx.res.x / 2, gfx.res.y / 2));
    SDL_Flip(gfx.screen);
}

/**
Pseudoconstructor of whole GUI.
Creates menus, loads font and background, initializes main menu.
*/
void UserFace::init(){

    imageMap = 0;

        SDL_FreeSurface(splashimg);

        SDLInput* input;
        SDLGraphics* graphics;

        graphics = new SDLGraphics();
        graphics->setTarget(gfx.screen);
        input = new SDLInput();

        gui = new Gui();
        gui->setGraphics(graphics);
        gui->setInput(input);

        SDLImageLoader* imgLoader = new SDLImageLoader();
        Image::setImageLoader(imgLoader);

        ImageFont* font;

    font = new ImageFont(settings.loc_fontimg," abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&`'*#=[]_{}><");
        gcn::Widget::setGlobalFont(font);

        createMenus();

        getBkgImage();
        gui->setTop(container);

        current=mMain;
        switchTo(mMain);

    quit = false;
}

/**
Hides GUI and starts the game.
*/
void UserFace::launchGame(){
    hideAll();
    gameplay.theGame();
    if (gameplay.local){
        switchTo(mMain);
    }else{
        switchTo(mNetGames);
    }
    if (message != "") showMessage();
}

/**
The GUI loop. Handles SDL events and network communication.
*/
void UserFace::theGui(){

    init();

        while (!quit){

                SDL_Event event;
                while(SDL_PollEvent(&event)){
            if (event.type==SDL_QUIT) quit = true;
                        ((SDLInput*)(gui->getInput()))->pushInput(event);
                }

                gui->logic();

                if (!gameplay.local){
                    network.recv(false);
                    if (network.status < 0){
                        switchTo(mConnect);
                        showMessage("Connection to server lost");
                    }
                }
                if (gameplay.approved) launchGame();

                gui->draw();
                SDL_Flip(gfx.screen);

                SDL_Delay(DELAY);

        }

}
