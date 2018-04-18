#ifndef DISTRIBUTEDMONITOR_ACKNOWLEDGEMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_ACKNOWLEDGEMESSAGESERIALIZER_H


#include <Messages/Message.h>
#include "MessageSerializer.h"

class AcknowledgeMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_ACKNOWLEDGEMESSAGESERIALIZER_H
