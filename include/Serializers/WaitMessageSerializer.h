#ifndef DISTRIBUTEDMONITOR_WAITMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_WAITMESSAGESERIALIZER_H


#include <sstream>
#include "Messages/Message.h"
#include "Messages/WaitMessage.h"
#include "MessageSerializer.h"

class WaitMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_WAITMESSAGESERIALIZER_H
