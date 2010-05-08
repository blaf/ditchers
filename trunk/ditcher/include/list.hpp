#ifndef LIST_HPP_INCLUDED
#define LIST_HPP_INCLUDED
template<class T>class List{	private:	T *data;	bool *active;	int max;	int high;	int vacant;	int current;
    public:	List(int maxSize);
	bool add(T thing);	bool add(T thing, int index);

    int idAdd(T thing);	int idAdd(T thing, int index);

	T byID();
	T byID(int index);

	bool remove(T thing);
	bool remove(int index);

	T begin();	T begin(int* iterator);
	T next();	T next(int* iterator);
	bool end();	bool end(int* iterator);
	int getCurrent();	int getCurrent(int* iterator);
	int getHigh();};
template <class T>List<T>::List(int maxSize){	if (maxSize<1) maxSize=1;	max = maxSize;	data = new T[max];	active = new bool[max];
	for(int i=0;i<max;i++) active[i]=false;	vacant = 0;	high = -1;	current = 0;}
template <class T>bool List<T>::add(T thing){	return (idAdd(thing)>=0);}
template <class T>bool List<T>::add(T thing, int index){	return (idAdd(thing, index)>=0);}
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

template <class T>bool List<T>::remove(int index){	if ((index<0) || (index>high)) return false;	active[index] = false;	if (vacant>index) vacant = index;
	while ((!active[high]) && (high>=0)) high--;	return true;}
template <class T>T List<T>::begin(){	current = -1;	return next();}
template <class T>T List<T>::begin(int* iterator){	(*iterator) = -1;	return next(iterator);}
template <class T>T List<T>::next(){	do{ current++; }while((!end()) && (!active[current]));	if (!end()) return data[current]; else return data[0];}
template <class T>T List<T>::next(int* iterator){	do{ (*iterator)++; }while((!end(iterator)) && (!active[(*iterator)]));	if (!end(iterator)) return data[(*iterator)]; else return data[0];}
template <class T>bool List<T>::end(){	return (current>high);
}
template <class T>bool List<T>::end(int* iterator){	return ((*iterator)>high);
}
template <class T>int List<T>::getHigh(){	return high;}
template <class T>int List<T>::getCurrent(){	return current;}
template <class T>int List<T>::getCurrent(int* iterator){	return (*iterator);}
#endif // LIST_HPP_INCLUDED
