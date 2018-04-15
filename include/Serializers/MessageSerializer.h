#ifndef DISTRIBUTEDMONITOR_SERIALIZER_H
#define DISTRIBUTEDMONITOR_SERIALIZER_H

#include <string>
#include <memory>

class MessageSerializer {
public:
    virtual std::string serialize(const Message &message) = 0;

    virtual std::unique_ptr<Message> deserialize(const std::string &string) = 0;
};


#endif //DISTRIBUTEDMONITOR_SERIALIZER_H
