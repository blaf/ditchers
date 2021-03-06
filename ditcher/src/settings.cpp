#include "settings.hpp"
#include "global.hpp"
#include "robottype.hpp"
#include "tinyxml.h"
#include "md5/hl_md5wrapper.h"
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

#include <iostream>
using namespace std;

#include <string>

AIType::AIType(string dirname, string pathname){
    dir  = dirname;
    path = pathname;
    name = dir;
}

Local::Local(){
    name = "newplayer";

    robottype = settings.robottypes[0];

    scriptid = -1;

    artificial = false;

    chosen = false;
}

string Local::getScript(){
    return settings.ais[scriptid]->dir;
}

string Local::getScriptPath(){
    return settings.ais[scriptid]->path;
}

GenObj::GenObj(string newtype, int newlayer, int newcolor, int newcount, int newmin, int newmax){
    layer = newlayer;
    type = newtype;
    count = newcount;
    color = newcolor;
    min = newmin;
    max = newmax;
}


Map::Map(string dirname, string pathname){
    dir  = dirname;
    path = pathname;

    name = "";

    unique = "";

    hash = "";

    soilfile = false;

    rockfile = false;

    basefile = false;

    conffile = false;

    torus = 0;
    limit = 0;

    blob  = true;

    basetype = 1;

    size.set(0, 0);
}

/**
Creates a hash from the map to avoid usage of different map versions by clients
*/
void Map::computeHash(){
    hashwrapper* hasher = new md5wrapper();
    string temp = "";
    if (basefile)
        temp += hasher->getHashFromFile(path+"/"+dir+"/base.png");
    else temp += "_";
    if (soilfile)
        temp += hasher->getHashFromFile(path+"/"+dir+"/soil.png");
    else temp += "_";
    if (rockfile)
        temp += hasher->getHashFromFile(path+"/"+dir+"/rock.png");
    else temp += "_";
    if (conffile)
        temp += hasher->getHashFromFile(path+"/"+dir+"/map.xml");
    else temp += "_";
    hash = hasher->getHashFromString(temp);
    delete(hasher);
}

/**
Gets hexadecimal value and converts it to an integer
*/
int Map::getColor(const char* hexstring){
	int thecolor;
	sscanf(hexstring, "%x", &thecolor);
	return thecolor;
}

Base::Base(int basetype){
    index = -1;
    type  = basetype;
    x     = -1;
    y     = -1;
}

void Settings::load(){
    readLocations();
    readControls();
    readMaps();
    readRobots();
    readAIs();
    readLocals();
}

/**
Loads all settings, maps, robot types, AI scripts and players.
*/
Settings::Settings(){
    gfxres.set(800, 600);
    fullscreen = false;
    sound      = false;
    defaultai  = 0;
    readSettings();
    loc_splashimg       = loader.locateFile("general/splash.png");
}

/**
Loads paths to constant files and directories.
*/
void Settings::readLocations(){
    loc_sndexplode      = loader.locateFile("general/explode.wav");
    loc_sndditch        = loader.locateFile("general/ditch.wav");
//    loc_sndfwd          = loader.locateFile("general/forward.wav");
//    loc_sndbwd          = loader.locateFile("general/backward.wav");
    loc_statusimg       = loader.locateFile("general/status.png");
    loc_bkgimg          = loader.locateFile("general/mud.jpg");
    loc_fontimg         = loader.locateFile("general/font.png");
    loc_nopreviewimg    = loader.locateFile("maps/nopreview.png");
    loc_weaponsdir      = loader.locateDir ("weapons");
    loc_weaponsiconsdir = loader.locateDir ("weapons/icons");
}

/**
Loads settings from file "settings.xml".
*/
void Settings::readSettings(){
    TiXmlDocument doc;

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++)
        if (doc.LoadFile((loader.paths[i]+"/settings.xml"))) break;

    if (i == (int)loader.paths.size()){ cerr << "cannot load settings.xml" << endl; return; }

    TiXmlElement* root = doc.RootElement();

    for (TiXmlNode* node = root->FirstChild(); node; node = node->NextSibling()){
        if (node->Type() == node->TINYXML_ELEMENT){
            TiXmlElement* element = node->ToElement();
            if (!strcmp(element->Value(), "network")){
                for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "client")){
                        clientname.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "host")){
                        hostname.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "port")){
                        attr->QueryIntValue(&port);
                    }
                }
            }else if (!strcmp(element->Value(), "graphics")){
                for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "width")){
                        attr->QueryIntValue(&gfxres.x);
                    }else if (!strcmp(attr->Name(), "height")){
                        attr->QueryIntValue(&gfxres.y);
                    }else if (!strcmp(attr->Name(), "fullscreen")){
                        if (!strcmp(attr->Value(), "true")) fullscreen = true;
                    }
                }
            }else if (!strcmp(element->Value(), "sound")){
                for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "on")){
                        if (!strcmp(attr->Value(), "true")) sound = true;
                    }
                }
            }
        }
    }

}

/**
Saves settings into file "settings.xml".
*/
void Settings::writeSettings(){
    TiXmlDocument doc;
        TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
        doc.LinkEndChild( decl );
    TiXmlElement* root = new TiXmlElement( "settings" );
        doc.LinkEndChild( root );
    TiXmlElement* networkelem = new TiXmlElement( "network" );
    networkelem->SetAttribute("client", clientname);
    networkelem->SetAttribute("host", hostname);
    networkelem->SetAttribute("port", port);
        root->LinkEndChild( networkelem );
    TiXmlElement* graphicselem = new TiXmlElement( "graphics" );
    graphicselem->SetAttribute("width", gfxres.x);
    graphicselem->SetAttribute("height", gfxres.y);
    graphicselem->SetAttribute("fullscreen", fullscreen ? "true" : "false");
        root->LinkEndChild( graphicselem );
    TiXmlElement* soundelem = new TiXmlElement( "sound" );
    soundelem->SetAttribute("on", sound ? "true" : "false");
        root->LinkEndChild( soundelem );

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++)
        if (doc.SaveFile((loader.paths[i]+"/settings.xml"))) break;
    if (i == (int)loader.paths.size()) cerr << "cannot save settings.xml" << endl;
}

/**
Loads controls from file "controls.xml".
*/
void Settings::readControls(){
    TiXmlDocument doc;

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++)
        if (doc.LoadFile((loader.paths[i]+"/controls.xml"))) break;

    if (i == (int)loader.paths.size()){ cerr << "cannot load controls.xml" << endl; return; }

    TiXmlElement* root = doc.RootElement();

    for (TiXmlNode* node = root->FirstChild(); node; node = node->NextSibling()){
        if (node->Type() == node->TINYXML_ELEMENT){
            TiXmlElement* element = node->ToElement();
            if (!strcmp(element->Value(), "global")){
                for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "chat")){
                        attr->QueryIntValue(&controls.chat);
                    }else if (!strcmp(attr->Name(), "quit")){
                        attr->QueryIntValue(&controls.quit);
                    }else if (!strcmp(attr->Name(), "sound")){
                        attr->QueryIntValue(&controls.sound);
                    }else if (!strcmp(attr->Name(), "fullscreen")){
                        attr->QueryIntValue(&controls.fscreen);
					}
                }
            }else if (!strcmp(element->Value(), "spectator")){
                for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "main")){
                        attr->QueryIntValue(&controls.spectator.main);
                    }else if (!strcmp(attr->Name(), "right")){
                        attr->QueryIntValue(&controls.spectator.right);
                    }else if (!strcmp(attr->Name(), "left")){
                        attr->QueryIntValue(&controls.spectator.left);
                    }else if (!strcmp(attr->Name(), "splitscreen")){
                        attr->QueryIntValue(&controls.spectator.split);
					}
                }
            }else{
				PlayerControl* pc = NULL;
				if (!strcmp(element->Value(), "single")) pc = &controls.single;
				else if (!strcmp(element->Value(), "left")) pc = &controls.left;
				else if (!strcmp(element->Value(), "right")) pc = &controls.right;
				
				if (pc){
					for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "up")){
                        attr->QueryIntValue(&pc->up);
                    }else if (!strcmp(attr->Name(), "down")){
                        attr->QueryIntValue(&pc->down);
                    }else if (!strcmp(attr->Name(), "left")){
                        attr->QueryIntValue(&pc->left);
                    }else if (!strcmp(attr->Name(), "right")){
                        attr->QueryIntValue(&pc->right);
                    }else if (!strcmp(attr->Name(), "fire")){
                        attr->QueryIntValue(&pc->fire);
                    }else if (!strcmp(attr->Name(), "weapon")){
                        attr->QueryIntValue(&pc->weapon);
                    }else if (!strcmp(attr->Name(), "teamchat")){
                        attr->QueryIntValue(&pc->teamchat);
                    }else for (int i=0; i < 10; i++) 
						if (!strcmp(attr->Name(), ("w"+intToString(i)).c_str())){
                        attr->QueryIntValue(&pc->w[i]);
					}
                }
				}
            }
        }
    }
}

/**
Saves controls into file "controls.xml".
*/
void Settings::writeControls(){
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
    TiXmlElement* root = new TiXmlElement( "controls" );
    doc.LinkEndChild( root );
    TiXmlElement* globalelem = new TiXmlElement( "global" );
    globalelem->SetAttribute("quit", controls.quit);
    globalelem->SetAttribute("chat", controls.chat);
    globalelem->SetAttribute("sound", controls.sound);
    globalelem->SetAttribute("fullscreen", controls.fscreen);
    root->LinkEndChild( globalelem );
	for (int i = 0; i < 3; i++){
		TiXmlElement* pelem;
		PlayerControl* pc;
		
		if (i == 0){
			pelem = new TiXmlElement( "single" );
			pc = &controls.single;
		}else if (i == 1){
			pelem = new TiXmlElement( "left" );
			pc = &controls.left;
		}else if (i == 2){
			pelem = new TiXmlElement( "right" );
			pc = &controls.right;
		}
		
		pelem->SetAttribute("up", pc->up);
		pelem->SetAttribute("down", pc->down);
		pelem->SetAttribute("left", pc->left);
		pelem->SetAttribute("right", pc->right);
		pelem->SetAttribute("fire", pc->fire);
		pelem->SetAttribute("weapon", pc->weapon);
		pelem->SetAttribute("teamchat", pc->teamchat);
		for (int j = 0; j < 10; j++){
			pelem->SetAttribute("w"+intToString(j), pc->w[j]);
		}
		root->LinkEndChild( pelem );
	}
	
    TiXmlElement* spectelem = new TiXmlElement( "spectator" );
    spectelem->SetAttribute("main", controls.spectator.main);
    spectelem->SetAttribute("right", controls.spectator.right);
    spectelem->SetAttribute("left", controls.spectator.left);
    spectelem->SetAttribute("splitscreen", controls.spectator.split);
    root->LinkEndChild( spectelem );

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++)
        if (doc.SaveFile((loader.paths[i]+"/controls.xml"))) break;
    if (i == (int)loader.paths.size()) cerr << "cannot save controls.xml" << endl;
}

/**
Loads maps from directory "maps".
*/
void Settings::readMaps(){
    TiXmlDocument doc;
    string mainpath;

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++){
        mainpath = loader.paths[i];

    string thesepath = mainpath+"/maps";
    fs::path dirs_path(thesepath);
    fs::directory_iterator end_itr;

    if ( !exists( dirs_path ) ) continue;

    for ( fs::directory_iterator itrs( dirs_path ); itrs != end_itr; ++itrs )
        if (is_directory(itrs->status()) && (itrs->path().leaf().string()[0] != '.') && (itrs->path().leaf().string()[0] != '_')){

        string mname = itrs->path().leaf().string();

        Map* map = new Map(mname, thesepath);
        maps.push_back(map);

        fs::path dir_path(itrs->path());

        for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr ) if (!is_directory(itr->status())){

            if (itr->path().leaf().string() == "base.png"){
                map->basefile = true;
            }else if (itr->path().leaf().string() == "soil.png"){
                map->soilfile = true;
            }else if (itr->path().leaf().string() == "rock.png"){
                map->rockfile = true;
            }else if (itr->path().leaf().string() == "map.xml"){
                map->conffile = true;

                TiXmlDocument doc(itr->path().string());

                doc.LoadFile();
                TiXmlElement* rootelem = doc.RootElement();
                for (TiXmlAttribute* attr = rootelem->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "name")){
                        map->name.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "unique")){
                        map->unique.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "topology")){
                        if (!strcmp(attr->Value(), "torus")) map->torus = 1;
                        else if (!strcmp(attr->Value(), "plane")) map->torus = 0;
                        else if (!strcmp(attr->Value(), "random")) map->torus = 2;
                    }
                }

                for (TiXmlNode* node = rootelem->FirstChild(); node; node = node->NextSibling()){

                    if (node->Type() == node->TINYXML_ELEMENT){
                        TiXmlElement* element = node->ToElement();
                        if (!strcmp(element->Value(), "size")){
                            for (TiXmlAttribute* attr = element->FirstAttribute();
                                attr; attr = attr->Next()){
                                if (!strcmp(attr->Name(), "width")){
                                    attr->QueryIntValue(&map->size.x);
                                }else if (!strcmp(attr->Name(), "height")){
                                    attr->QueryIntValue(&map->size.y);
                                }
                            }
                        }else if (!strcmp(element->Value(), "players")){
                            for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                                if (!strcmp(attr->Name(), "limit")){
                                    attr->QueryIntValue(&map->limit);
                                    map->bases.resize(map->limit);
                                }else if (!strcmp(attr->Name(), "blob")){
                                    if (!strcmp(attr->Value(), "false")) map->blob = false;
                                }else if (!strcmp(attr->Name(), "basetype")){
                                    if (!strcmp(attr->Value(), "none")) map->basetype = 0;
                                    else if (!strcmp(attr->Value(), "2-way")) map->basetype = 1;
                                    else if (!strcmp(attr->Value(), "4-way")) map->basetype = 2;
                                }
                            }
                            for (TiXmlNode* plnode = element->FirstChild();
                                plnode; plnode = plnode->NextSibling()){
                                if (plnode->Type() == plnode->TINYXML_ELEMENT){
                                    TiXmlElement* plelement = plnode->ToElement();
                                    if (!strcmp(plelement->Value(), "base")){
                                        Base* base = new Base(map->basetype);
                                        for (TiXmlAttribute* attr = plelement->FirstAttribute();
                                            attr; attr = attr->Next()){
                                            if (!strcmp(attr->Name(), "index")){
                                                attr->QueryIntValue(&base->index);
                                            }else if (!strcmp(attr->Name(), "x")){
                                                attr->QueryIntValue(&base->x);
                                            }else if (!strcmp(attr->Name(), "y")){
                                                attr->QueryIntValue(&base->y);
                                            }else if (!strcmp(attr->Name(), "type")){
                                                if (!strcmp(attr->Value(), "none")) base->type = 0;
                                                else if (!strcmp(attr->Value(), "2-way")) base->type = 1;
                                                else if (!strcmp(attr->Value(), "4-way")) base->type = 2;
                                            }
                                        }
                                        if (base->index >= 0){
                                            delete(map->bases[base->index]);
                                            map->bases[base->index] = base;
                                        }else delete(base);
                                    }
                                }
                            }
                        }else if (!strcmp(element->Value(), "custom")){
                            for (TiXmlNode* plnode = element->FirstChild();
                                plnode; plnode = plnode->NextSibling()){
                                if (plnode->Type() == plnode->TINYXML_ELEMENT){
                                    TiXmlElement* plelement = plnode->ToElement();
                                    string thetype;
                                    thetype.assign(plelement->Value());
                                    int thelayer = 1;
                                    int thecount = 1;
                                    int thecolor = 1;
                                    int themin   = 1;
                                    int themax   = 100;
                                    for (TiXmlAttribute* attr = plelement->FirstAttribute();
                                        attr; attr = attr->Next()){
                                        if (!strcmp(attr->Name(), "layer")){
                                            attr->QueryIntValue(&thelayer);
                                        }else if (!strcmp(attr->Name(), "count")){
                                            attr->QueryIntValue(&thecount);
                                        }else if (!strcmp(attr->Name(), "color")){
											thecolor = map->getColor(attr->Value());
                                        }else if (!strcmp(attr->Name(), "min")){
                                            attr->QueryIntValue(&themin);
                                        }else if (!strcmp(attr->Name(), "max")){
                                            attr->QueryIntValue(&themax);
                                        }
                                    }
                                    map->generated.push_back(new GenObj(thetype, thelayer, thecolor, thecount, themin, themax));
                                }
                            }
                        }
                    }
                }
            }
        }
        map->computeHash();
    }


    }
    if (maps.size() == 0) cerr << "cannot find any maps" << endl;
}

/**
Loads robot types from directory "robots".
*/
void Settings::readRobots(){
    TiXmlDocument doc;
    string mainpath;

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++){
        mainpath = loader.paths[i];

        string thesepath = mainpath+"/robots";
        fs::path dirs_path(thesepath);
        fs::directory_iterator end_itr;

        if ( !exists( dirs_path ) ) continue;

    for ( fs::directory_iterator itrs( dirs_path ); itrs != end_itr; ++itrs )
        if (is_directory(itrs->status()) && (itrs->path().leaf().string()[0] != '.') && (itrs->path().leaf().string()[0] != '_')){

        string rname = itrs->path().leaf().string();
        RobotType* robottype = new RobotType(rname, thesepath);
        robottype->unique = rname;
        robottype->name = rname;

        bool imgfound = false;

        fs::path dir_path(itrs->path());

        for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
            if (!is_directory(itr->status())){
                string filename = itr->path().leaf().string();

            if (filename == "robot.png"){
                imgfound = true;
                if (!robottype->imgs[robottype->wholePath()+"/robot.png"])
                    robottype->imgs[robottype->wholePath()+"/robot.png"] = new SDL_Surface*[ROTCOUNT];
            }else if (filename == "robot.xml"){
                TiXmlDocument doc(itr->path().string());
                doc.LoadFile();
                TiXmlElement* rootelem = doc.RootElement();
                for (TiXmlAttribute* attr = rootelem->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "name")){
                        robottype->name.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "unique")){
                        robottype->unique.assign(attr->Value());
                    }
                }
                for (TiXmlNode* node = rootelem->FirstChild(); node; node = node->NextSibling()){

                    if (node->Type() == node->TINYXML_ELEMENT){
                        TiXmlElement* element = node->ToElement();
                        if (!strcmp(element->Value(), "movement")){
                            for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                                if (!strcmp(attr->Name(), "regress")){
                                    if (!strcmp(attr->Value(), "true")) robottype-> moveregress = true;
                                }else if (!strcmp(attr->Name(), "blit")){
                                    if (!strcmp(attr->Value(), "true")) robottype-> moveblit = true;
                                }else if (!strcmp(attr->Name(), "steps")){
                                    attr->QueryIntValue(&robottype->movestepscount);
                                    robottype->movesteps = new string[robottype->movestepscount];
                                }
                            }
                            for (TiXmlNode* subnode = element->FirstChild(); subnode; subnode = subnode->NextSibling()){
                                TiXmlElement* subelement = subnode->ToElement();
                                if (!strcmp(subelement->Value(), "img")){
                                    int imgat = -1; string imgid = ""; string imgpath = robottype->dir;
                                    for (TiXmlAttribute* attr = subelement->FirstAttribute(); attr; attr = attr->Next()){
                                        if (!strcmp(attr->Name(), "at")){
                                            attr->QueryIntValue(&imgat);
                                        }else if (!strcmp(attr->Name(), "id")){
                                            imgid.assign(attr->Value());
                                        }else if (!strcmp(attr->Name(), "path")){
                                            imgpath.assign(attr->Value());
                                        }
                                    }
                                    if ((imgat >= 0) && (imgat < robottype->movestepscount)){
                                        string imgfull = robottype->path+"/"+imgpath+"/robot"+imgid+".png";
                                        robottype->movesteps[imgat] = imgfull;
                                        if (!robottype->imgs[imgfull])
                                            robottype->imgs[imgfull] = new SDL_Surface*[ROTCOUNT];
                                    }
                                }
                            }
                        }
                        if (!strcmp(element->Value(), "weapon")){
                            int shi = -1; bool shotblit = false; int shotstepcount = 0;
                            for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                                if (!strcmp(attr->Name(), "index")){
                                    attr->QueryIntValue(&shi);
                                }else if (!strcmp(attr->Name(), "blit")){
                                    if (!strcmp(attr->Value(), "true")) shotblit = true;
                                }else if (!strcmp(attr->Name(), "steps")){
                                    attr->QueryIntValue(&shotstepcount);
                                }
                            }
                            if (shi >= 0){
                                robottype->shotsteps[shi] = new string[shotstepcount];
                                robottype->shotstepscount[shi] = shotstepcount;
                                robottype->shotblit[shi] = shotblit;

                                for (TiXmlNode* subnode = element->FirstChild(); subnode; subnode = subnode->NextSibling()){
                                    TiXmlElement* subelement = subnode->ToElement();
                                    if (!strcmp(subelement->Value(), "img")){
                                        int imgat = -1; string imgid = ""; string imgpath = robottype->dir;
                                        for (TiXmlAttribute* attr = subelement->FirstAttribute(); attr; attr = attr->Next()){
                                            if (!strcmp(attr->Name(), "at")){
                                                attr->QueryIntValue(&imgat);
                                            }else if (!strcmp(attr->Name(), "id")){
                                                imgid.assign(attr->Value());
                                            }else if (!strcmp(attr->Name(), "path")){
                                                imgpath.assign(attr->Value());
                                            }
                                        }
                                        if ((imgat >= 0) && (imgat < robottype->shotstepscount[shi])){
                                            string imgfull = robottype->path+"/"+imgpath+"/robot"+imgid+".png";
                                            robottype->shotsteps[shi][imgat] = imgfull;
                                            if (!robottype->imgs[imgfull])
                                                robottype->imgs[imgfull] = new SDL_Surface*[ROTCOUNT];
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        }

        if (imgfound){
            robottype->id = robottypes.size();
            robottypes.push_back(robottype);
        }else{
            delete(robottype);
        }

    }

    }
    if (robottypes.size() == 0) cerr << "cannot find any robots" << endl;
    else RobotType::acquireImages();

}

/**
Loads AI scripts from directory "scripts".
*/
void Settings::readAIs(){
    TiXmlDocument doc;
    string mainpath;

    for (int i = 0; i < (int)loader.paths.size(); i++){
        mainpath = loader.paths[i];

        string thesepath = mainpath+"/scripts";
        fs::path dirs_path(thesepath);
        fs::directory_iterator end_itr;

        if ( !exists( dirs_path ) ) continue;

    for ( fs::directory_iterator itrs( dirs_path ); itrs != end_itr; ++itrs )
        if (is_directory(itrs->status()) && (itrs->path().leaf().string()[0] != '.') && (itrs->path().leaf().string()[0] != '_')){

            string aidir = itrs->path().leaf().string();
            AIType* aitype = new AIType(aidir, thesepath);

            fs::path dir_path(itrs->path());

            for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
                if (!is_directory(itr->status())){
                if (itr->path().leaf().string() == "main.lua"){
                    if (aitype->dir == "default") defaultai = ais.size();
                    ais.push_back(aitype);
                    break;
                }
            }
    }

    }
    if (ais.size() == 0) cerr << "cannot find any AI scripts" << endl;
}

/**
Loads list of local players from file "players.xml".
*/
void Settings::readLocals(){
    TiXmlDocument doc;

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++)
        if (doc.LoadFile((loader.paths[i]+"/players.xml"))) break;

    if (i == (int)loader.paths.size()){ cerr << "cannot load players.xml" << endl; return; }

    TiXmlElement* rootelem = doc.RootElement();

    for (TiXmlNode* node = rootelem->FirstChild(); node; node = node->NextSibling()){

        if (node->Type() == node->TINYXML_ELEMENT){
            TiXmlElement* element = node->ToElement();
            if (!strcmp(element->Value(), "player")){

                Local* loc = new Local();
                loc->robottype = robottypes[0];

                for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){

                    if (!strcmp(attr->Name(), "name")){
                        loc->name.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "robot")){
                        for (unsigned int i = 0; i < robottypes.size(); i++){
                            if (!strcmp(attr->Value(), robottypes[i]->unique.c_str())){
                                loc->robottype = robottypes[i];
                                break;
                            }
                            if (!strcmp(attr->Value(), robottypes[i]->dir.c_str()))
                                loc->robottype = robottypes[i];
                        }
                    }else if (!strcmp(attr->Name(), "control")){
                        if (!strcmp(attr->Value(), "AI")) loc->artificial = true;
                        else if (!strcmp(attr->Value(), "human")) loc->artificial = false;
                    }else if (!strcmp(attr->Name(), "script")){
                        string tempscript = attr->Value();
                        for (unsigned int i = 0; i < ais.size(); i++)
                            if (tempscript == ais[i]->name) { loc->scriptid = i; break; }
                    }
                }

                if (loc->artificial){
                    if (loc->scriptid == -1) loc->scriptid = defaultai;
                }else
                    if (loc->scriptid >= 0) loc->scriptid = -1;

                locals.push_back(loc);

            }
        }
    }
}

/**
Writes list of local players to file "players.xml".
*/
void Settings::writeLocals(){
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
    TiXmlElement* root = new TiXmlElement( "players" );
    doc.LinkEndChild( root );
    for (unsigned int i = 0; i < locals.size(); i++){
        TiXmlElement* player = new TiXmlElement( "player" );
        player->SetAttribute("name", locals[i]->name);
        player->SetAttribute("robot", locals[i]->robottype->unique);
        player->SetAttribute("control", locals[i]->artificial ? "AI" : "human");
        if (locals[i]->artificial) player->SetAttribute("script", ais[locals[i]->scriptid]->name);
        root->LinkEndChild( player );
    }
    int i;
    for (i = 0; i < (int)loader.paths.size(); i++)
        if (doc.SaveFile((loader.paths[i]+"/players.xml"))) break;
    if (i == (int)loader.paths.size()) cerr << "cannot save players.xml" << endl;
}
