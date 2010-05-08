#ifndef TEMPLATEHEADER
#define TEMPLATEHEADER

#include <map>
#include <vector>
#include <stack>

using namespace std;

template<class T>
class WrapMap : public map<int, T>{
    public:

    unsigned int max;
    unsigned int provider;

    stack<int> deleted;

    WrapMap<T>() : map<int, T>(){
        provider = 0;
    }

    int newID(){
        if (map<int, T>::size() >= max) return 0;

        do{
            provider++;
            if (provider > max * 2) provider = 1;
        }while (map<int, T>::find(provider) != map<int, T>::end());

        return provider;
    }

    bool freeID(int testid){
        return map<int, T>::find(testid) == map<int, T>::end();
    }

    int add(T newT){
        int newid = newID();
        if (newid > 0){
            newT->id = newid;
            map<int, T>::operator[](newid) = newT;
        }
        return newid;
    }

    int add(int newid, T newT){
        if (freeID(newid)){
            newT->id = newid;
            map<int, T>::operator[](newid) = newT;
        }else newid = 0;
        return newid;
    }

    void defer(int key){
        T value = map<int,T>::operator[](key);
        free(value);
        map<int,T>::operator[](key) = 0;
        deleted.push(key);
    }

    T byID(int theid){
        if (map<int, T>::find(theid) != map<int, T>::end()){
            return map<int, T>::operator[](theid);
        }else return 0;
    }

    void clean(){
        while (!deleted.empty()){
            map<int, T>::erase(deleted.top());
            deleted.pop();
        }
    }

};

template<class T>
class WrapBoth : public vector<T>, public WrapMap<T>{
    public:

    WrapBoth<T>() : vector<T>(), WrapMap<T>(){}

    int add(int index, int newid, T newT, bool blob){

        unsigned int sindex;

        if (blob)
            index = vector<T>::size();
        else{
            if (index < 0){
                    for(unsigned int i = 0; i < vector<T>::size(); i++)
                        if (vector<T>::at(i) == 0){ index = i; break; }
                    if ((index == -1) && (vector<T>::size() < WrapMap<T>::max)) index = vector<T>::size();
            }else{
                sindex = (index < 0) ? 0 : index;
                if ((sindex >= vector<T>::size()) || (vector<T>::at(index) != 0)) index = -1;
            }
        }

        sindex = (index < 0) ? 0 : index;

        if ((index >= 0) && (sindex < WrapMap<T>::max)){
            if (newid == 0) newid = WrapMap<T>::newID();

            if (newid > 0){
                newT->id = newid;
                newT->index = sindex;

                WrapMap<T>::operator[](newid) = newT;

                if (sindex >= vector<T>::size()){
                    while (sindex > vector<T>::size()) vector<T>::push_back(0);
                    vector<T>::push_back(newT);
                }else
                    vector<T>::at(sindex) = newT;
            }

        }

        return newid;
    }

    int add(int index, T newT, bool blob){
        return add(index, 0, newT, blob);
    }

    T byIndex(int theindex){
        unsigned int stheindex = (theindex < 0) ? 0 : theindex;
        if ((theindex >= 0) && (stheindex < vector<T>::size()))
            return vector<T>::at(stheindex);
        else return 0;
    }

    int getIndex(int getid){
        T temp = WrapMap<T>::byID(getid);
        if (temp) return temp->index; else return -1;
    }

    unsigned int size(){
        return vector<T>::size();
    }

    void remove(int index, bool blob){

        T temp = byIndex(index);
        if (!temp) return;

        WrapMap<T>::erase(temp->id);

        if (blob){
            vector<T>::erase(vector<T>::begin() + index);
            for (unsigned int i = index; i < vector<T>::size(); i++)
                if (vector<T>::at(i) != 0) vector<T>::at(i)->index = i;
        }else{
            vector<T>::at(index) = 0;
        }

    }

};

#endif // TEMPLATEHEADER
