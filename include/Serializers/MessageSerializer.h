#ifndef DISTRIBUTEDMONITOR_MESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_MESSAGESERIALIZER_H

#include <string>
#include <memory>
#include "Serializer.h"

class MessageSerializer : public Serializer<Message> {
public:
    virtual std::string serialize(const Message &message) = 0;

    virtual std::unique_ptr<Message> deserialize(const std::string &string) = 0;
};


#endif //DISTRIBUTEDMONITOR_MESSAGESERIALIZER_H
