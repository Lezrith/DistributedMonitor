#ifndef DISTRIBUTEDMONITOR_STRINGMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_STRINGMESSAGESERIALIZER_H


#include <Messages/StringMessage.h>
#include "MessageSerializer.h"

class StringMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    Message deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_STRINGMESSAGESERIALIZER_H
