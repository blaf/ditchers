#include <cstdlib>
#include <iostream>
using namespace std;
#include "path.hpp"
#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

string Directory::wholePath(){
    return path+"/"+dir;
}

/**
 * Locates home directory and adds ./ditchers
 */
string Loader::getHomeDitchers()
{
    return string(getenv("HOME") ? getenv("HOME") : "~")+"/.ditchers";
}

string Loader::locateFile(string file){
    string location = "";
    for (int i = 0; i < (int)paths.size(); i++)
        if (fs::exists(paths[i]+"/"+file) && (fs::is_regular_file(fs::status(paths[i]+"/"+file)))){
            location = paths[i]+"/"+file;
            break;
        }
    if (location == "") cerr << "could not locate the " << file << " file" << endl;
    return location;
}

string Loader::locateDir(string dir){
    string location = "";
    for (int i = 0; i < (int)paths.size(); i++)
        if (fs::exists(paths[i]+"/"+dir) && (fs::is_directory(fs::status(paths[i]+"/"+dir)))){
            location = paths[i]+"/"+dir;
            break;
        }
    if (location == "") cerr << "could not locate the " << dir << " directory" << endl;
    return location;
}

Loader::Loader(){
    fs::create_directory(getHomeDitchers());
    fs::create_directory(getHomeDitchers()+"/maps");
    fs::create_directory(getHomeDitchers()+"/robots");
    fs::create_directory(getHomeDitchers()+"/scripts");

    paths.push_back("./data");
    paths.push_back(getHomeDitchers());
    paths.push_back(".");
    paths.push_back("/usr/share/ditchers");
    paths.push_back("/usr/local/share/ditchers");
    paths.push_back("/usr/share/games/ditchers");
    paths.push_back("/usr/local/share/games/ditchers");
    paths.push_back("/usr/bin/ditchers");
    paths.push_back("/usr/local/bin/ditchers");
    paths.push_back("/etc/ditchers");
}
