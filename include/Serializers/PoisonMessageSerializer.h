#ifndef DISTRIBUTEDMONITOR_POISONMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_POISONMESSAGESERIALIZER_H


#include <Messages/Message.h>
#include "MessageSerializer.h"

class PoisonMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_POISONMESSAGESERIALIZER_H
