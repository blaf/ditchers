#ifndef LIST_HPP_INCLUDED
#define LIST_HPP_INCLUDED
template<class T>
    public:


    int idAdd(T thing);

	T byID();
	T byID(int index);

	bool remove(T thing);
	bool remove(int index);

	T begin();
	T next();
	bool end();
	int getCurrent();
	int getHigh();
template <class T>
	for(int i=0;i<max;i++) active[i]=false;
template <class T>
template <class T>
template <class T>
int List<T>::idAdd(T thing){
	int index = vacant;
	return idAdd(thing, index);
}

template <class T>
int List<T>::idAdd(T thing, int index){
	if ((index >= max) || (index<0) || ((index<=high) && (active[index]))) return -1;
	data[index] = thing;
	active[index] = true;
	if (high<index) high = index;
	while ((vacant<=high) && (active[vacant]==true)) vacant++;
	return index;
}

template <class T>
T List<T>::byID(){
	if ((!end()) && (active[current])) return data[current]; else return 0;
}

template <class T>
T List<T>::byID(int index){
	if ((!end(&index)) && (active[index])) return data[index]; else return 0;
}

template <class T>
bool List<T>::remove(T thing){
	int index = -1;
	if (thing == data[current])
		index = current;
	else
		for (int i=0;i<=high;i++)
			if ((active[i]==true) && (data[i]==thing)){
				index = i;
				break;
			}
    return remove(index);
}

template <class T>
	while ((!active[high]) && (high>=0)) high--;
template <class T>
template <class T>
template <class T>
template <class T>
template <class T>
}
template <class T>
}
template <class T>
template <class T>
template <class T>
#endif // LIST_HPP_INCLUDED