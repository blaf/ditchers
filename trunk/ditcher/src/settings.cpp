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

    torus = false;
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

Base::Base(int basetype){
    index = -1;
    type  = basetype;
    x     = -1;
    y     = -1;
}

void Settings::load(){
    readLocations();
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
    fullscreen  = false;
    defaultai = 0;
    readSettings();
    loc_splashimg       = loader.locateFile("splash.png");
}

/**
Loads paths to constant files and directories.
*/
void Settings::readLocations(){
    loc_sndexplode      = loader.locateFile("general/explode.wav");
    loc_sndfwd          = loader.locateFile("general/forward.wav");
    loc_sndbwd          = loader.locateFile("general/backward.wav");
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
        if (node->Type() == node->ELEMENT){
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

    int i;
    for (i = 0; i < (int)loader.paths.size(); i++)
        if (doc.SaveFile((loader.paths[i]+"/settings.xml"))) break;
    if (i == (int)loader.paths.size()) cerr << "cannot save settings.xml" << endl;
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
        if (is_directory(itrs->status()) && (itrs->path().leaf()[0] != '.') && (itrs->path().leaf()[0] != '_')){

        string mname = itrs->path().leaf();

        Map* map = new Map(mname, thesepath);
        maps.push_back(map);

        fs::path dir_path(itrs->path());

        for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr ) if (!is_directory(itr->status())){

            if (itr->path().leaf() == "base.png"){
                map->basefile = true;
            }else if (itr->path().leaf() == "soil.png"){
                map->soilfile = true;
            }else if (itr->path().leaf() == "rock.png"){
                map->rockfile = true;
            }else if (itr->path().leaf() == "map.xml"){
                map->conffile = true;

                TiXmlDocument doc(itr->path().file_string());

                doc.LoadFile();
                TiXmlElement* rootelem = doc.RootElement();
                for (TiXmlAttribute* attr = rootelem->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "name")){
                        map->name.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "unique")){
                        map->unique.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "topology")){
                        if (!strcmp(attr->Value(), "torus")) map->torus = true;
                        else if (!strcmp(attr->Value(), "plane")) map->torus = false;
                    }
                }

                for (TiXmlNode* node = rootelem->FirstChild(); node; node = node->NextSibling()){

                    if (node->Type() == node->ELEMENT){
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
                                if (plnode->Type() == plnode->ELEMENT){
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
        if (is_directory(itrs->status()) && (itrs->path().leaf()[0] != '.') && (itrs->path().leaf()[0] != '_')){

        string rname = itrs->path().leaf();
        RobotType* robottype = new RobotType(rname, thesepath);
        robottype->unique = rname;

        bool imgfound = false;

        fs::path dir_path(itrs->path());

        for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
            if (!is_directory(itr->status())){

            if (itr->path().leaf() == "robot.png"){
                imgfound = true;
            }else if (itr->path().leaf() == "robot.xml"){
                TiXmlDocument doc(itr->path().file_string());
                doc.LoadFile();
                TiXmlElement* rootelem = doc.RootElement();
                for (TiXmlAttribute* attr = rootelem->FirstAttribute(); attr; attr = attr->Next()){
                    if (!strcmp(attr->Name(), "name")){
                        robottype->name.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "unique")){
                        robottype->unique.assign(attr->Value());
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
        if (is_directory(itrs->status()) && (itrs->path().leaf()[0] != '.') && (itrs->path().leaf()[0] != '_')){

            string aidir = itrs->path().leaf();
            AIType* aitype = new AIType(aidir, thesepath);

            fs::path dir_path(itrs->path());

            for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
                if (!is_directory(itr->status())){
                if (itr->path().leaf() == "main.lua"){
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

        if (node->Type() == node->ELEMENT){
            TiXmlElement* element = node->ToElement();
            if (!strcmp(element->Value(), "player")){

                Local* loc = new Local();
                loc->robottype = robottypes[0];

                for (TiXmlAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){

                    if (!strcmp(attr->Name(), "name")){
                        loc->name.assign(attr->Value());
                    }else if (!strcmp(attr->Name(), "robot")){
                        for (unsigned int i = 0; i < robottypes.size(); i++){
                            if (!strcmp(attr->Value(), robottypes[i]->name.c_str())){
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
