#ifndef DISTRIBUTEDMONITOR_STATEMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_STATEMESSAGESERIALIZER_H


#include <Messages/Message.h>
#include "MessageSerializer.h"

class StateMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_STATEMESSAGESERIALIZER_H
