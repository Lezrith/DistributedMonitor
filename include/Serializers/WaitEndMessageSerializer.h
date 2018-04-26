#ifndef DISTRIBUTEDMONITOR_WAITENDSERIALIZER_H
#define DISTRIBUTEDMONITOR_WAITENDSERIALIZER_H


#include <sstream>
#include "Messages/Message.h"
#include "Messages/WaitEndMessage.h"
#include "MessageSerializer.h"

class WaitEndMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_WAITENDSERIALIZER_H
