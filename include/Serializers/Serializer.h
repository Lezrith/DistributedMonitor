#ifndef DISTRIBUTEDMONITOR_SERIALIZER_H
#define DISTRIBUTEDMONITOR_SERIALIZER_H

#include <string>
#include <memory>

template<typename T>
class Serializer {
public:
    virtual std::string serialize(const T &object) = 0;

    virtual std::unique_ptr<T> deserialize(const std::string &string) = 0;
};

#endif //DISTRIBUTEDMONITOR_SERIALIZER_H
