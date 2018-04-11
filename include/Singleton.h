#ifndef DISTRIBUTEDMONITOR_SINGLETON_H
#define DISTRIBUTEDMONITOR_SINGLETON_H

#include <cassert>

template<class T>
class Singleton {
public:
    // Threadsafe (see http://en.cppreference.com/w/cpp/language/storage_duration#Static_local_variables)
    static T *getInstance() {
        if (!instance) instance = new T();
        return instance;
    }

private:

    static T *instance;
};

template<class T>
T *Singleton<T>::instance = nullptr;


#endif //DISTRIBUTEDMONITOR_SINGLETON_H
