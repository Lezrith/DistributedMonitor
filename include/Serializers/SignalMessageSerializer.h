#ifndef DISTRIBUTEDMONITOR_SIGNALMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_SIGNALMESSAGESERIALIZER_H


#include <sstream>
#include "Messages/SignalMessage.h"
#include "Messages/Message.h"
#include "MessageSerializer.h"

class SignalMessageSerializer : public MessageSerializer {
public:
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_SIGNALMESSAGESERIALIZER_H
