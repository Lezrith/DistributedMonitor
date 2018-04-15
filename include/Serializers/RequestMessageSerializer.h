#ifndef DISTRIBUTEDMONITOR_REQUESTMESSAGESERIALIZER_H
#define DISTRIBUTEDMONITOR_REQUESTMESSAGESERIALIZER_H


#include <string>
#include <memory>
#include <sstream>
#include "Messages/Message.h"
#include "Messages/RequestMessage.h"
#include "MessageSerializer.h"

class RequestMessageSerializer : public MessageSerializer {
    std::string serialize(const Message &message) override;

    std::unique_ptr<Message> deserialize(const std::string &string) override;
};


#endif //DISTRIBUTEDMONITOR_REQUESTMESSAGESERIALIZER_H
